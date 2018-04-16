/*
 * TaskThread.h
 *
 *  Created on: 28 Mar 2018
 *      Author: nusuav
 */

#ifndef TASK_THREAD_H_
#define TASK_THREAD_H_

#include <pthread.h>

class TaskThread {
public:
	TaskThread();
	virtual ~TaskThread();

protected:
	static constexpr int PULSE_ACTIVATE = 1;
	static constexpr int PULSE_EXIT = 2;
	static constexpr int SIGNAL_COMPLETE = 1;
	static constexpr int SIGNAL_EXIT = 2;

	pthread_t _thread_id;
	unsigned int _loop_cnt;
	bool _task_should_exit;

	unsigned long int _tic1; 	/** Timestamp when task starts */
	unsigned long int _tic2;	/**< Timestamp when task finishes */
	unsigned long int _task_tic;	/**< Task used tic in us */
	pthread_cond_t _cond_pulse;		/**< Conditional variable for pulse */
	pthread_mutex_t _mtx_pulse;		/**< Mutex variable for pulse */
	int _pulse;		/**< Synchronization pulse object from main thread to task threads */

	pthread_cond_t	_cond_signal;	/**< Conditional variable for signal */
	pthread_mutex_t _mtx_signal;	/**< Mutex variable for signal */
	int _signal;	/** Signal object from task threads to notify main thread*/

protected:

	/*
	 * @brief called by task management thread (main)
	 * used for message passing from main thread to task thread
	 */
	void send_pulse(int pulse);

	/*
	 * @brief called by task thread to wait to capture a pulse
	 * used for message passing from main thread to task thread
	 * Main thread will send a activate pulse (PULSE_ACTIVE) to task threads
	 * and wait for
	 */
	int wait_pulse();

	/*
	 * @brief Called by task threads to notify task is done
	 * used for message passing from task threads to main thread
	 * @param[in] signal the inter-thread communication msg
	 */
	void send_signal(int signal);

	/*
	 * @brief Called by main thread to wait for signal from task threads
	 * @param[in] signal task finished signal from task thread to main thread
	 * @param[in] us time in micro seconds the main thread waits for task thread
	 * if task thread execution time is more than us, main thread will start to
	 * schedule the next task thread leaving the current task thread working in
	 * background.
	 */
	void wait_signal(int signal, int us);

public:
	/*
	 * @brief Get the task running time in us
	 */
	inline unsigned long int get_task_tic() const { return _task_tic; }

	const unsigned int get_loop_cnt() const { return _loop_cnt; }

	/*
	 * @brief Main thread sends ACTIVATE pulse to task threads and
	 * then wait for COMPLETION signal from task threads in us
	 */
	void send_activatepulse_wait_signal(int us);

	/*
	 * @brief Main thread sends EXIT pulse to task threads and
	 * then wait for EXIT signal from task threads in us
	 */
	void send_exitpulse_wait_signal(int us);

	virtual bool init_task();
	int create_task(int priority);
	virtual bool exit_task();
	static void* task_entry(void* parameters);
	virtual void task_loop();
	virtual void task_loop1();
};

#endif /* TASK_THREAD_H_ */
