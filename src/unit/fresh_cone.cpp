
#include <drawstuff/drawstuff.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include <math.h>
#include "MathUtils.h"
#include "ControlPBP.h"
#include "UnityOde.h"
#include <ode/ode.h>
#include <stdio.h>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <unit/from_robot.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unit/for_double.h>
#include <unit/for_feedback.h>
dsFunctions fn;
using namespace AaltoGames;

float pos_robotx;
float pos_roboty;
float ang_robot_a4;
float ang_robot_a5;


ros::ServiceServer service;
ros::Publisher chatter_pub;
std_msgs::Float32 msg; 



float h_floor_table = 0.65f ;
float h_base = 0.32f ;
float h_first= 0.60f ;
float h_main = 0.12f ;
float h_sphere= 0.08f;
float h_support = 0.14f ;
float stage_dim[3]= {0.46f, 0.4f, 0.052f };


const int nSamples=65; //working 65
//physics simulation time step
float timeStep=1.0f/100.0f;
ControlPBP pbp;
int nTimeSteps=20; // 15 working
const int nStateDimensions=4;
const int nControlDimensions=2;
float minControl[2]={-0.5,-0.5}; //lower sampling bound -0.8 both working
float maxControl[2]={0.5,0.5}; //upper sampling bound 0.8 both working
float controlMean[2]={0,0}; //we're using torque as the control, makes sense to have zero mean
//Square root of the diagonal elements of C_u in the paper, i.e., stdev of a Gaussian prior for control.
//Note that the optimizer interface does not have the C_u as a parameter, and instead uses meand and stdev arrays as parameters.
//The 3D character tests compute the C_u on the Unity side to reduce the number of effective parameters, and then compute the arrays based on it as described to correspond to the products \sigma_0 C_u etc.
float C=0.2;
float controlStd[2]={0.65f*C,0.65f*C}; // 0.65 both working //sqrt(\sigma_{0}^2 C_u) of the paper (we're not explicitly specifying C_u as u is a scalar here). In effect, a "tolerance" for torque minimization in this test
float controlDiffStd[2]={0.9f*C, 0.9f*C}; // 0.9 both working //sqrt(\sigma_{1}^2 C_u) in the pape. In effect, a "tolerance" for angular jerk minimization in this test
float controlDiffDiffStd[2]={18.5f*C,18.5f*C}; //18.5 both working//sqrt(\sigma_{2}^2 C_u) in the paper. A large value to have no effect in this test.
float mutationScale=0.25f;



bool tick=true;
bool debug = false;
bool final_debug = true;
bool debug_inside =false ;


float starting_posX = -0.106; //NEED TO SET IT EVERYTIME
float starting_posY = -0.008;

float last_posX= starting_posX;
float last_posY= starting_posY; 


float vel_estx;
float vel_esty;
float last_vel_estx=0.0;
float last_vel_esty=0.0;


float current_posX, current_posY;


class objects
{
	public:
	dReal body;
	dReal geom;
	dReal joint;
	dReal joint_extra;
	float radius;
	float length;
};

objects stage, support, LinkBall, mainLink,firstLink,base, ball, obs,cone,obs2;



bool robot(unit::for_feedback::Request &req, unit::for_feedback::Response &res)
{

	ros::Time begin = ros::Time::now();

	pos_robotx= req.positionx;
	pos_roboty= req.positiony;
	ang_robot_a4 = req.angle_a4 ;
	ang_robot_a5 = req.angle_a5 ;


	setCurrentOdeContext(0);
	restoreOdeState(0);

///////////////////FEEDBACKS//////////////////
	//joint angle feedbak
	/*
	float angle_sim_a4=odeJointGetHingeAngle(mainLink.joint);
	float angle_sim_a5=odeJointGetHingeAngle(LinkBall.joint);
	
	if (debug) printf(" simlutor  angle before a4:%f  a5:%f  \n", angle_sim_a4, angle_sim_a5);

	float time_step =24  ;
	dReal MaxForce = dInfinity;

	dReal DesiredPosition_a4 = ang_robot_a4;
	dReal DesiredPosition_a5 = ang_robot_a5;

	dReal Error_a4 = angle_sim_a4 - DesiredPosition_a4;
	dReal Error_a5 = angle_sim_a5 - DesiredPosition_a5;

	dReal DesiredVelocity_a4 = -Error_a4*time_step;
	dReal DesiredVelocity_a5 = -Error_a5*time_step;
		
		
		
	if (debug) printf("robot angle  a4:%f ,  a5: %f \n", ang_robot_a4,ang_robot_a5);
	if (debug) printf("Error  a4%f, a5 %f \n", Error_a4, Error_a5);
	if (debug) printf("desired velocity a4%f  a5 %f\n", DesiredVelocity_a4,DesiredVelocity_a5);
	*/
	//odeJointSetHingeParam(mainLink.joint,dParamFMax,dInfinity);
	//odeJointSetHingeParam(mainLink.joint,dParamVel,DesiredVelocity_a4);

	//odeJointSetHingeParam(LinkBall.joint,dParamFMax,dInfinity);
	//odeJointSetHingeParam(LinkBall.joint,dParamVel,DesiredVelocity_a5);
	
	//const dReal *vel_stage;
	//vel_stage =odeBodyGetLinearVel(stage.body);
	//odeBodySetLinearVel(stage.body, 0.0, 0.0, 0.0);
	


	//stepOde(1);
	
	//float angle_f=odeJointGetHingeAngle(mainLink.joint);
	//float angle_f1=odeJointGetHingeAngle(mainLink.joint);
	//if (debug) printf("angle after  a4%f  a5 %f\n", angle_f,angle_f1 );



	//position feedback
	const dReal *pos_first = odeBodyGetPosition(ball.body);
	//printf(" ball position befor setting  x %f  y %f \n", pos_first[0],pos_first[1] );
	//printf(" setting postion x%f , y %f \n",pos_robotx,pos_roboty  );

	odeBodySetPosition(ball.body,pos_robotx,pos_roboty,pos_first[2]);




	//velocity feedback

	//velocity filter

	const dReal *vel;
	float vel_robotX,vel_robotY ;

	float alpha = 0.1;

	current_posX = pos_robotx;
	current_posY = pos_roboty;

	//printf(" current_posX %f current_posY %f\n", current_posX, current_posY );
	vel_robotX = current_posX-last_posX;  //TODO: also get the values in other axes.
	vel_robotY = current_posY-last_posY;

	vel_estx = alpha*last_vel_estx +(1-alpha)*vel_robotX; 
	vel_esty = alpha*last_vel_esty +(1-alpha)*vel_robotY; 


	vel =odeBodyGetLinearVel(ball.body);

	if (debug) printf("vel of the body %f and real ball x%f  y%f \n", vel[1], vel_estx, vel_esty);
	
	odeBodySetLinearVel(ball.body, vel_estx, vel_esty, vel[2]);
	stepOde(1);


///////////////////////////////////////////////////////////////////


	saveOdeState(0);

	//init all trajectories to the master state
	for (int i=0; i<nSamples; i++)
	{
		//activate the context for this sample
		setCurrentOdeContext(i+1);
		//load the state from the master context
		restoreOdeState(0);
		saveOdeState(i+1,0);
	}

	setCurrentOdeContext(0);
	restoreOdeState(0);
	const dReal *pos = odeBodyGetPosition(ball.body);
	float angle=odeJointGetHingeAngle(mainLink.joint);
	float angle_second=odeJointGetHingeAngle(LinkBall.joint);
	
	if(debug) printf("position befor simulate forward  x%f  y %f \n", pos[0],pos[1]);

	float stateVector[4]={pos[0],pos[1], angle, angle_second};
	pbp.startIteration(true,stateVector);

	//simulate forward
	for (int k=0; k<nTimeSteps; k++)
	{
	//signal the start of a planning step
		pbp.startPlanningStep(k);
		for (int i=0; i<nSamples; i++)
		{
			//get control from C-PBP
			float control[2];
			pbp.getControl(i,control);
			//get the mapping from this to previous state (affected by resampling operations)
			int previousStateIdx=pbp.getPreviousSampleIdx(i);
			//simulate to get next state.
			setCurrentOdeContext(i+1);
			//continue the a trajectory (selected in the resampling operation inside ControlPBP)
			restoreOdeState(previousStateIdx+1);
			const dReal *pos = odeBodyGetPosition(ball.body);
			float angle=odeJointGetHingeAngle(mainLink.joint);
			
			dReal Gain = 1;
			dReal MaxForce = dInfinity;
			
			odeJointSetHingeParam(mainLink.joint, dParamFMax, MaxForce);
			odeJointSetHingeParam(mainLink.joint, dParamVel,control[0] );
			odeJointSetHingeParam(LinkBall.joint, dParamFMax, MaxForce);
			odeJointSetHingeParam(LinkBall.joint, dParamVel,control[1] );

			stepOde(1);
			pos = odeBodyGetPosition(ball.body);
			angle=odeJointGetHingeAngle(mainLink.joint);
			float angle_second = odeJointGetHingeAngle(LinkBall.joint);

			const dReal *vel_inside  = odeBodyGetLinearVel(ball.body);
			
			//working 12.5, 12.5, 9.5,9.5
			float cost=squared((0.1-pos[0])*12.5f)+squared((pos[1])*12.5f)+squared(angle*10.5)+squared(angle_second*10.5)+squared(vel_inside[0]*5.0f) + squared(vel_inside[1]*5.0f) ;
			//+squared(control[0]*1.5)+squared(control[1]*1.5) ;//+ squared(vel_robotX*0.05f)+ squared(vel_robotY*0.05f) ;
			
			
			//if (-0.06<pos[1] && pos[1] <0.06 && -0.06<pos[0] && pos[0]<0.06 )
			//{
			//cost = cost+  squared(0.01/pos[0])+squared(0.01/pos[1]);
			//}
			
		//store the state and cost to C-PBP. Note that in general, the stored state does not need to contain full simulation state as in this simple case.
		//instead, one may use arbitrary state features
			float stateVector[4]={pos[0],pos[1], angle, angle_second};
			pbp.updateResults(i,control,stateVector,cost);
		}
		/*
		int j = 0;
		loop : std::cin >> j;
		if ( j != 1)
		goto loop;
		*/

		//save all states, will be used at next step (the restoreOdeState() call above)
		for (int i=0; i<nSamples; i++)
		{
			saveOdeState(i+1,i+1);
		}
		//signal the end of the planning step. this normalizes the state costs etc. for the next step
		pbp.endPlanningStep(k);
	}


	//signal the end of an iteration. this also executes the backwards smoothing pass
	pbp.endIteration();
	//deploy the best control found using the master context
	float control[2];
	pbp.getBestControl(0,control);
	float cost=(float)pbp.getSquaredCost();
	setCurrentOdeContext(0);
	restoreOdeState(0);
	

	odeJointSetHingeParam(mainLink.joint,dParamFMax,dInfinity);
	odeJointSetHingeParam(mainLink.joint,dParamVel,control[0]);
	odeJointSetHingeParam(LinkBall.joint,dParamFMax,dInfinity);
	odeJointSetHingeParam(LinkBall.joint,dParamVel,control[1]);

	stepOde(1);
	saveOdeState(0);


	const dReal *pos1 = odeBodyGetPosition(ball.body);
	float angle1=odeJointGetHingeAngle(mainLink.joint);
	
	//sending the control for real robot
	res.commandx = control[0];
    res.commandy = control[1];


	ros::Time end = ros::Time::now();
	double dt = (begin - end).toSec();
	if (debug) printf(" dt %f\n", dt );

	last_posX = pos_robotx;
	last_posY = pos_roboty; 
	last_vel_estx = vel_estx;
	last_vel_esty = vel_esty;
	
	//printf("rel_vec %f and stage %f\n", re_vec, stage_pos[0]);
	if (final_debug) printf("FINAL Posx %1.3f,posy = %f angle %1.3f, cost=%1.3f, control %f \n",pos1[0],pos1[1],angle1*180/3.1416,cost,control[0]);
	/*
	int j = 0;
	loop : std::cin >> j;
	if ( j != 1)
	goto loop;
	*/
}







int main(int argc, char **argv)
{
	ros::init(argc,argv, "talker");
	ros::NodeHandle n ;


	chatter_pub = n.advertise<std_msgs::Float32>("from_ode", 1);
	service = n.advertiseService("for_feedback", robot);


	initOde(nSamples+1);
	setCurrentOdeContext(ALLTHREADS);
	odeRandSetSeed(0);
	odeSetContactSoftCFM(0);
	odeWorldSetGravity(0, 0, -9.81f);

	//creating stage
	stage.body = odeBodyCreate();
	stage.geom = odeCreateBox(0.46f, 0.38f, 0.05f);
	odeMassSetBoxTotal(stage.body, 1.05, 0.46f,0.38f, 0.05f);
	odeBodySetPosition(stage.body,0,0.0,h_floor_table+h_base+h_sphere+h_support+stage_dim[2]/2);
	odeGeomSetBody(stage.geom,stage.body);
	printf(" Stage body id %f, geom id %f \n", stage.body, stage.geom);
	const dReal *pos;
	pos= odeBodyGetPosition(stage.body);
	printf("stage position x%f y%f z%f \n",pos[0],pos[1], pos[2]);

	
	//creating support //
	//odeMassSetCapsuleTotal(int bodyId, float total_mass, float radius, float length)
	support.radius = 0.05f ;
	support.length = 0.2f;
	support.body = odeBodyCreate();
	support.geom = odeCreateCapsule(0, support.radius, support.length-support.radius);
	odeMassSetCapsuleTotal(support.body, 0.5f, support.radius, support.length-support.radius);
	odeBodySetPosition(support.body,0.0f,0.0f,h_floor_table+h_base+h_sphere+(h_support/2)-stage_dim[2]/2);
	odeGeomSetBody(support.geom,support.body);
	printf("Support capsule body id %f, geom id %f \n", support.body, support.geom);
	pos= odeBodyGetPosition(support.body);
	printf("stage position %f \n", pos[2]);


	//creating ball link
	LinkBall.radius = 0.08f ;
	LinkBall.body = odeBodyCreate();
	LinkBall.geom = odeCreateSphere( LinkBall.radius);
	odeMassSetSphereTotal(LinkBall.body, 0.5,LinkBall.radius);
	odeBodySetPosition(LinkBall.body,0,0,h_floor_table+h_base-support.radius);
	odeGeomSetBody(LinkBall.geom,LinkBall.body);
	printf("Ball link body id %f, geom id %f \n", LinkBall.body, LinkBall.geom);





	//creating main link
	mainLink.radius= 0.05f;
	mainLink.length= 0.12f;
	mainLink.body = odeBodyCreate();
	mainLink.geom = odeCreateBox(0.12, 0.05,0.05);
	odeMassSetBoxTotal(mainLink.body, 0.5,0.12,0.05,0.05);
	odeBodySetPosition(mainLink.body,(mainLink.length+mainLink.radius),0, h_floor_table+h_base-mainLink.radius);
	odeGeomSetBody(mainLink.geom,mainLink.body);
	printf("capsule main link link body id %f, geom id %f \n", mainLink.body, mainLink.geom);


	



	//Creating Ball
	ball.radius = 0.02f ;
	ball.body = odeBodyCreate();
	ball.geom = odeCreateSphere( ball.radius);
	odeMassSetSphereTotal(ball.body, 0.026,ball.radius);
	odeBodySetPosition(ball.body,-0.08,0.00,h_floor_table+h_base+h_sphere+h_support+2*stage_dim[2]);
	odeGeomSetBody(ball.geom,ball.body);
	printf("ball body id %f, geom id %f \n", ball.body, ball.geom);


	/*
	//creating Obstacle
	obs.radius = 0.03f;
	obs.body = odeBodyCreate();
	obs.geom = odeCreateSphere( obs.radius);
	odeMassSetSphereTotal(obs.body, 0.005,obs.radius);
	odeBodySetPosition(obs.body,0,0.0,h_floor_table+h_base+h_sphere+h_support+stage_dim[2]+obs.radius);
	odeGeomSetBody(obs.geom,obs.body);
	printf("capsule obstacle body id %f, geom id %f \n", obs.body, obs.geom);
	*/



	

//creating Obstacle cone first part
	obs.radius = 0.02f;
	obs.body = odeBodyCreate();
	obs.geom = odeCreateBox(0.062,0.031,0.02);
	odeMassSetBoxTotal(obs.body,0.05,0.062,0.031,0.02 );
	odeBodySetPosition(obs.body,0.025,0.045,h_floor_table+h_base+h_sphere+h_support+stage_dim[2]+obs.radius/2);
	odeGeomSetBody(obs.geom,obs.body);
	printf("capsule obstacle body id %f, geom id %f \n", obs.body, obs.geom);


	dQuaternion q1;
	dQFromAxisAndAngle (q1,0,0,1,0.87) ; //dQFromAxisAndAngle (dQuaternion q, dReal ax, dReal ay, dReal az, dReal angle);
	odeGeomSetQuaternion(obs.geom,q1);

	//creating Obstacle cone second part
	cone.radius = 0.02f;
	cone.body = odeBodyCreate();
	cone.geom = odeCreateBox(0.062,0.031,0.02);
	odeMassSetBoxTotal(cone.body,0.05,0.062,0.031,0.02 );
	odeBodySetPosition(cone.body,0.025,-0.045,h_floor_table+h_base+h_sphere+h_support+stage_dim[2]+obs.radius/2);
	odeGeomSetBody(cone.geom,cone.body);
	printf("capsule obstacle body id %f, geom id %f \n", cone.body, cone.geom);

	dQuaternion q2;
	dQFromAxisAndAngle (q2,0,0,1,-0.87) ; //dQFromAxisAndAngle (dQuaternion q, dReal ax, dReal ay, dReal az, dReal angle);
	odeGeomSetQuaternion(cone.geom,q2);

		
		//creating Obstacle cone third part
	obs2.radius = 0.02f;
	obs2.body = odeBodyCreate();
	obs2.geom = odeCreateBox(0.031,0.031,0.02);
	odeMassSetBoxTotal(obs2.body,0.05,0.031,0.031,0.02 );
	odeBodySetPosition(obs2.body,0.0,0.0,h_floor_table+h_base+h_sphere+h_support+stage_dim[2]+obs.radius/2);
	odeGeomSetBody(obs2.geom,obs2.body);
	printf("capsule obstacle body id %f, geom id %f \n", obs2.body, obs2.geom);

	dQuaternion q3;
	dQFromAxisAndAngle (q2,0,0,1,1.57) ; //dQFromAxisAndAngle (dQuaternion q, dReal ax, dReal ay, dReal az, dReal angle);
	odeGeomSetQuaternion(obs2.geom,q2);
    


	// Main link and World
	mainLink.joint =odeJointCreateHinge();
	odeJointAttach(mainLink.joint,mainLink.body,0);
	//odeJointSetHingeAnchor(capsule2.joint[0],-0.5,0,0.60);
	odeJointSetHingeAxis(mainLink.joint,1,0,0);
	//odeJointSetHingeParam(mainLink.joint,dParamHiStop,2.5);
	//odeJointSetHingeParam(mainLink.joint,dParamLoStop,-2.5);


	//Ball link and Main link
	LinkBall.joint =odeJointCreateHinge();
	odeJointAttach(LinkBall.joint,LinkBall.body,mainLink.body);
	//odeJointSetHingeAnchor(capsule3.joint[1],-1.1,0.5,0.60);
	odeJointSetHingeAxis(LinkBall.joint,0,1,0);
	


	// Support and Ball link
	support.joint =odeJointCreateFixed();
	odeJointAttach(support.joint,support.body,LinkBall.body);
	odeJointSetFixed(support.joint);


	//Stage and support link
	stage.joint =odeJointCreateFixed();
	odeJointAttach(stage.joint,stage.body,support.body);
	odeJointSetFixed(stage.joint);

	//Stage and Obstacle
	obs.joint =odeJointCreateFixed();
	odeJointAttach(obs.joint,stage.body,obs.body);
	odeJointSetFixed(obs.joint);


	//Stage and Obstacle
	cone.joint =odeJointCreateFixed();
	odeJointAttach(cone.joint,stage.body,cone.body);
	odeJointSetFixed(cone.joint);

	//Stage and Obstacle
	obs2.joint =odeJointCreateFixed();
	odeJointAttach(obs2.joint,stage.body,obs2.body);
	odeJointSetFixed(obs2.joint);


	


	setCurrentOdeContext(0);
	saveOdeState(0);
	//initialize the optimizer
	pbp.init(nSamples,nTimeSteps,nStateDimensions,nControlDimensions,minControl,maxControl,controlMean,controlStd,controlDiffStd,controlDiffDiffStd,mutationScale,NULL);
	//set further params: portion of "no prior" samples, resampling threshold, whether to use the backwards smoothing pass, and the regularization of the smoothing pass
	pbp.setParams(0.1f,0.5f,true,0.001f);

	while (ros::ok())
	{

        	ros::spinOnce();
	}

	return 0;
}

