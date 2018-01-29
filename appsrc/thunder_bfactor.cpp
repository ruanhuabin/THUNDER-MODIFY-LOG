//This header file is add by huabin
#include "huabin.h"
/*******************************************************************************
 * Author: Mingxu Hu
 * Dependecy:
 * Test:
 * Execution:
 * Description:
 * ****************************************************************************/

#include <fstream>

#include "ImageFile.h"
#include "Volume.h"
#include "Filter.h"
#include "FFT.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
    loggerInit(argc, argv);

    TSFFTW_init_threads();

    ImageFile imf(argv[2], "rb");
    imf.readMetaData();

    Volume ref;
    imf.readVolume(ref);

    FFT fft;
    fft.fwMT(ref);

    bFactorFilter(ref,
                  ref,
                  atof(argv[3]));

    fft.bwMT(ref);

    imf.readMetaData(ref);

    imf.writeVolume(argv[1], ref, atof(argv[4]));

    TSFFTW_cleanup_threads();

    return 0;
}
