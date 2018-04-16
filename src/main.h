/*
 * main.h
 *
 *  Created on: 11 Apr 2018
 *      Author: nusuav
 */

#ifndef MAIN_H_
#define MAIN_H_

extern unsigned long int tic();

#define PRIORITY_MAIN		50
#define PRIORITY_RANSAC		40
#define PRIORITY_DDR		40

// 5 Hz can be achieved in Release build
//#define INTERVAL_MAIN		200000	// 200 ms
//#define INTERVAL_RANSAC		150000	// 100 ms
//#define INTERVAL_DDR		30000	// 30 ms
//#define INVERVAL_DLG		10000	// 30 ms

// 10 Hz if iteration 100 times
#define INTERVAL_MAIN		100000	// 100 ms
#define INTERVAL_RANSAC		70000	// 70 ms
#define INTERVAL_DDR		5000	// 5 ms
#define INVERVAL_DLG		5000	// 5 ms

extern unsigned long int _tic0;
inline double GetTime()	{ return (double)(tic() - _tic0) / 1000000; }

#endif /* MAIN_H_ */
