/*
 * main.cpp
 *
 *  Created on: 27 Mar 2018
 *      Author: nusuav
 */

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "main.h"
#include "comm_fpga/task_readDDR.h"
#include "ransac/task_ransac.h"
#include "dlg/task_dlg.h"
#include "camera/task_camera.h"

TaskRansac _task_ransac;
TaskReadDDR _task_readddr;
TaskDlg	_task_dlg;
TaskCamera _task_cam;

TaskRansac* GetTaskRansac() { return &_task_ransac; }
TaskReadDDR* GetTaskReadDDR()	 { return &_task_readddr; }
TaskCamera* GetTaskCamera() { return &_task_cam; }

static unsigned int main_loop_cnt = 0;
unsigned long int _tic0 = 0;

void main_loop(int num)
{
	_task_cam.send_activatepulse_wait_signal(INTERVAL_CAM);
//	printf("[Task CAM] complete.\n");

	_task_readddr.send_activatepulse_wait_signal(INTERVAL_DDR);
//	printf("[Task DDR] complete.\n");

	_task_ransac.send_activatepulse_wait_signal(INTERVAL_RANSAC);
//	printf("[Task Ransac] complete.\n");

	_task_dlg.send_activatepulse_wait_signal(INVERVAL_DLG);
//	printf("[Task Dlg] complete.\n");

	main_loop_cnt++;
//	printf("[main] %d: time %.3f \n", main_loop_cnt, ::GetTime());
}

int main(int argc, char *argv[])
{
	_tic0 = tic();

	// Initialize process scheduler
	sched_param param;
	param.sched_priority = 50;
	int error = sched_setscheduler(0, SCHED_RR, &param);
	if (error != 0) {
		printf("Thread scheduling initialization failed %s\n", strerror(errno));
		return 1;
	}

	timespec ts;
	sched_rr_get_interval(0, &ts);
	printf("[main] scheduling quantum %lu us\n", ts.tv_nsec / 1000);

	// Launch all tasks and start running
	_task_cam.create_task(30);
	_task_ransac.create_task(30);
	_task_readddr.create_task(30);
	_task_dlg.create_task(20);

	// Set a periodic timer to run the main thread
	struct sigaction sa;
	struct itimerval timer;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &main_loop;
	sigaction(SIGALRM, &sa, NULL);

	timer.it_value.tv_sec = 1;
	timer.it_value.tv_usec = 0;

	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = INTERVAL_MAIN;

	setitimer(ITIMER_REAL, &timer, NULL);

	while (1) {
		char ch = getchar();
		if (ch == 'q') {
			break;
		}
	}

	// Send exit pulse to all tasks
	_task_cam.send_exitpulse_wait_signal(10000);
	_task_readddr.send_exitpulse_wait_signal(10000);
	_task_ransac.send_exitpulse_wait_signal(10000);
	_task_dlg.send_exitpulse_wait_signal(10000);

	usleep(20000);
	printf("Main loop exit.\n");

	return 0;
}


