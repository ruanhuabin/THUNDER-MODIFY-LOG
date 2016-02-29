/*******************************************************************************
 * Author: Mingxu Hu
 * Dependency:
 * Test:
 * Execution:
 * Description:
 *
 * Manual:
 * ****************************************************************************/

#ifndef IMAGE_BASE
#define IMAGE_BASE 

#include <functional>
#include <cstring>
#include <cstdio>

#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

#include "Complex.h"
#include "Typedef.h"
#include "Functions.h"

#define FOR_EACH_PIXEL_RL(base) \
    for (size_t i = 0; i < base.sizeRL(); i++)

#define FOR_EACH_PIXEL_FT(base) \
    for (size_t i = 0; i < base.sizeFT(); i++)

#define NEG_RL(base) \
    FOR_EACH_PIXEL_RL(base) \
        base(i) *= -1;

#define ADD_FT(a, b) \
    FOR_EACH_PIXEL_FT(a) \
        a[i] += b[i]

#define SUB_FT(a, b) \
    FOR_EACH_PIXEL_FT(a) \
        a[i] -= b[i]

#define MUL_FT(a, b) \
    FOR_EACH_PIXEL_FT(a) \
        a[i] *= b[i]

#define DIV_FT(a, b) \
    FOR_EACH_PIXEL_FT(a) \
        a[i] /= b[i]

class ImageBase
{
    protected:

        double* _dataRL = NULL;
        Complex* _dataFT = NULL;

        size_t _sizeRL = 0;
        size_t _sizeFT = 0;

    public:

        ImageBase();

        ImageBase(const ImageBase& that);

        ImageBase& operator=(const ImageBase& that);

        const double& getRL(size_t i = 0) const;
        // return a const pointer which points to the i-th element

        const Complex& getFT(size_t i = 0) const;
        // return a const pointer which points to the i-th element
        
        double& operator()(const size_t i);

        Complex& operator[](const size_t i);
        
        bool isEmptyRL() const;
        // check whether _data is NULL or not

        bool isEmptyFT() const;
        // check whether _dataFT is NULL or not

        size_t sizeRL() const;
        // return the total size of this image

        size_t sizeFT() const;
        // return the total size of the Fourier transformed image

        void clear();
        // free the memory

        void clearRL();
        // free the memory storing real space image

        void clearFT();
        // free the memory storing Fourier Transform image
};

void normalise(ImageBase& base);

void negRL(ImageBase& base);

void addFT(ImageBase& dst,
           const ImageBase& src1,
           const ImageBase& src2);

void subFT(ImageBase& dst,
           const ImageBase& src1,
           const ImageBase& src2);

void mulFT(ImageBase& dst,
           const ImageBase& src1,
           const ImageBase& src2);

void divFT(ImageBase& dst,
           const ImageBase& src1,
           const ImageBase& src2);

#endif // IMAGE_BASE 
