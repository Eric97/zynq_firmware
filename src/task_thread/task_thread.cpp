/*
 * TaskThread.cpp
 *
 *  Created on: 28 Mar 2018
 *      Author: nusuav
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "task_thread.h"

extern unsigned long int tic();

TaskThread::TaskThread() :
	_thread_id(0),
	_loop_cnt(-1),
	_task_should_exit(false),
	_tic1(0),
	_tic2(0),
	_task_tic(0),
	_pulse(0),
	_signal(0)
{
	// TODO Auto-generated constructor stub

}

TaskThread::~TaskThread() {
	// TODO Auto-generated destructor stub
}

bool TaskThread::init_task()
{
	pthread_cond_init(&_cond_pulse, NULL);
	pthread_mutex_init(&_mtx_pulse, NULL);

	pthread_cond_init(&_cond_signal, NULL);
	pthread_mutex_init(&_mtx_signal, NULL);

	return true;
}

int TaskThread::create_task(int priority)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	sched_param param;
	pthread_attr_getschedparam(&attr, &param);
	param.sched_priority = priority;
	pthread_attr_setschedparam(&attr, &param);

	int nCreate = pthread_create(&_thread_id, &attr, &task_entry, this);
	if (nCreate != 0) {
		printf("pthread_create error %s\n", strerror(errno));
	}

	return nCreate;
}

bool TaskThread::exit_task()
{
	pthread_cond_destroy(&_cond_pulse);
	pthread_mutex_destroy(&_mtx_pulse);

	pthread_cond_destroy(&_cond_signal);
	pthread_mutex_destroy(&_mtx_signal);

	return true;
}

void TaskThread::task_loop()
{
	for (_loop_cnt = 0; ; _loop_cnt++) {
		int pulse = wait_pulse();
		if (pulse == PULSE_EXIT) {
			break;
		}

		if (pulse == PULSE_ACTIVATE) {
			_tic1 = tic();
			task_loop1();
			_tic2 = tic();
			_task_tic = _tic2 - _tic1;
			send_signal(SIGNAL_COMPLETE);

			printf("[TASK] id %lu, tic1 %lu, tic2 %lu, tic %lu\n", (unsigned long int)_thread_id,
					_tic1, _tic2, _task_tic);
		}
	}

	send_signal(SIGNAL_EXIT);
}

void TaskThread::task_loop1()
{

}

void* TaskThread::task_entry(void* parameters)
{
	TaskThread *p_task = (TaskThread *)parameters;
	if (!p_task->init_task()) return nullptr;

	p_task->task_loop();
	p_task->exit_task();

	return nullptr;
}

void TaskThread::send_activatepulse_wait_signal(int us)
{
	send_pulse(PULSE_ACTIVATE);

	if (us != 0)
		wait_signal(SIGNAL_COMPLETE, us);
}

void TaskThread::send_exitpulse_wait_signal(int us)
{
	send_pulse(PULSE_EXIT);
	wait_signal(SIGNAL_EXIT, us);
}

void TaskThread::send_pulse(int pulse)
{
	if (pthread_mutex_trylock(&_mtx_pulse) != 0) return;

	_pulse = pulse;
	pthread_cond_signal(&_cond_pulse);
	pthread_mutex_unlock(&_mtx_pulse);
}

int TaskThread::wait_pulse()
{
	pthread_mutex_lock(&_mtx_pulse);
	pthread_cond_wait(&_cond_pulse, &_mtx_pulse);

	// Conditional wait released, get the pulse data
	int pulse = _pulse;
	pthread_mutex_unlock(&_mtx_pulse);

	return pulse;
}

void TaskThread::send_signal(int signal)
{
	pthread_mutex_lock(&_mtx_signal);

	_signal = signal;
	pthread_cond_signal(&_cond_signal);
	pthread_mutex_unlock(&_mtx_signal);
}

void TaskThread::wait_signal(int signal, int us)
{
	timespec ts;
	clock_gettime(0, &ts);

	int sec = us / 1000000;
	int nsec = (us % 1000000) * 1000;	// nano seconds

	ts.tv_sec += sec;
	ts.tv_nsec += nsec;

	if (ts.tv_nsec > 1000000000) {
		ts.tv_sec ++;
		ts.tv_nsec -= 1000000000;
	}

	pthread_mutex_lock(&_mtx_signal);
	int wait = pthread_cond_timedwait(&_cond_signal, &_mtx_signal, &ts);

	pthread_mutex_unlock(&_mtx_signal);

	if (wait == ETIMEDOUT) {
		printf("Timeout on task %lu.\n", (unsigned long int)this->_thread_id);
	}
}
