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
#include <ros/ros.h>

#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Transform.h>
#include <trajectory_msgs/MultiDOFJointTrajectory.h>
#include <visualization_msgs/Marker.h>

void visualizeWaypoints(std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> points)
{
  ros::NodeHandle nr;
  ros::Publisher vis_pub = nr.advertise<visualization_msgs::Marker>("/visualization_marker", 25, true);
  for (std::size_t idx = 0; idx < points.size(); idx++)
  {
    visualization_msgs::Marker marker;
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time();
    marker.ns = "trajectory";
    marker.id = idx;
    marker.type = visualization_msgs::Marker::SPHERE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = points.at(idx).transforms[0].translation.x;
    marker.pose.position.y = points.at(idx).transforms[0].translation.y;
    marker.pose.position.z = points.at(idx).transforms[0].translation.z;
    marker.pose.orientation.x = points.at(idx).transforms[0].rotation.x;
    marker.pose.orientation.y = points.at(idx).transforms[0].rotation.y;
    marker.pose.orientation.z = points.at(idx).transforms[0].rotation.z;
    marker.pose.orientation.w = points.at(idx).transforms[0].rotation.w;
    marker.scale.x = 0.25;
    marker.scale.y = 0.25;
    marker.scale.z = 0.25;
    marker.color.a = 1.0;
    marker.color.r = 0;
    marker.color.g = 0;
    marker.color.b = points.at(idx).transforms[0].translation.z / 2;
    vis_pub.publish(marker);
    ros::Duration(0.001).sleep();
  }
}

std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> createStraightLine(int number_of_points)
{
  std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> points;
  trajectory_msgs::MultiDOFJointTrajectoryPoint point;
  point.transforms.resize(1);
  geometry_msgs::Transform transform;

  // Point #1
  transform.translation.x = 0;
  transform.translation.y = 0;
  transform.translation.z = 1.2;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #2
  transform.translation.x = 4.5;
  transform.translation.y = 0;
  transform.translation.z = 1.2;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #3
  transform.translation.x = 9;
  transform.translation.y = 0;
  transform.translation.z = 1.2;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Ended
  visualizeWaypoints(points);
  return points;
}

std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> createSpiral(int number_of_points)
{
  std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> points;
  trajectory_msgs::MultiDOFJointTrajectoryPoint point;
  point.transforms.resize(1);
  geometry_msgs::Transform transform;

  // Point #0
  transform.translation.x = 0;
  transform.translation.y = 0;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  double max_height = 1.5;
  float angle = 0;
  int a = 1, b = 1;

  for (int i = 0; i < number_of_points; i += 3)
  {
    angle = 0.05 * i;
    transform.translation.x = (a + b * angle) * cos(angle * M_PI * 2);
    transform.translation.y = (a + b * angle) * sin(angle * M_PI * 2);
    transform.translation.z = 0.5 + float(i) / float(number_of_points) * max_height;
    transform.rotation.x = 0;
    transform.rotation.y = 0;
    transform.rotation.z = 0;
    transform.rotation.w = 1;
    point.transforms[0] = transform;
    points.push_back(point);
  }
  visualizeWaypoints(points);
  return points;
}

std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> createSpiralForWarehouse(int number_of_points)
{
  std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> points;
  trajectory_msgs::MultiDOFJointTrajectoryPoint point;
  point.transforms.resize(1);
  geometry_msgs::Transform transform;

  transform.translation.x = 1.60;
  transform.translation.y = 3.75;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  double max_height = 1.5;
  float angle = 0;
  int a = 1.5, b = 1.5;

  for (int i = 3; i < number_of_points; i += 3)
  {
    angle = 0.05 * i;
    transform.translation.x = 0.20 + (a + b * angle) * cos(angle * M_PI * 2);
    // Small hack to move these points in a safer distance from the obstacle
    transform.translation.y = 3.75 + (a + b * angle) * sin(angle * M_PI * 2);
    transform.translation.z = 0.5 + float(i) / float(number_of_points) * max_height;
    transform.rotation.x = 0;
    transform.rotation.y = 0;
    transform.rotation.z = 0;
    transform.rotation.w = 1;
    point.transforms[0] = transform;
    points.push_back(point);
  }
  visualizeWaypoints(points);
  return points;
}

std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> createMeander(int number_of_points)
{
  std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> points;
  trajectory_msgs::MultiDOFJointTrajectoryPoint point;
  point.transforms.resize(1);
  geometry_msgs::Transform transform;

  // Point #0
  transform.translation.x = 0;
  transform.translation.y = 0;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #1
  transform.translation.x = 0;
  transform.translation.y = -3;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #2
  transform.translation.x = 5;
  transform.translation.y = -3;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #3
  transform.translation.x = 5;
  transform.translation.y = 3;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 1;
  transform.rotation.w = 0;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #4
  transform.translation.x = 1;
  transform.translation.y = 3;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = -0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #5
  transform.translation.x = 1;
  transform.translation.y = -2;
  transform.translation.z = 1;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #6
  transform.translation.x = 3;
  transform.translation.y = -2;
  transform.translation.z = 1;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #7
  transform.translation.x = 3;
  transform.translation.y = 2;
  transform.translation.z = 1;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 1;
  transform.rotation.w = 0;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #8
  transform.translation.x = 2;
  transform.translation.y = 2;
  transform.translation.z = 1;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = -0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #9
  transform.translation.x = 2;
  transform.translation.y = 0;
  transform.translation.z = 2;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #10
  transform.translation.x = 2.5;
  transform.translation.y = 0;
  transform.translation.z = 2;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Ended
  visualizeWaypoints(points);
  return points;
}

std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> createMeanderForWarehouse(int number_of_points)
{
  std::vector<trajectory_msgs::MultiDOFJointTrajectoryPoint> points;
  trajectory_msgs::MultiDOFJointTrajectoryPoint point;
  point.transforms.resize(1);
  geometry_msgs::Transform transform;

  // Point #0
  transform.translation.x = 2;
  transform.translation.y = -0.5;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #1
  transform.translation.x = 2;
  transform.translation.y = 5.5;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 1;
  transform.rotation.w = 0;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #2
  transform.translation.x = -1.5;
  transform.translation.y = 5.5;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = -0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #3
  transform.translation.x = -1.5;
  transform.translation.y = 1.0;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #4
  transform.translation.x = 1.6;
  transform.translation.y = 1.0;
  transform.translation.z = 0.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #5
  transform.translation.x = 1.6;
  transform.translation.y = 4.5;
  transform.translation.z = 1;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 1;
  transform.rotation.w = 0;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #6
  transform.translation.x = -1.0;
  transform.translation.y = 4.5;
  transform.translation.z = 1;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = -0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #7
  transform.translation.x = -1.0;
  transform.translation.y = 2.0;
  transform.translation.z = 1;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #8
  transform.translation.x = 1.3;
  transform.translation.y = 2.0;
  transform.translation.z = 1.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0.7071068;
  transform.rotation.w = 0.7071068;
  point.transforms[0] = transform;
  points.push_back(point);

  // Point #9
  transform.translation.x = 1.6;
  transform.translation.y = 3.3;
  transform.translation.z = 1.5;
  transform.rotation.x = 0;
  transform.rotation.y = 0;
  transform.rotation.z = 0;
  transform.rotation.w = 1;
  point.transforms[0] = transform;
  points.push_back(point);

  // Ended
  visualizeWaypoints(points);
  return points;
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "produce_trajectory");

  ros::NodeHandle nh;
  ros::Publisher trajectory_pub = nh.advertise<trajectory_msgs::MultiDOFJointTrajectory>("/waypoints_smooth", 10, true);

  trajectory_msgs::MultiDOFJointTrajectory msg;
  msg.header.stamp = ros::Time::now();
  msg.header.frame_id = "map";

  std::string type = "straight_line";

  if (type == "straight_line")
    msg.points = createStraightLine(3);
  else if (type == "spiral")
    msg.points = createSpiral(25);
  else if (type == "meander")
    msg.points = createMeander(10);
  else if (type == "spiral_warehouse")
    msg.points = createSpiralForWarehouse(25);
  else if (type == "meander_warehouse")
    msg.points = createMeanderForWarehouse(10);

  trajectory_pub.publish(msg);

  ros::spin();
  return 0;
}
