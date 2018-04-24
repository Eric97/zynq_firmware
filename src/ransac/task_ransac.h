/*
 * task_ransac.h
 *
 *  Created on: 27 Mar 2018
 *      Author: nusuav
 */

#ifndef TASK_RANSAC_H_
#define TASK_RANSAC_H_

#include <pthread.h>
#include <stdio.h>
#include <vector>

#include "pose_compute.h"
#include "../task_thread/task_thread.h"

class TaskRansac : public TaskThread {
public:
	TaskRansac();
	virtual ~TaskRansac() {}

private:
	PoseCompute *_p_pose_compute;	/**< pointer to PoseCompute class */
//	Eigen::Matrix<double, 255, 8> _selectedpoint;
	std::vector<p_match> _p_matched;		/**< vector to store matched feature points */
	FILE *_p_match_file;			/**< file pointer to matching points file */

public:
	Matrix _pose;	/**< Camera pose */

	/**
	 * @brief Task thread initialization
	 *
	 * @return true on success
	 */
	bool init_task();

	/**
	 * @brief Task thread exit
	 *
	 * @return true on success
	 */
	bool exit_task();

	/**
	 * @brief Task loop to carry execution
	 *
	 * @return
	 */
	void task_loop1();

	/*
	 * @brief Print out all feature points in the vector
	 */
	void print_feature_points();

	Matrix get_pose() const { return _pose; }
};

#endif /* TASK_RANSAC_H_ */
