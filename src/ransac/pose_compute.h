#pragma once

#include <vector>
#include "p_match.h"
#include <stdint.h>
#include "matrix.h"

enum Result { UPDATED, FAILED, CONVERGED };

struct Calibration {
	double f;  // focal length (in pixels)
	double cu; // principal point (u-coordinate)
	double cv; // principal point (v-coordinate)
	Calibration() {
		f = 1;
		cu = 0;
		cv = 0;
	}
};

// stereo-specific parameters (mandatory: base)
struct Parameters{
	double  base;             // baseline (meters)
	int32_t ransac_iters;     // number of RANSAC iterations
	double  inlier_threshold; // fundamental matrix inlier threshold
	bool    reweighting;      // lower border weights (more robust to calibration errors)
	Calibration calib;
	Parameters() {
		base = 1.0;
		ransac_iters = 100;
		inlier_threshold = 2.0;
		reweighting = true;
	}
};

class PoseCompute
{
public:
	Parameters param;
	PoseCompute(Parameters _param);
	bool estimationMotion(std::vector<p_match> p_matched);

	Matrix getMotion() const { return _rt_delta; }

private:
	Result updateParameters(std::vector<p_match> &p_matched, std::vector<int32_t> &active,
		std::vector<double> &tr, double step_size, double eps);
	void computeObservations(std::vector<p_match> &p_matched, std::vector<int32_t> &active);
	void computeResidualsAndJacobian(std::vector<double> &tr, std::vector<int32_t> &active);

	Matrix transformationVectorToMatrix (std::vector<double> tr);
	std::vector<int32_t> getInlier(std::vector<p_match> &p_matched, std::vector<double> &tr);
	std::vector<int32_t> inliers;    // inlier set

	Matrix _rt_delta;	/**< Transformation from previous frame to current frame */
	bool _rt_valid; 	/**< Flag to indicate validity of R|T */

	double *X;  //��һʱ�̵���������ά���� X Y Z
	double *Y;
	double *Z;
	double *J;
	double *p_predict;
	double *p_observe;
	double *p_residual;

};
