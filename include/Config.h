/*******************************************************************************
 * Author: Mingxu Hu
 * Dependency:
 * Test:
 * Execution:
 * Description: some macros
 *
 * Manual:
 * ****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#define VERBOSE_LEVEL_0

#define VERBOSE_LEVEL_1

//#define VERBOSE_LEVEL_2

//#define VERBOSE_LEVEL_3

//#define VERBOSE_LEVEL_4

#define IMG_VOL_BOUNDARY_NO_CHECK

#define MATRIX_BOUNDARY_NO_CHECK

#define NOISE_ZERO_MEAN

//#define RECONSTRUCTOR_MKB_KERNEL

#define RECONSTRUCTOR_TRILINEAR_KERNEL

#define RECONSTRUCTOR_ADD_T_DURING_INSERT

//#define RECONSTRUCTOR_CHECK_C_AVERAGE

#define RECONSTRUCTOR_CHECK_C_MAX

#define RECONSTRUCTOR_CORRECT_CONVOLUTION_KERNEL

#ifdef NOISE_ZERO_MEAN
#define RECONSTRUCTOR_ZERO_MASK
#endif

#ifdef NOISE_ZERO_MEAN
#define OPTIMISER_REFERENCE_ZERO_MASK
#endif

#ifdef NOISE_ZERO_MEAN
#define OPTIMISER_ADJUST_2D_IMAGE_NOISE_ZERO_MEAN
#endif

#define OPTIMISER_RECENTRE_IMAGE_EACH_ITERATION

#define OPTIMISER_COMPRESS_WEIGHTING

#ifdef OPTIMISER_RECENTRE_IMAGE_EACH_ITERATION
#define PARTICLE_CAL_VARI_TRANS_ZERO_MEAN
#endif

#define PARTICLE_TRANS_INIT_GAUSSIAN

//#define PARTILCE_TRANS_INIT_FLAT

#endif // CONFIG_H