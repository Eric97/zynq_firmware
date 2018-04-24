/*
 * camera.cpp
 *
 *  Created on: 24 Apr 2018
 *      Author: nusuav
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <string.h>
#include <errno.h>

#include "task_camera.h"

TaskCamera::TaskCamera() :
	_fd_i2c_cam(-1),
	_fd_i2c_hdmi(-1)
{

}

bool TaskCamera::init_task()
{
	if (!TaskThread::init_task())
		return false;

	// Open I2C camera interface
	_fd_i2c_cam = open("/dev/i2c-1", O_RDWR);
	if (_fd_i2c_cam < 0) {
		printf("Open i2c camera interface failed\n");
		return false;
	}

	// Configure I2C camera
	if (!configure_camera()) {
		printf("Configure camera failed.\n");
		return false;
	}

	// Open I2C HDMI interface
	if (!configure_hdmi()) {
		printf("Configure hdmi failed.\n");
		return false;
	}

	printf("[TaskCamera] init done.\n");
	return true;
}

bool TaskCamera::exit_task()
{
	TaskThread::exit_task();

	printf("[TaskCamera] exit\n");
	return true;
}

void TaskCamera::task_loop1()
{
	return;
}

bool TaskCamera::configure_camera()
{
	unsigned char WriteBuffer[26] = {0};
	unsigned char WriteBuffer1[26] = {0};

	// WriteBuffer to configure camera in acquisition mode
	WriteBuffer[0] = 0x56;
	WriteBuffer[1] = 0x53;
	WriteBuffer[2] = 0x4C;
	WriteBuffer[3] = 0x43;

	WriteBuffer[4] = 0x00;
	WriteBuffer[5] = 0x40;

	WriteBuffer[6] = 0x02;
	WriteBuffer[7] = 0x08;

	WriteBuffer[8] = 0x0C;
	WriteBuffer[9] = 0x00;

	WriteBuffer[10] = 0x03;
	WriteBuffer[11] = 0x00;

	WriteBuffer[12] = 0xB4;
	WriteBuffer[13] = 0x00;
	WriteBuffer[14] = 0x00;
	WriteBuffer[15] = 0x10;
	WriteBuffer[16] = 0x00;
	WriteBuffer[17] = 0x00;
	WriteBuffer[18] = 0x00;
	WriteBuffer[19] = 0x00;

	WriteBuffer[20] = 0x02;
	WriteBuffer[21] = 0x00;
	WriteBuffer[22] = 0x00;
	WriteBuffer[23] = 0x00;

	WriteBuffer[24] = 0x26;
	WriteBuffer[25] = 0x8B;

	// WriteBuffer1 to start image acquisition
	WriteBuffer1[0] = 0x56;
	WriteBuffer1[1] = 0x53;
	WriteBuffer1[2] = 0x4C;
	WriteBuffer1[3] = 0x43;

	WriteBuffer1[4] = 0x00;
	WriteBuffer1[5] = 0x40;

	WriteBuffer1[6] = 0x02;
	WriteBuffer1[7] = 0x08;

	WriteBuffer1[8] = 0x0C;
	WriteBuffer1[9] = 0x00;

	WriteBuffer1[10] = 0x04;
	WriteBuffer1[11] = 0x00;

	WriteBuffer1[12] = 0xAC;
	WriteBuffer1[13] = 0x00;
	WriteBuffer1[14] = 0x00;
	WriteBuffer1[15] = 0x10;
	WriteBuffer1[16] = 0x00;
	WriteBuffer1[17] = 0x00;
	WriteBuffer1[18] = 0x00;
	WriteBuffer1[19] = 0x00;

	WriteBuffer1[20] = 0x01;
	WriteBuffer1[21] = 0x00;
	WriteBuffer1[22] = 0x00;
	WriteBuffer1[23] = 0x00;

	WriteBuffer1[24] = 0x00;
	WriteBuffer1[25] = 0x28;

	// Set slave device address
	if (ioctl(_fd_i2c_cam, I2C_SLAVE, LEFT_CAM_I2C_ADDR) < 0) {
		printf("Failed to access slave device.\n");
		close(_fd_i2c_cam);
		return false;
	}

	// Write to i2c device
	int bytes = write(_fd_i2c_cam, WriteBuffer, 26);
	usleep(100);
	if (bytes < 0) {
		printf("Write WriteBuffer to camera i2c failed %s\n", strerror(errno));
		close(_fd_i2c_cam);
		return false;
	}

	// Read data back to verify
	unsigned char ReadBuffer[18] = {};
	bytes = read(_fd_i2c_cam, ReadBuffer, 18);
	if (bytes < 0) {
		printf("Read from camera i2c failed %s\n", strerror(errno));
		close(_fd_i2c_cam);
		return false;
	}

	for (int iRead = 0; iRead < 18; iRead++) {
		printf("%02x ", (unsigned char)ReadBuffer[iRead]);
	}
	printf("\n");

	// Write WriteBuffer1
	bytes = write(_fd_i2c_cam, WriteBuffer1, 26);
	usleep(100);
	if (bytes < 0) {
		printf("Write WriteBuffer1 to camera i2c failed %s\n", strerror(errno));
		close(_fd_i2c_cam);
		return false;
	}
	// Read back to check
	bytes = read(_fd_i2c_cam, ReadBuffer, 18);
	if (bytes < 0) {
		printf("Read from camera i2c failed %s\n", strerror(errno));
		close(_fd_i2c_cam);
		return false;
	}

	for (int iRead = 0; iRead < 18; iRead++) {
		printf("%02x ", (unsigned char)ReadBuffer[iRead]);
	}
	printf("\n");

	return true;
}

bool TaskCamera::configure_hdmi()
{
	return true;
}
