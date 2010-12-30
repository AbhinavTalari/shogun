/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2010-2011 Alexander Binder
 * Copyright (C) 1999-2009 Fraunhofer Institute FIRST and Max-Planck-Society
 * Copyright (C) 2010-2011 Berlin Institute of Technology
 */

#ifndef _RANDOMFOURIERGAUSSPREPROC__H__
#define _RANDOMFOURIERGAUSSPREPROC__H__

#include <vector>
#include <algorithm>

#include "lib/common.h"
#include "lib/Mathematics.h"
#include "preproc/SimplePreProc.h"

namespace shogun {
class CRandomFourierGaussPreproc: public CSimplePreProc<float64_t> {
	/** @brief Preprocessor CRandomFourierGaussPreproc
	 * implements Random Fourier Features for the Gauss kernel a la Ali Rahimi and Ben Recht Nips2007
	 * after preprocessing the features using them in a linear kernel approximates a gaussian kernel
	 *
	 * approximation quality depends on dimension of feature space, NOT on number of data.
	 *
	 * effectively it requires two parameters for initialization: (A) the dimension of the input features stored in
	 * dim_input_space
	 * (B) the dimension of the output feature space
	 *
	 * in order to make it work there are two ways:
	 * (1) if you have already previously computed random fourier features which you want to use together with
	 * newly computed ones, then you have to take the random coefficients from the previous computation and provide them
	 * via void set_randomcoefficients(...) for the new computation
	 * this case is important for example if you compute separately features on training and testing data in two feature objects
	 *
	 * in this case you have to set
	 * 1a) set_dim_input_space(const int32_t dim);
	 * 1b) void set_dim_feature_space(const int32_t dim);
	 * 1c) void set_randomcoefficients(...)
	 *
	 * (2) if you compute random fourier features from scratch
	 * in this case you have to set
	 * 2a) set_kernelwidth(...)
	 * 2b) set_dim_input_space(const int32_t dim);
	 * 2c) void set_dim_feature_space(const int32_t dim);
	 * 2d) init_randomcoefficients() or apply_to_feature_matrix(...)
	 */

public:
	/** default constructor */
	CRandomFourierGaussPreproc();

	/** alternative constructor */
	CRandomFourierGaussPreproc(const CRandomFourierGaussPreproc & pr);

	CRandomFourierGaussPreproc & operator=(
			const CRandomFourierGaussPreproc & pr);

	/** default destructor
	 * takes care for ::std::vector<float64_t*> randomcoeff_multiplicative;
	 */
	~CRandomFourierGaussPreproc();

	/** default processing routine, inherited from base class
	 * @param CFeatures *f - the features to be processed, must be of type CSimpleFeatures<float64_t>
	 * @return  float64_t * the processed feature matrix from the CSimpleFeatures<float64_t> class
	 * in case (2) (see description above) this routine requires only steps 2a) and 2b), the rest is determined automatically
	 * in case (1) (see description above) this routine requires only step 1b) and 1c)
	 */
	virtual float64_t * apply_to_feature_matrix(CFeatures *f); // ref count fo the feature matrix???


	/** alternative processing routine, inherited from base class
	 * @param float64_t *f - the one feature to be processed
	 * @return float64_t * - processed feature
	 * @return int32_t &len - length of processed feature, is equal to return value of get_dim_feature_space()
	 *
	 * in order to work this routine requires the steps described above under cases (1) or two (2) before calling this routine
	 */
	virtual float64_t * apply_to_feature_vector(float64_t *f, int32_t &len);

	/** inherited from base class
	 * @return C_SIMPLE
	 */
	virtual EFeatureType get_feature_type();

	/** inherited from base class
	 * @return F_DREAL
	 */
	virtual EFeatureClass get_feature_class();

	/** initializer routine
	 * calls set_dim_input_space(const int32_t dim); with the proper value
	 * calls init_randomcoefficients(); this call does NOT override a previous call to void set_randomcoefficients(...) IF and ONLY IF
	 * the dimensions of input AND feature space are equal to the values from the previous call to void set_randomcoefficients(...)
	 * @param CFeatures *f - the features to be processed, must be of type CSimpleFeatures<float64_t>
	 * @return returns true if new random coefficients were generated, false if old ones from a call to set_randomcoefficients(...) are kept
	 */
	virtual bool init(CFeatures *f);

	/**  setter for kernel width
	 * @param kernelwidth to be set
	 */
	void set_kernelwidth(const float64_t );

	/**  getter for kernel width
	 * @return kernelwidth
	 * throws exception if kernelwidth <=0
	 */
	float64_t get_kernelwidth( ) const;

	/**  getter for the random coefficients
	 * necessary for creating random fourier features compatible to the current ones
	 * returns values of internal members 	::std::vector<float64_t> randomcoeff_additive;
	 * and ::std::vector<float64_t*> randomcoeff_multiplicative;
	 */
	void get_randomcoefficients(float64_t ** randomcoeff_additive2,
			float64_t ** randomcoeff_multiplicative2,
			int32_t *dim_feature_space2, int32_t *dim_input_space2) const;

	/**  setter for the random coefficients
	 * necessary for creating random fourier features compatible to the previous ones
	 * sets values of internal members 	::std::vector<float64_t> randomcoeff_additive;
	 * and ::std::vector<float64_t*> randomcoeff_multiplicative;
	 * simply use as input what you got from get_random_coefficients(...)
	 */
	void set_randomcoefficients(float64_t *randomcoeff_additive2,
			float64_t * randomcoeff_multiplicative2,
			const int32_t dim_feature_space2, const int32_t dim_input_space2);

	/** a setter
	 * @param sets the value of protected member dim_input_space
	 * throws a shogun exception if dim<=0
	 */
	void set_dim_input_space(const int32_t dim);

	/** a setter
	 * @param sets the value of protected member dim_feature_space
	 * throws a shogun exception if dim<=0
	 *
	 */
	void set_dim_feature_space(const int32_t dim);

	/** computes new random coefficients IF test_rfinited() evaluates to false
	 * test_rfinited() evaluates to TRUE if void set_randomcoefficients(...) hase been called and the values set by set_dim_input_space(...) and set_dim_feature_space(...) are consistent to the call of void set_randomcoefficients(...)
	 *
	 * throws shogun exception if dim_feature_space <= 0 or dim_input_space <= 0
	 *
	 * @return returns true if test_rfinited() evaluates to false and new coefficients are computed
	 * returns false if test_rfinited() evaluates to true and old random coefficients are kept which were set by a previous call to void set_randomcoefficients(...)
	 *
	 * this function is useful if you want to use apply_to_feature_vector but cannot call init(CFeatures *f)
	 *
	 */
	bool init_randomcoefficients();


	/** a getter
	 * @return the set value of protected member dim_input_space
	 */
	int32_t get_dim_input_space() const;

	/** a getter
	 * @return the set value of protected member dim_feature_space
	 */
	int32_t get_dim_feature_space() const;

	/** inherited from base class
	 * does nothing
	 */
	void cleanup();

protected:

	/**
	 * helper for copy constructor and assignment operator=
	 */
	void copy(const CRandomFourierGaussPreproc & feats); // helper for two constructors


	/** dimension of input features
	 * width of gaussian kernel in the form of exp(-x^2 / (2.0 kernelwidth^2) ) NOTE the 2.0!
	 */
	float64_t kernelwidth;

	/** dimension of input features
	 *
	 */
	int32_t dim_input_space;

	/** actual dimension of input features as set by bool init_randomcoefficients() or void set_randomcoefficients
	 *
	 */
	int32_t cur_dim_input_space;

	/** dimension of output features
	 *
	 */
	int32_t dim_feature_space;

	/**
	 * tests whether rf features have already been initialized
	 */
	bool test_rfinited() const;

	/**
	 * random coefficient
	 * length = dim_feature_space
	 */
	::std::vector<float64_t> randomcoeff_additive;

	/**
	 * random coefficient
	 * length of ::std::vector = dim_feature_space, length of float64_t* = cur_dim_input_space
	 */
	::std::vector<float64_t*> randomcoeff_multiplicative;
};
}
#endif
