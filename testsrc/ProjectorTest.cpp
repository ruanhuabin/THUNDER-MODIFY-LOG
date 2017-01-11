/*******************************************************************************
 * Author: Mingxu Hu
 * Dependecy:
 * Test:
 * Execution:
 * Description:
 * ****************************************************************************/

#include <iostream>

#include <gsl/gsl_randist.h>

#include "Projector.h"
#include "ImageFile.h"
#include "FFT.h"
#include "Random.h"
#include "Transformation.h"

#define N 128
#define M 10
#define PF 2

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
    loggerInit(argc, argv);

    std::cout << "Define a head." << std::endl;

    Volume head(N, N, N, RL_SPACE);

    VOLUME_FOR_EACH_PIXEL_RL(head)
    {
        double ii = i * 0.8;
        double jj = j * 0.8;
        double kk = k * 0.8;
        if ((NORM_3(ii, jj, kk) < N / 8) ||
            (NORM_3(ii - N / 8, jj, kk - N / 8) < N / 16) ||
            (NORM_3(ii - N / 8, jj - N / 8, kk - N / 8) < N / 16) ||
            (NORM_3(ii + N / 8, jj, kk - N / 8) < N / 16) ||
            (NORM_3(ii + N / 8, jj + N / 8, kk - N / 8) < N / 16) ||
            ((NORM(ii, jj) < N / 16) &&
             (kk + N / 16 < 0) &&
             (kk + 3 * N / 16 > 0)))
            head.setRL(1, i, j, k);
        else
            head.setRL(0, i, j, k);
    }

    gsl_rng* engine = get_random_engine();

    Volume noise(N, N, N, RL_SPACE);
    FOR_EACH_PIXEL_RL(noise)
        noise(i) = gsl_ran_gaussian(engine, 1);

    ADD_RL(head, noise);

    mat33 centre;
    centre << -1, 0, 0,
              0, -1, 0,
              0, 0, -1;
    /***
    mat33 centre({{-1, 0, 0},
                  {0, -1, 0},
                  {0, 0, -1}});
                  ***/
    Volume centreHead(N, N, N, RL_SPACE);
    VOL_TRANSFORM_MAT_RL(centreHead,
                         head,
                         centre,
                         centreHead.nColRL() / 2 -1,
                         LINEAR_INTERP);

    ImageFile imf;
    imf.readMetaData(head);
    imf.writeVolume("head.mrc", head);
    imf.readMetaData(centreHead); 
    imf.writeVolume("centreHead.mrc", centreHead);

    Volume padHead;
    VOL_PAD_RL(padHead, head, PF);
    Volume padCentreHead;
    VOL_PAD_RL(padCentreHead, centreHead, PF);
    /***
    imf.readMetaData(padHead);
    imf.writeVolume("padHead.mrc", padHead);
    ***/

    FFT fft;
    fft.fw(padHead);
    fft.fw(padCentreHead);

    Projector projector;
    projector.setMaxRadius(N / 4);
    projector.setPf(PF);

    Image image(N, N, RL_SPACE);

    mat33 rot;

    projector.setProjectee(padHead.copyVolume());

    rotate3D(rot, 0.3, 0.3, 0.3);

    R2R_FT(image,
           image,
           projector.project(image, rot));
    image.saveRLToBMP("Positive.bmp");

    rotate3D(rot, 0.3 + M_PI, 0.3, 0.3);

    R2R_FT(image,
           image,
           projector.project(image, rot));

    image.saveRLToBMP("PositiveCounterPart.bmp");

    R2R_FT(image,
           image,
           projector.project(image, rot));

    projector.setProjectee(padCentreHead.copyVolume());

    rotate3D(rot, 0.3, 0.3, 0.3);
    mat33 rot2;
    rotate3D(rot2, 0, 0, M_PI);
    std::cout << rot2 << std::endl;

    R2R_FT(image,
           image,
           projector.project(image, rot2 * rot));
    image.saveRLToBMP("Negative.bmp");

    /***
    rot = rot * mat33({{-1, 0, 0},
                       {0, -1, 0},
                       {0, 0, -1}});
                       ***/
    //rot *= rot2;

    /***
    R2R_FT(image,
           image,
           projector.project(image, 2 * M_PI - 0.3, M_PI - 0.3, 2 * M_PI - 0.3));
           ***/
    /***
    R2R_FT(image,
           image,
           projector.project(image, rot));
    image.saveRLToBMP("Negative.bmp");
    ***/

    Image img(N, N, FT_SPACE);

    CLOG(INFO, "LOGGER_SYS") << "START";

    char name[FILE_NAME_LENGTH];

    try
    {
    for (int k = 0; k < M; k++)
        for (int j = 0; j < M; j++)
            for (int i = 0; i < M; i++)
            {
                SET_0_FT(img);
                printf("%02d %02d %02d\n", i, j, k);
                /***
                projector.project(img,
                                  2 * M_PI * i / M,
                                  M_PI * j / M,
                                  2 * M_PI * k / M,
                                  10,
                                  10);
                ***/
                projector.project(img,
                                  2 * M_PI * i / M,
                                  M_PI * j / M,
                                  2 * M_PI * k / M);

                sprintf(name, "%02d%02d%02dFT.bmp", i, j, k);
                img.saveFTToBMP(name, 0.1);

                fft.bw(img);

                sprintf(name, "%02d%02d%02dRL.bmp", i, j, k);
                img.saveRLToBMP(name);

                sprintf(name, "%02d%02d%02dRL.mrc", i, j, k);
                imf.readMetaData(img);
                imf.writeImage(name, img);

                fft.fw(img);
            }
    }
    catch (Error& err)
    {
        std::cout << err << std::endl;
    }

    CLOG(INFO, "LOGGER_SYS") << "END";

    return 0;
}
