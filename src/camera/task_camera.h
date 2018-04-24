/*
 * camera.h
 *
 *  Created on: 24 Apr 2018
 *      Author: nusuav
 */

#ifndef SRC_CAMERA_TASK_CAMERA_H_
#define SRC_CAMERA_TASK_CAMERA_H_

#include "../task_thread/task_thread.h"

//#define LEFT_CAM_I2C_ADDR 0x3D

class TaskCamera : public TaskThread
{
public:
	TaskCamera();
	virtual ~TaskCamera() {}

public:
	bool init_task();
	bool exit_task();
	void task_loop1();

private:
	static constexpr unsigned char LEFT_CAM_I2C_ADDR = 0x3D;
	bool configure_camera();
	bool configure_hdmi();

	int _fd_i2c_cam;	/**< Camera I2C interface file descriptor */
	int _fd_i2c_hdmi;	/**< HDMI I2C interface file descriptor */

};



#endif /* SRC_CAMERA_TASK_CAMERA_H_ */
