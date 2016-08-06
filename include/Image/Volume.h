/*******************************************************************************
 * Author: Mingxu Hu
 * Dependency:
 * Test:
 * Execution:
 * Description:
 *
 * Manual:
 * ****************************************************************************/

#ifndef VOLUME_H
#define VOLUME_H

#include <cmath>
#include <cstdlib>
#include <vector>

#include "omp_if.h"
#include "Typedef.h"
#include "Error.h"
#include "Complex.h"
#include "Logging.h"

#include "ImageBase.h"
#include "BMP.h"
#include "Image.h"
#include "Interpolation.h"
#include "Functions.h"
#include "TabFunction.h"
#include "Coordinate5D.h"
#include "Transformation.h"

#define VOLUME_SUB_SPHERE_FT(a) \
    for (int k = MAX(-_nSlc / 2, floor(iSlc - a)); \
             k <= MIN(_nSlc / 2 - 1, ceil(iSlc + a)); \
             k++) \
        for (int j = MAX(-_nRow / 2, floor(iRow - a)); \
                 j <= MIN(_nRow / 2 - 1, ceil(iRow + a)); \
                 j++) \
            for (int i = MAX(-_nCol / 2, floor(iCol - a)); \
                     i <= MIN(_nCol / 2, ceil(iCol + a)); \
                     i++)

#define VOLUME_FOR_EACH_PIXEL_RL(that) \
    for (int k = -that.nSlcRL() / 2; k < that.nSlcRL() / 2; k++) \
        for (int j = -that.nRowRL() / 2; j < that.nRowRL() / 2; j++) \
            for (int i = -that.nColRL() / 2; i < that.nColRL() / 2; i++) \

#define VOLUME_FOR_EACH_PIXEL_FT(that) \
    for (int k = -that.nSlcRL() / 2; k < that.nSlcRL() / 2; k++) \
        for (int j = -that.nRowRL() / 2; j < that.nRowRL() / 2; j++) \
            for (int i = 0; i <= that.nColRL() / 2; i++)

inline bool conjHalf(int& iCol,
                     int& iRow,
                     int& iSlc)
{
    if (iCol >= 0) return false;

    iCol *= -1;
    iRow *= -1;
    iSlc *= -1;

    return true;
};

inline bool conjHalf(double& iCol,
                     double& iRow,
                     double& iSlc)
{
    if (iCol >= 0) return false;

    iCol *= -1;
    iRow *= -1;
    iSlc *= -1;

    return true;
}

class Volume : public ImageBase
{
    MAKE_DEFAULT_MOVE(Volume)

    public:

        /**
         * number of columns of this volume
         */
        int _nCol = 0;

        /**
         * number of rows of this volume
         */
        int _nRow = 0;

        /**
         * number of slices of this volume
         */
        int _nSlc = 0;

    public:

        /**
         * default constructor
         */
        Volume();

        /**
         * constructor
         *
         * @param nCol number of columns of this volume
         * @param nRow number of rows of this volume
         * @param nSlc number of slices of this volume
         * @param space the space this volume allocating, where RL_SPACE stands
         *              for the real space and FT_SPACE stands for the Fourier
         *              space
         */
        Volume(const int nCol,
               const int nRow,
               const int nSlc,
               const int space);

        /**
         * The deconstructor will automatically free all allocated space.
         */
        ~Volume();

        /**
         * This function allocates a piece of memory in a certain space.
         * @param space the space this volume allocating, where RL_SPACE stands
         *              for the real space and FT_SPACE stands for the Fourier
         *              space
         */
        void alloc(const int space);

        /* This function allocates a piece of memory in a certain space.
         *
         * @param nCol number of columns of this volume
         * @param nRow number of rows of this volume
         * @param nSlc number of slices of this volume
         * @param space the space this volume allocating, where RL_SPACE stands
         *              for the real space and FT_SPACE stands for the Fourier
         *              space
         */
        void alloc(const int nCol,
                   const int nRow,
                   const int nSlc,
                   const int space);

        /**
         * This function returns the number of columns of this volume in real
         * space.
         */
        inline int nColRL() const { return _nCol; };

        /**
         * This function returns the number of rows of this volume in real
         * space.
         */
        inline int nRowRL() const { return _nRow; };

        /**
         * This function returns the number of rows of this volume in real
         * space.
         */
        inline int nSlcRL() const { return _nSlc; };

        /**
         * This function returns the number of columns of this volume in Fourier
         * space.
         */
        inline int nColFT() const { return _nCol / 2 + 1; };

        /**
         * This function returns the number of rows of this volume in Fourier
         * space.
         */
        inline int nRowFT() const { return _nRow; };

        /**
         * This function returns the number of slices of this volume in Fourier
         * space.
         */
        inline int nSlcFT() const { return _nSlc; };

        /**
         * This function gets the value of the voxel in real space at a given
         * coordinate.
         *
         * @param iCol the index of the column of this voxel in real space
         * @param iRow the index of the row of this voxel in real space
         * @param iSlc the index of the slice of this voxel in real space
         */
        double getRL(const int iCol,
                     const int iRow,
                     const int iSlc) const;

        /**
         * This function sets the value of the voxel in real space at a given
         * coordinate.
         *
         * @param value the value of the voxel in real space
         * @param iCol the index of the column of this voxel in real space
         * @param iRow the index of the row of this voxel in real space
         * @param iSlc the index of the slice of this voxel in real space
         */
        void setRL(const double value,
                   const int iCol,
                   const int iRow,
                   const int iSlc);

        /**
         * This function add a certain value on the voxel in real space at a
         * given coordinate.
         *
         * @param value the value of the voxel in real space
         * @param iCol the index of the column of this voxel in real space
         * @param iRow the index of the row of this voxel in real space
         * @param iSlc the index of the slice of this voxel in real space
         */
        void addRL(const double value,
                   const int iCol,
                   const int iRow,
                   const int iSlc);

        /**
         * This function gets the value of the voxel in Fourier space at a given
         * coordinate.
         *
         * @param iCol the index of the column of this voxel in Fourier space
         * @param iRow the index of the row of this voxel in Fourier space
         * @param iSlc the index of the slice of this voxel in real space
         */
        Complex getFT(int iCol,
                      int iRow,
                      int iSlc) const;

        Complex getFTHalf(const int iCol,
                          const int iRow,
                          const int iSlc) const;

        /**
         * This function sets the value of the voxel in Fourier space at a given
         * coordinate.
         *
         * @param value the value of voxel in Fourier space
         * @param iCol the index of the column of this voxel in Fourier space
         * @param iRow the index of the row of this voxel in Fourier space
         * @param iSlc the index of the slice of this voxel in real space
         */
        void setFT(const Complex value,
                   int iCol,
                   int iRow,
                   int iSlc);

        /**
         * This function sets the value of the voxel in Fourier space at a given
         * coordinate.
         *
         * @param value the value of the voxel in Fourier space
         * @param iCol the index of the column of this voxel in Fourier space
         * @param iRow the index of the row of this voxel in Fourier space
         * @param iSlc the index of the slice of this voxel in real space
         */
        void setFTHalf(const Complex value,
                       const int iCol,
                       const int iRow,
                       const int iSlc);
        
        /**
         * This function adds a certain value on a voxel in Fourier space at a
         * given coordinate.
         *
         * @param value the value of the voxel
         * @param iCol the index of the column of this voxel in Fourier space
         * @param iRow the index of the row of this voxel in Fourier space
         * @param iSlc the index of the slice of this voxel in real space
         */
        void addFT(const Complex value,
                   int iCol,
                   int iRow,
                   int iSlc);

        /**
         * This function returns the value of an unregular voxel in speace spce
         * by interpolation.
         *
         * @param iCol the index of the column of this unregular voxel in
         *             Fourier space
         * @param iRow the index of the row of this unregular voxel in
         *             Fourier space
         * @param iSlc the index of the slice of this unregular voxel in
         *             Fourier space
         * @param interp indicate the type of interpolation, where INTERP_NEAREST
         *               stands for the nearest point interpolation,
         *               INTERP_LINEAR stands for the trilinear interpolation
         *               and INTERP_SINC stands for the sinc interpolation
         */
        double getByInterpolationRL(const double iCol,
                                    const double iRow,
                                    const double iSlc,
                                    const int interp) const;

        /**
         * This function returns the value of an unregular voxel in Fourier
         * space by interpolation.
         *
         * @param iCol the index of the column of this unregular voxel in
         *             real space
         * @param iRow the index of the row of this unregular voxel in
         *             real space
         * @param iSlc the index of the slice of this unregular voxel in
         *             real space
         * @param interp indicate the type of interpolation, where INTERP_NEAREST
         *               stands for the nearest point interpolation,
         *               INTERP_LINEAR stands for the trilinear interpolation
         *               and INTERP_SINC stands for the sinc interpolation
         */
        Complex getByInterpolationFT(double iCol,
                                     double iRow,
                                     double iSlc,
                                     const int interp) const;

        /**
         * This function adds a certain value on an unregular voxel in Fourier
         * space by a kernal of Modified Kaiser Bessel Function.
         *
         * @param value the value to be added
         * @param iCol the index of the column of this unregular voxel in
         *             real space
         * @param iRow the index of the row of this unregular voxel in
         *             real space
         * @param iSlc the index of the slice of this unregular voxel in
         *             real space
         * @param a the radius of Modified Kaiser Bessel Function
         * @param alpha the smooth factor of Modified Kaiser Bessel Function
         */
        void addFT(const Complex value,
                   const double iCol,
                   const double iRow,
                   const double iSlc,
                   const double a,
                   const double alpha);

        /**
         * This function adds a certain value on an unregualr voxel in Fourier
         * space by a certain kernel.
         *
         * @param value the value to be added
         * @param iCol the index of the column of this unregular voxel in
         *             real space
         * @param iRow the index of the row of this unregular voxel in
         *             real space
         * @param iSlc the index of the slice of this unregular voxel in
         *             real space
         * @param a the radius of the blob
         * @param kernel a tabular function indicating the kernel
         */
        void addFT(const Complex value,
                   const double iCol,
                   const double iRow,
                   const double iSlc,
                   const double a,
                   const TabFunction& kernel);

        /**
         * This function clears up the allocated space and resets the size of
         * the volume to 0.
         */
        void clear();

        /**
         * This function returns a copy of itself.
         */
        Volume copyVolume() const;

        inline int iRL(const int i,
                       const int j,
                       const int k) const
        {
            return (k >= 0 ? k : k + _nSlc) * _nCol * _nRow
                 + (j >= 0 ? j : j + _nRow) * _nCol
                 + (i >= 0 ? i : i + _nCol);
        }

        inline int iFT(bool& conj,
                       int i,
                       int j,
                       int k) const
        {
            conj = conjHalf(i, j, k);

            return iFTHalf(i, j, k);
        }

        inline int iFTHalf(const int i,
                           const int j,
                           const int k) const
        {
            int nColFT = _nCol / 2 + 1;

            return (k >= 0 ? k : k + _nSlc) * nColFT * _nRow
                 + (j >= 0 ? j : j + _nRow) * nColFT 
                 + i;
        }

    private:

        /**
         * This function checks whether the given coordinates is in the boundary
         * of the volume or not in real space. If not, it will crash the process
         * and record a fatal log.
         *
         * @param iCol the index of the column of this voxel in real space
         * @param iRow the index of the row of this voxel in real space
         * @param iSlc the index of the slice of this voxel in real space
         */
        void coordinatesInBoundaryRL(const int iCol,
                                     const int iRow,
                                     const int iSlc) const;

        /**
         * This function checks whether the given coordinates is in the boundary
         * of the volume or not in Fourier space. If not, it will crash the
         * process and record a fatal log.
         *
         * @param iCol the index of the column of this voxel in Fourier space
         * @param iRow the index of the row of this voxel in Fourier space
         * @param iSlc the index of the slice of this voxel in Fourier space
         */
        void coordinatesInBoundaryFT(const int iCol,
                                     const int iRow,
                                     const int iSlc) const;

        double getRL(const double w[2][2][2],
                     const int x0[3]) const;

        Complex getFTHalf(const double w[2][2][2],
                          const int x0[3]) const;
};

#endif // VOLUME_H
