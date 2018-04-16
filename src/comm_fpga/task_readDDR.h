/*
 * TaskReadDDR.h
 *
 *  Created on: 29 Mar 2018
 *      Author: nusuav
 */

#ifndef TASK_READDDR_H_
#define TASK_READDDR_H_

#include <stdint.h>
#include <vector>
#include "../task_thread/task_thread.h"

#define FEATURE_POINTS_REG	"/dev/feature_points_reg"

struct FEATURE_POINT {
	int32_t x1,y1;
	int32_t x2,y2;
	int32_t x3,y3;
	int32_t x4,y4;

	FEATURE_POINT() {}
	FEATURE_POINT(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
			int32_t x3, int32_t y3, int32_t x4, int32_t y4) :
			x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3), x4(x4), y4(y4) {}
};	/**< Stereo feature points: 8 4-bytes integers */

class TaskReadDDR : public TaskThread {
public:
	TaskReadDDR();
	virtual ~TaskReadDDR() {}

public:
	bool init_task();
	bool exit_task();
	void task_loop1();

	unsigned int get_feature_num() const { return _feature_num; }

	int *get_features_arry()	{ return &_feature_pnts_arry[0]; }

	/**
	 * @brief Get the referenct to the feature points vector
	 */
	std::vector<FEATURE_POINT>& get_features() { return _feature_pnts_ddr; }

	/**
	 *
	 */
	void clear_ddr() { _feature_pnts_ddr.clear(); }

private:
	static constexpr int32_t FEATURE_POINTS_DDR_ADDR = 0x1F400000;
	static constexpr int32_t MAP_SIZE = 256 * 8 * 4;	/**< Maximum 256 feature points */
	static constexpr int32_t MAP_MASK = MAP_SIZE - 1;
	static constexpr int	FEATURE_STATUS_READY = 1;

	void *_mapped_base;		/**< Base address after memory mapping */

	FILE *_p_match_file;	/**< File pointer to offline feature file */

	std::vector<FEATURE_POINT> _feature_pnts;		/**< Feature points read from local file */
	std::vector<FEATURE_POINT> _feature_pnts_ddr;	/**< Feature points read from DDR */

	int _feature_pnts_arry[2048];	/** Allocate maximum of 256 * 8 feature points array */
//	pthread_mutex_t	_mtx_feature;	/**< Mutex for feature points */

	int _fd_mem;	/**< File descriptor of shared memory */
	int _fd_fp_reg;	/**< File descriptor of feature points register device */
	unsigned int _feature_num;	/**< Number of feature points stored in DDR */

	/*
	 * @brief Initialize feature points register
	 */
	bool init_fp_reg();

	/**
	 * @brief Remap reserved DDR memory
	 */
	bool init_ddr();

	/**
	 * @brief Read feature points from DDR
	 */
	bool read_from_ddr();



	/**
	 * @brief Write feature points to DDR
	 */
	bool write_to_ddr();

	/**
	 * @brief Read feature points from file
	 */
	bool read_from_file(char *filename);
};

#endif /* TASK_READDDR_H_ */
