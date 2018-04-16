/*
 * TaskDlg.h
 *
 *  Created on: 29 Mar 2018
 *      Author: nusuav
 */

#ifndef TASKDLG_H_
#define TASKDLG_H_

#include <stdio.h>
#include "../task_thread/task_thread.h"

class TaskDlg: public TaskThread {
public:
	TaskDlg();
	virtual ~TaskDlg() {}

private:
	FILE *_p_flog;	/**< Log file pointer */

public:
	bool create_log_file();
	bool init_task();
	bool exit_task();
	void task_loop1();
};

#endif /* TASKDLG_H_ */
