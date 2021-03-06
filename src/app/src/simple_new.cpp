
#include "ros/ros.h"
#include "ros/console.h"
#include "math.h"

#include "iostream"
#include "signal.h"

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/array.hpp>

#include "std_msgs/Empty.h"
#include "sensor_msgs/JointState.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Vector3.h"
#include "JointPositions.h"

#include "fricomm.h"

#include <std_srvs/Empty.h>
#include "KUKACommander/set_bool.h"
#include "KUKACommander/set_fri_ctrl.h"
#include "KUKACommander/joint_ptp_motion.h"
//#include "KUKACommander/move_to_cart_pos.h"
#include "KUKACommander/cart_ptp_motion.h"


using namespace std;
using namespace ros;


namespace cmd = ::boost::process;

double m;
double x=0.5;
/*
float a1 = (-2.01*3.1416)/180 ; // -0.350812
float a2 = (100.0*M_PI)/180 ;  // 1.75433
float a3 = (90.00*3.1416)/180 ; //1.5708
float a4 = (85.64*3.1416)/180 ;  //1.4947
float a5 = (-1.12*3.1416)/180 ;   //-0.0195477
float a6 = (116.92*3.1416)/180 ;  // 2.04064
float e1 = (-0.92*3.1416)/180 ;  // -0.0160571
*/


double a1 = (90*3.1416)/180 ; // -0.350812
double a2 = (20.0*M_PI)/180 ;  // 1.75433
double a3 = (-100.00*3.1416)/180 ; //1.5708
double a4 = (0*3.1416)/180 ;  //1.4947
double a5 = (0*3.1416)/180 ;   //1.57
double a6 = (0*3.1416)/180 ;  // 2.04064
double e1 = (0*3.1416)/180 ;  // -0.0160571


void chatterCallback(const sensor_msgs::JointState msg)
{

 m = msg.position[5];
 //ROS_INFO("In chatter: [%f]", msg.position[5]);

}







int main(int argc, char **argv) {
	//ros::init(argc, argv, "iros/pbd/dmp/");
	ros::init(argc, argv, "Simple_new");
	ros::NodeHandle nh { "Simple_new" };
	ros::NodeHandle n;



//not sure what this block does


	std_srvs::Empty foo;
	KUKACommander::set_bool set_bool_true;
	set_bool_true.request.activate = true;


ros::Rate rate(1);

ROS_INFO("check");
 ros::Subscriber sub = n.subscribe("/iros/pbd/dmp/JointPos", 1, chatterCallback);
ros::spinOnce();








ros::ServiceClient activateGravityCompensation = nh.serviceClient<KUKACommander::set_bool>("/KUKACommander/activateGravityCompensation");
	ros::ServiceClient stopGravityCompensation = nh.serviceClient<std_srvs::Empty>("/KUKACommander/stopGravityCompensation");
ros::ServiceClient jointPTPMotion = nh.serviceClient<KUKACommander::joint_ptp_motion>("/KUKACommander/jointPTPMotion");	


	// Create communication objects with desired parameters
	KUKACommander::set_fri_ctrl control_pos_mon;
	control_pos_mon.request.control = FRI_CTRL_POSITION;
	control_pos_mon.request.state = FRI_STATE_CMD;
	ros::ServiceClient setControlModeClient = nh.serviceClient<KUKACommander::set_fri_ctrl>("/KUKACommander/setControlMode");





	// Wait for services to show up
	if(!activateGravityCompensation.exists()) {
	ROS_INFO("Waiting for KUKACommander");
	activateGravityCompensation.waitForExistence (ros::Duration(2, 0));
		if(!activateGravityCompensation.exists()) {
			ROS_ERROR("KUKACommander didn't appear");
			return 0;
		}
		ROS_INFO("KUKACommander has started");
	}


	bool cont = true;

	



setControlModeClient.call(control_pos_mon);

	KUKACommander::joint_ptp_motion motion_srv;

ROS_INFO("boost");

	boost::array<double, 7> pos={a1,a2,e1,a3,a4,a5,a6};

 //boost::array<double, 7> pos={0.0,1.25,0.0,0.0,0.0,a5,2.5};

		double ref =abs( a5); 


		motion_srv.request.position = pos;
		motion_srv.request.speed = 10;
		// Move to start position
		jointPTPMotion.call(motion_srv);
		//ros::Duration(5).sleep();
		//setControlModeClient.call(control_pos_mon);
    











while (ros::ok())
{

//ROS_INFO("repeating");

zero:

ros::spinOnce();
 rate.sleep();
double ref =abs( a5);
   




//if (m< (ref+.2) || m>(ref -.2)  )

//if (abs(m) >(ref-.05) && abs(m) < (ref+.05))

//if (m<-0.005 && m>-0.025)





ROS_INFO("a5 [%f]", a5);
ROS_INFO("m [%f]", m);

//if (abs(abs(a5)-abs(m))<.1 )
if ( m < .5)

{




	if (m ==0)
	{
		goto zero;
	}


a5 = a5+.7; 
ROS_INFO("I am close : [%f]", m);
//ROS_INFO("my target: [%f]", a5);







//setControlModeClient.call(control_pos_mon);

	KUKACommander::joint_ptp_motion motion_srv;

ROS_INFO("boost");

	boost::array<double, 7> pos={a1,a2,e1,a3,a4,a5,a6};

 //boost::array<double, 7> pos={0.0,1.25,0.0,0.0,0.0,a5,2.5};

		; 


		motion_srv.request.position = pos;
		motion_srv.request.speed = 10;
		// Move to start position
		jointPTPMotion.call(motion_srv);
		//ros::Duration(5).sleep();
		//setControlModeClient.call(control_pos_mon);




}

else {


	
ROS_INFO(" in else: [%f]", m);

ros::spinOnce();



 rate.sleep();
//goto check;

//}
//}

}
}
ROS_INFO("break");


  return 0;
}


