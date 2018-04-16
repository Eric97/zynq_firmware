/*
 * common.cpp
 *
 *  Created on: 28 Mar 2018
 *      Author: nusuav
 */

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

unsigned long int tic()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long int t_ms = tv.tv_usec + tv.tv_sec * 1000000;

	return t_ms;
}



