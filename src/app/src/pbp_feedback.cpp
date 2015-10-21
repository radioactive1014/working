#include "ros/ros.h"
#include <Types.hpp>
#include "ros/console.h"

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
#include "KUKACommander/get_bool.h"
#include "KUKACommander/set_fri_ctrl.h"
#include "KUKACommander/joint_ptp_motion.h"
//#include "KUKACommander/move_to_cart_pos.h"
#include "KUKACommander/cart_ptp_motion.h"
#include "KUKACommander/get_fri_state.h"
#include "KUKACommander/move_to_jnt_pos.h"

#include <std_msgs/Float32.h>

#include <std_msgs/Int16.h>
#include "sensor_msgs/JointState.h"
#include <unit/for_double.h>
#include <unit/for_feedback.h>
#include <iostream>
#include <fstream>
#include "app/cam.h"


using namespace std;
using namespace ros;
float robot_a4, robot_a5 ;
float cam_X;
float cam_Y;
float ode_p ;
float ode_pid ;
float control ;
float cam_p;

 unit::for_feedback info;


//debug variables

float d_cam= .1;

//velocity 

double va1 = 0.0 ;
double va2 = 0.0 ;  // 1.75433
double va3 = 0.0 ; ; //1.5708
double va4 = 0.0 ; //1.4947
double va5 = 0.1;  //-0.0195477
double va6 = 0.0 ;;  // 2.04064
double ve1 = 0;  // -0.0160571
double final ;


double target = 20;
double error= 0;
double sum_error= 0;
double dif_error= 0;
double vel ;
double last_error= 0;
double gain ;
double kp ;
double kd ;
double ki ;
double step = .01 ;

int c = 0;





double a1 = (86.17*3.1416)/180 ; // -0.350812
double a2 = (-75.79*M_PI)/180 ;  // 1.75433
double a3 = (-3.85*3.1416)/180 ; //1.5708
double a4 = (88.36*3.1416)/180 ;  //1.4947
double a5 = (91.31*3.1416)/180 ;   //-0.0195477
double a6 = (-23.30*3.1416)/180 ;  // 2.04064
double e1 = (-89.46*3.1416)/180 ;  // -0.0160571

double limit1 = (98*3.1416)/180 ;  // -0.0160571
double limit2 = (72*3.1416)/180 ;  // -0.0160571


namespace cmd = ::boost::process;




void chatterCallback(const sensor_msgs::JointState& msg)
{

 robot_a4 = msg.position[4];	
 robot_a5 = msg.position[5];
//ROS_INFO("From krc: [%f]", msg.position[5]*180/3.1416);
//ROS_INFO("From krc: [%f]", a5*180/3.1416);
}

void odeCallback(const std_msgs::Float32& ode)
{

 ode_p = ode.data;
//ROS_INFO("In ode: [%f]", ode.data);
 

}

void cameraCallbackX(const app::cam& camX)
{

 cam_X = camX.posx;
 cam_Y = camX.posy;

//ROS_INFO("In camera x: [%f], y %f ", cam_X, cam_Y);
 

}




int main(int argc, char **argv) {
	
	ros::init(argc, argv, "Simple");
	ros::NodeHandle nh { "Simple" };
	ros::NodeHandle n;

	ros::Rate loop_rate(24);


	std_srvs::Empty foo;
	KUKACommander::set_bool set_bool_true;
	set_bool_true.request.activate = true;


	//subscribers
	ros::Subscriber sub_cameraX = n.subscribe("/from_cameraX", 1, cameraCallbackX);
	ros::Subscriber sub_ode = n.subscribe("/from_ode", 1, odeCallback);
	ros::Subscriber sub_krc = n.subscribe("/iros/pbd/dmp/JointPos", 1, chatterCallback);
	


  	//clients  

    ros::ServiceClient client = n.serviceClient<unit::for_feedback>("for_feedback");
	


    //publishers
    ros::Publisher chatter_pub = n.advertise<sensor_msgs::JointState >("pid", 1);

    //camera warmup

	for (int j = 0 ; j< 50; j++)

	{
		ros::spinOnce();  //first callback

		printf("%d\n",j );
		//printf("%f\n",cam_p );
		loop_rate.sleep();	
	}



	while (ros::ok())
	{

		//ros::Time begin = ros::Time::now();
		ros::spinOnce();
		float ball_posX,ball_posY ;



		ball_posX = (cam_X-20)/100 ;
		ball_posY = (cam_Y-20)/100 ;
		printf("BALL posx %f, posy %f\n",ball_posX, ball_posY );



		info.request.positionx = ball_posX;
		info.request.positiony = ball_posY;

		info.request.angle_a4 = robot_a4-a4; 
		info.request.angle_a5 = robot_a5-a5; 




		if (client.call(info))
		{
			ROS_INFO("commandx: %f commandy: %f\n", info.response.commandx,info.response.commandy);

		}
		else
		{
			ROS_ERROR("Failed to call service ");
			return 1;
		}


		sensor_msgs::JointState msg;
		float  desired_velX = info.response.commandx  ;
		float  desired_velY = info.response.commandy  ;

		msg.position = {va1,va2,ve1,va3,desired_velX, -desired_velY,va6};

		chatter_pub.publish(msg); 
		loop_rate.sleep();



		}
	return 0;


}	


