/*
* Copyright (c) 2019 Kosmas Tsiakas
*
* GNU GENERAL PUBLIC LICENSE
*    Version 3, 29 June 2007
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <libPF/CRandomNumberGenerator.h>
#include "particle_filter/DroneMovementModel.h"

using namespace std;

DroneMovementModel::DroneMovementModel(ros::NodeHandle* nh, tf2_ros::Buffer* tfBuffer, const std::string& worldFrameID,
                                       const std::string& baseFootprintFrameID, const std::string& baseLinkID)
  : libPF::MovementModel<DroneState>()
  , _tfBuffer(tfBuffer)
  , _tfListener(new tf2_ros::TransformListener(*tfBuffer))
  , _worldFrameID(worldFrameID)
  , _baseFootprintFrameID(baseFootprintFrameID)
  , _baseLinkFrameID(baseLinkID)
  , _odometryReceived(false)
{
  m_RNG = new libPF::CRandomNumberGenerator();
  nh->param<double>("/movement/x_std_dev", _XStdDev, 0.2);
  nh->param<double>("/movement/y_std_dev", _YStdDev, 0.2);
  nh->param<double>("/movement/z_std_dev", _ZStdDev, 0.2);
  nh->param<double>("/movement/roll_std_dev", _RollStdDev, 0.2);
  nh->param<double>("/movement/pitch_std_dev", _PitchStdDev, 0.2);
  nh->param<double>("/movement/yaw_std_dev", _YawStdDev, 0.2);

  nh->param<double>("/x_pos", _xMean, 0);
  nh->param<double>("/y_pos", _yMean, 0);
  nh->param<double>("/z_pos", _zMean, 0);

  nh->param<double>("/roll", _rollMean, 0);
  nh->param<double>("/pitch", _pitchMean, 0);
  nh->param<double>("/yaw", _yawMean, 0);

  ROS_INFO("Drone movement model has been initialized!\n");
}

DroneMovementModel::~DroneMovementModel()
{
  delete m_RNG;
}

void DroneMovementModel::drift(DroneState& state, double dt) const
{
  geometry_msgs::Pose currentPose;
  currentPose.position.x = state.getXPos();
  currentPose.position.y = state.getYPos();
  currentPose.position.z = state.getZPos();

  // Convert RPY to quaternion, to apply the odom transform, then roll back
  tf2::Quaternion currentOrientation;
  currentOrientation.setRPY(state.getRoll(), state.getPitch(), state.getYaw());
  // Convert tf::quaternion to std_msgs::quaternion to be accepted in the odom msg
  currentPose.orientation = tf2::toMsg(currentOrientation.normalize());

  geometry_msgs::TransformStamped odomTransform;

  if (!(lookupOdomTransform(_lastOdomPose.header.stamp + ros::Duration(dt), odomTransform)))
  {
    ROS_WARN("Transform not found! \n");
  }

  applyOdomTransform(odomTransform, currentPose);

  // Set the updated state
  state.setXPos(currentPose.position.x);
  state.setYPos(currentPose.position.y);
  state.setZPos(currentPose.position.z);

  // Set the orientation. Pose has quaternion but we need RPY, convert!
  tf2::Quaternion newOrientation;
  tf2::fromMsg(currentPose.orientation, newOrientation);

  double roll, pitch, yaw;
  tf2::Matrix3x3(newOrientation).getRPY(roll, pitch, yaw);

  state.setRoll(roll);
  state.setPitch(pitch);
  state.setYaw(yaw);
}

void DroneMovementModel::diffuse(DroneState& state, double dt) const
{
  // Transform the N(0,1) distribution that has been created, using the sigma*X + u transform
  // Now our distribution has the previous standard deviation but mean _*Mean, different one for each direction
  state.setXPos(state.getXPos() + (m_RNG->getGaussian(_XStdDev)) * dt);
  // (m_RNG->getGaussian(_XStdDev) + _xMean) * dt); // DOESNT WORK, MOVES FASTER THAN NEEDED
  state.setYPos(state.getYPos() + (m_RNG->getGaussian(_YStdDev)) * dt);
  state.setZPos(state.getZPos() + (m_RNG->getGaussian(_ZStdDev)) * dt);

  state.setRoll(state.getRoll() + (m_RNG->getGaussian(_RollStdDev)) * dt);
  state.setPitch(state.getPitch() + (m_RNG->getGaussian(_PitchStdDev)) * dt);
  state.setYaw(state.getYaw() + (m_RNG->getGaussian(_YawStdDev)) * dt);
}

void DroneMovementModel::setXStdDev(double d)
{
  _XStdDev = d;
}

double DroneMovementModel::getXStdDev() const
{
  return _XStdDev;
}

void DroneMovementModel::setYStdDev(double d)
{
  _YStdDev = d;
}

double DroneMovementModel::getYStdDev() const
{
  return _YStdDev;
}

void DroneMovementModel::setZStdDev(double d)
{
  _ZStdDev = d;
}

double DroneMovementModel::getZStdDev() const
{
  return _ZStdDev;
}

void DroneMovementModel::setRollStdDev(double d)
{
  _RollStdDev = d;
}

double DroneMovementModel::getRollStdDev() const
{
  return _RollStdDev;
}

void DroneMovementModel::setPitchStdDev(double d)
{
  _PitchStdDev = d;
}

double DroneMovementModel::getPitchStdDev() const
{
  return _PitchStdDev;
}

void DroneMovementModel::setYawStdDev(double d)
{
  _YawStdDev = d;
}

double DroneMovementModel::getYawStdDev() const
{
  return _YawStdDev;
}

void DroneMovementModel::setLastOdomPose(geometry_msgs::PoseStamped& odomPose)
{
  _odometryReceived = true;
  if (odomPose.header.stamp < _lastOdomPose.header.stamp)
  {
    ROS_WARN("Trying to store an OdomPose that is older than the current in the MotionModel, ignoring!");
  }
  else
  {
    _lastOdomPose = odomPose;
  }
}

void DroneMovementModel::reset()
{
  _odometryReceived = false;
}

geometry_msgs::PoseStamped DroneMovementModel::getLastOdomPose() const
{
  return _lastOdomPose;
}

geometry_msgs::TransformStamped DroneMovementModel::computeOdomTransform(geometry_msgs::PoseStamped& currentPose) const
{
  tf2::Transform lastOdom;
  tf2::fromMsg(_lastOdomPose.pose, lastOdom);

  tf2::Transform curPose;
  tf2::fromMsg(currentPose.pose, curPose);

  geometry_msgs::TransformStamped output;

  if (_odometryReceived)
  {
    tf2::convert(lastOdom.inverseTimes(curPose), output.transform);
    return output;
  }
  else
  {
    tf2::convert(tf2::Transform::getIdentity(), output.transform);
    return output;
  }
}

void DroneMovementModel::applyOdomTransform(geometry_msgs::TransformStamped& odomTransform,
                                            geometry_msgs::Pose& statePose) const
{
  tf2::Transform odomTF;
  tf2::fromMsg(odomTransform.transform, odomTF);

  tf2::Transform pose;
  tf2::fromMsg(statePose, pose);

  tf2::Transform output;
  output = pose * odomTF;

  tf2::toMsg(output, statePose);
}

bool DroneMovementModel::lookupOdomPose(ros::Time const& t, geometry_msgs::PoseStamped& odomPose) const
{
  geometry_msgs::PoseStamped identity;
  identity.header.frame_id = _baseFootprintFrameID;
  identity.header.stamp = t;

  tf2::toMsg(tf2::Transform::getIdentity(), identity.pose);
  try
  {
    _tfBuffer->transform(identity, odomPose, _worldFrameID, ros::Duration(0.1));
  }
  catch (tf2::TransformException& e)
  {
    ROS_WARN("Failed to compute odom pose, skipping scan (%s)", e.what());
    return false;
  }
  return true;
}

bool DroneMovementModel::lookupOdomTransform(const ros::Time& t, geometry_msgs::TransformStamped& odomTransform) const
{
  geometry_msgs::PoseStamped odomPose;

  if (t <= _lastOdomPose.header.stamp)
  {
    ROS_WARN("Looking up OdomTransform that is %f ms older than the lastOdomPose!",
             (_lastOdomPose.header.stamp - t).toSec() / 1000.0);
  }

  if (!lookupOdomPose(t, odomPose))
    return false;

  odomTransform = computeOdomTransform(odomPose);
  return true;
}

bool DroneMovementModel::lookupTargetToBaseTransform(std::string const& targetFrame, ros::Time const& t,
                                                     geometry_msgs::TransformStamped& localTransform) const
{
  try
  {
    localTransform = _tfBuffer->lookupTransform(targetFrame, _baseLinkFrameID, t);
  }
  catch (tf2::TransformException& e)
  {
    ROS_WARN("Failed to lookup local transform %s ", e.what());
    return false;
  }
  return true;
}
