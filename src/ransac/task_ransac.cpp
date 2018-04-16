/*
 * task_ransac.cpp
 *
 *  Created on: 27 Mar 2018
 *      Author: nusuav
 */

#include <iostream>
#include <unistd.h>

#include "task_ransac.h"
#include "../comm_fpga/task_readDDR.h"

extern unsigned int tic();
extern TaskReadDDR	_task_readddr;

TaskRansac::TaskRansac() :
	_p_pose_compute(nullptr),
	_p_match_file(nullptr)
{

}

bool TaskRansac::init_task()
{
	if (!TaskThread::init_task())
		return false;

	// Initialize parameters
	Parameters param;

	// calibration parameters for sequence 2010_03_09_drive_0019
	param.calib.f = 645.24; // focal length in pixels
	param.calib.cu = 635.96; // principal point (u-coordinate) in pixels
	param.calib.cv = 194.13; // principal point (v-coordinate) in pixels
	param.base = 0.5707; // baseline in meters

	_p_pose_compute = new PoseCompute(param);

	// Load matching points from match file
/*	_p_match_file = fopen("/root/match.csv", "r");
	if (!_p_match_file) {
		printf("[TaskRansac] match file open failed\n");
		return false;
	}
	else {
		// Prepare all the matching points
		float u1c, v1c, u2c, v2c, u1p, v1p, u2p, v2p;
		while (fscanf(_p_match_file, "%f %f %f %f %f %f %f %f", &u1p, &v1p, &u2p, &v2p, &u1c, &v1c, &u2c, &v2c) != EOF)
		{
			_p_matched.push_back(p_match(u1p, v1p, 0, u2p, v2p, 0, u1c, v1c, 0, u2c, v2c, 0));
		}
	}*/

	printf("[TaskRansac] init done.\n");
	return true;
}

bool TaskRansac::exit_task()
{
	TaskThread::exit_task();

	if (_p_pose_compute != nullptr) {
		delete _p_pose_compute;		// TODO: Use smart pointer
	}

	if (_p_match_file != nullptr) {
		delete _p_match_file;
	}

	printf("[TaskRansac] exit\n");
	return true;
}

void TaskRansac::task_loop1()
{
	// Get the feature points from DDR
	unsigned int num_feature = _task_readddr.get_feature_num();
	if (num_feature == 0) return;

	int *ptr_feature = _task_readddr.get_features_arry();

	for (unsigned int i = 0; i < num_feature; i++) {
		_p_matched.push_back(p_match((float)(*ptr_feature), (float)(*(ptr_feature+1)), 0,
				(float)(*(ptr_feature+2)), (float)(*(ptr_feature+3)), 0,
				(float)(*(ptr_feature+4)), (float)(*(ptr_feature+5)), 0,
				(float)(*(ptr_feature+6)), (float)(*(ptr_feature+7)), 0));
		ptr_feature += 8;
	}

//	print_feature_points();
	std::cout << _p_matched.at(175).u1p << " " << _p_matched.at(175).v1p << std::endl;

	std::vector<double> rt_delta = _p_pose_compute->estimationMotion(_p_matched);
	for (int i = 0; i < 6; i++) {
		std::cout << rt_delta[i] << std::endl;
	}

	// Reset feature_ddr and wait idle until next valid feature points data
	_task_readddr.clear_ddr();

	// Clear feature vector at current frame
	_p_matched.clear();

	// Reset feature status register to '0' to start next frame on PL
	// ...

}

void TaskRansac::print_feature_points()
{
	for (std::vector<p_match>::iterator it = _p_matched.begin(); it != _p_matched.end(); it++) {
		printf("%.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f \n",
				(double)it->u1p, (double)it->v1p, (double)it->u2p, (double)it->v2p,
				(double)it->u1c, (double)it->v1c, (double)it->u2c, (double)it->v2c);
	}
}