/*
 * TaskReadDDR.cpp
 *
 *  Created on: 29 Mar 2018
 *      Author: nusuav
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include "task_readDDR.h"

extern unsigned long int tic();

TaskReadDDR::TaskReadDDR() :
	_mapped_base(nullptr),
	_p_match_file(nullptr),
	_feature_pnts{},
	_feature_pnts_ddr{},
	_feature_pnts_arry{},
	_fd_mem(-1),
	_fd_fp_reg(-1),
	_feature_num(0)
{

}

bool TaskReadDDR::init_task()
{
	if (!TaskThread::init_task())
		return false;

	init_fp_reg();
	init_ddr();
//	read_from_file("/root/match.csv");
//	write_to_ddr();
//
//	read_from_ddr();

	printf("[TaskReadDDR] init done.\n");

	return true;
}

bool TaskReadDDR::exit_task()
{
	TaskThread::exit_task();

	// Release the mmaped memory
	if (munmap(_mapped_base, MAP_SIZE) == -1) {
		printf("Cannot unmap memory from user space\n");
		return false;
	}
	close(_fd_mem);

	// Close feature points register handle
	close(_fd_fp_reg);

	if (_p_match_file != NULL) fclose(_p_match_file);

	printf("[TaskReadDDR] exit\n");
	return true;
}

void TaskReadDDR::task_loop1()
{
//	return;
	read_from_ddr();
}

bool TaskReadDDR::init_fp_reg()
{
	_fd_fp_reg = open(FEATURE_POINTS_REG, O_RDWR);
	if (_fd_fp_reg < 0) {
		printf("Open feature register failed\n");
		return false;
	}
	return true;
}

bool TaskReadDDR::init_ddr()
{
//	void *mapped_base;
	off_t base_addr = FEATURE_POINTS_DDR_ADDR;
	_fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (_fd_mem == -1) {
		printf("Cannot open /dev/mem\n");
		return false;
	}
	printf("/dev/mem opened ok!\n");

	_mapped_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _fd_mem, base_addr & ~MAP_MASK);
	if (_mapped_base == (void *) -1) {
		printf("Can't map the memory to user space.\n");
		return false;
	}
	printf("Feature Memory mapped at address %p.\n", _mapped_base);

	return true;
}

bool TaskReadDDR::read_from_ddr()
{
	// Read feature status register from PL
	char readBuf[8] = {0};
	int nread = read(_fd_fp_reg, readBuf, sizeof(int) * 2);	// Read 2 regs: status reg and number reg
	if (nread < 0) {
		printf("Read from feature register failed\n");
		return false;
	}

	int status = *(int *)readBuf;
	if (status != FEATURE_STATUS_READY) {
		printf("Feature status not ready.\n");
		return false;
	}

	_feature_num = *(unsigned int *)(readBuf + 4);

	// Read feature number register from PL
	_feature_pnts_ddr.clear();
	_feature_pnts_ddr.resize(_feature_num);

	// Read into feature points
//	memcpy(&_feature_pnts_ddr[0], _mapped_base, sizeof(FEATURE_POINT) * _feature_num);
	memcpy(&_feature_pnts_arry[0], _mapped_base, sizeof(FEATURE_POINT) * _feature_num);
	printf("Read %d feature points\n", _feature_num);


//	for (std::vector<FEATURE_POINT>::iterator it = _feature_pnts_ddr.begin(); it != _feature_pnts_ddr.end(); it++) {
//			printf("%d %d %d %d %d %d %d %d \n", it->x1, it->y1, it->x2, it->y2, it->x3, it->y3, it->x4, it->y4);
//	}

	return true;
}

bool TaskReadDDR::write_to_ddr()
{
	void *pRtn1 = memcpy(_mapped_base, (void *)&_feature_pnts[0], sizeof(FEATURE_POINT) * _feature_pnts.size());
	printf("pReturn1 %p\n", pRtn1);

	return true;
}

bool TaskReadDDR::read_from_file(char *filename)
{
	// Load matching points from match file
	_p_match_file = fopen(filename, "r");
	if (!_p_match_file) {
		printf("[TaskReadDDR] match file open failed\n");
		return false;
	}
	else {
		// Prepare all the matching points
		float u1c, v1c, u2c, v2c, u1p, v1p, u2p, v2p;
		FEATURE_POINT feature_pnts;
		while (fscanf(_p_match_file, "%f %f %f %f %f %f %f %f", &u1p, &v1p, &u2p, &v2p, &u1c, &v1c, &u2c, &v2c) != EOF)
		{
			_feature_pnts.push_back(FEATURE_POINT((int)u1p, (int)v1p, (int)u2p, (int)v2p, (int)u1c, (int)v1c, (int)u2c, (int)v2c));
		}
		printf("[TaskDDR] Total %d of features are retrieved\n", _feature_pnts.size());
	}

	return true;
}
