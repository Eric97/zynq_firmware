/*
 * TaskDlg.cpp
 *
 *  Created on: 29 Mar 2018
 *      Author: nusuav
 */

#include <stdio.h>
#include <time.h>

#include "task_dlg.h"
#include "../comm_fpga/task_readDDR.h"
#include "../ransac/task_ransac.h"
#include "../main.h"

extern TaskRansac* GetTaskRansac();
extern TaskReadDDR* GetTaskReadDDR();

TaskDlg::TaskDlg() :
	_p_flog(nullptr)
{

}

bool TaskDlg::create_log_file()
{
	time_t t = time(NULL);
	tm *ptm = localtime(&t);

	char file_name[128];
	sprintf(file_name, "log_%02d%02d%02d%02d%02d.log",
			ptm->tm_year, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min);

	_p_flog = fopen(file_name, "w");
	if (_p_flog == NULL) {
		printf("Open log file failed.\n");
		return false;
	}

	return true;
}

bool TaskDlg::init_task()
{
	if (!TaskThread::init_task())
		return false;

	if (!create_log_file())
		return false;

	printf("[TaskDlg] init done.\n");

	return true;
}

bool TaskDlg::exit_task()
{
	TaskThread::exit_task();

	if (_p_flog != NULL) {
		fclose(_p_flog);
	}

	printf("[TaskDlg] exit\n");
	return true;
}

void TaskDlg::task_loop1()
{
	// Log each task execution time
//	printf("[DLG] Time %.3f: ddr %lu, ransac %lu, dlg %lu\n", ::GetTime(),
//			GetTaskReadDDR()->get_task_tic(), GetTaskRansac()->get_task_tic(), get_task_tic());

	fprintf(_p_flog, "%6.3f %lu %lu %lu %.3f %.3f %.3f\n", ::GetTime(), GetTaskReadDDR()->get_task_tic(),
			GetTaskRansac()->get_task_tic(), get_task_tic(),
			GetTaskRansac()->get_pose().val[0][3], GetTaskRansac()->get_pose().val[1][3], GetTaskRansac()->get_pose().val[2][3]);
}

