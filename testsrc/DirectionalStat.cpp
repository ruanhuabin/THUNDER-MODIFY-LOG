//This header file is add by huabin
#include "huabin.h"
/*******************************************************************************
 * Author: Mingxu Hu
 * Dependecy:
 * Test:
 * Execution:
 * Description:
 * ****************************************************************************/

#include <iostream>

#include <gsl/gsl_sort.h>

#include "DirectionalStat.h"

#define MU 0

#define KAPPA_0 0
#define KAPPA_1 0.5
#define KAPPA_2 1
#define KAPPA_3 2
#define KAPPA_4 4
#define KAPPA_5 8
#define KAPPA_6 16
#define KAPPA_7 100000

#define N 1000

//#define TEST_SAMPLE_ACG

//#define TEST_INFER_ACG

//#define TEST_PDF_VMS

#define TEST_SAMPLE_VMS

#define TEST_INFER_VMS

//#define TEST_EIGEN_SORT

INITIALIZE_EASYLOGGINGPP

using namespace std;

int main(int argc, const char* argv[])
{
#ifdef TEST_SAMPLE_ACG
    mat44 mat;

    /***
    mat << 1, 0, 0, 0,
           0, 5, 0, 0,
           0, 0, 5, 0,
           0, 0, 0, 5;
           ***/

    /***
    mat << 1, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1;
    ***/

    /***
    mat << 1000, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1;
    ***/

    mat << 2, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1;

    mat4 acg(N, 4);

    sampleACG(acg, mat, N);

    // cout << acg << endl;
    
    /***
    for (int i = 0; i < N; i++)
        printf("%15.6lf %15.6lf %15.6lf %15.6lf\n",
               acg(i, 0),
               acg(i, 1),
               acg(i, 2),
               acg(i, 3));
    ***/

    mat44 A;

    inferACG(A, acg);

    cout << A << endl;

    /***
    RFLOAT k0, k1;

    inferACG(k0, k1, acg);

    printf("k0 = %15.6lf, k1 = %15.6lf\n", k0, k1);

    for (int i = 0; i < N; i++)
        if (acg(i, 0) < 0)
            acg.row(i) *= -1;

    inferACG(k0, k1, acg);

    printf("k0 = %15.6lf, k1 = %15.6lf\n", k0, k1);

    ***/
#endif

#ifdef TEST_INFER_ACG
    mat4 acg = mat4::Zero(N, 4);

    acg.col(0) = vec::Ones(N);
    //acg.col(1) = vec::Ones(N);

    /***
    for (int i = 0; i < N; i++)
        printf("%15.6lf %15.6lf %15.6lf %15.6lf\n",
               acg(i, 0),
               acg(i, 1),
               acg(i, 2),
               acg(i, 3));
               ***/

    /***
    RFLOAT k0, k1;

    inferACG(k0, k1, acg);

    printf("k0 = %15.6lf, k1 = %15.6lf\n", k0, k1);
    ***/

    /***
    RFLOAT k1, k2, k3;

    inferACG(k1, k2, k3, acg);

    printf("k1 = %15.6lf, k2 = %15.6lf, k3 = %15.6lf\n", k1, k2, k3);
    ***/

    vec4 mean;

    inferACG(mean, acg);

    cout << "mean = \n" << mean << endl;

#endif

#ifdef TEST_PDF_VMS
    for (RFLOAT theta = -M_PI; theta < M_PI; theta += 0.01)
        printf("%6f   %6f   %6f   %6f   %6f   %6f\n",
               theta,
               pdfVMS(vec2(cos(theta), sin(theta)), vec2(cos(MU), sin(MU)), KAPPA_0),
               pdfVMS(vec2(cos(theta), sin(theta)), vec2(cos(MU), sin(MU)), KAPPA_1),
               pdfVMS(vec2(cos(theta), sin(theta)), vec2(cos(MU), sin(MU)), KAPPA_2),
               pdfVMS(vec2(cos(theta), sin(theta)), vec2(cos(MU), sin(MU)), KAPPA_3),
               pdfVMS(vec2(cos(theta), sin(theta)), vec2(cos(MU), sin(MU)), KAPPA_4));
#endif

#ifdef TEST_SAMPLE_VMS

    mat2 VMS = mat2::Zero(N, 2);

    sampleVMS(VMS, vec2(atof(argv[1]), atof(argv[2])), atof(argv[3]), N);

    for (int i = 0; i < N; i++)
        printf("%15.6lf %15.6lf\n", VMS(i, 0), VMS(i, 1));
#endif

#ifdef TEST_INFER_VMS
    vec2 mu;
    RFLOAT k;

    inferVMS(mu, k, VMS);
    printf("mu = (%lf, %lf), k = %lf\n", mu(0), mu(1), k);
#endif

#ifdef TEST_EIGEN_SORT

    vec4 b = vec4(4, 3, 1, 2);

    TSGSL_sort(b.data(), 1, 4);

    cout << b << endl;
#endif
}
