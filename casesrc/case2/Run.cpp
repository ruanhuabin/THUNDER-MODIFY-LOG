/*******************************************************************************
 * Author: Mingxu Hu
 * Dependecy:
 * Test:
 * Execution:
 * Description:
 * ****************************************************************************/

#include <iostream>

#include "Projector.h"
#include "Reconstructor.h"
#include "FFT.h"
#include "ImageFile.h"
#include "Particle.h"
#include "CTF.h"
#include "Experiment.h"
#include "MLOptimiser.h"

#define PF 2

#define N 380
#define TRANS_S 10

//#define PIXEL_SIZE 1.32
#define PIXEL_SIZE 1.30654

#define MG 1000
#define ML 100

using namespace std;

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
    loggerInit(argc, argv);

    MPI_Init(&argc, &argv);

    cout << "Initialising Parameters" << endl;
    MLOptimiserPara para;
    para.iterMax = atoi(argv[1]);
    para.k = 1;
    para.size = N;
    para.pf = PF;
    //para.a = 0.95;
    para.a = 1.9;
    para.alpha = 10;
    para.pixelSize = PIXEL_SIZE;
    para.mG = MG;
    para.mL = ML;
    para.transS = TRANS_S;
    para.initRes = 20;
    //para.ignoreRes = 200;
    para.ignoreRes = 200;
    para.sclCorRes = 80;
    //para.globalSearchRes = 15;
    //para.globalSearchRes = 10;
    //para.globalSearchRes = 40;
    para.globalSearchRes = 12;
    sprintf(para.sym, "C15");
    // sprintf(para.initModel, "padCylinder.mrc");
    sprintf(para.initModel, "padRef.mrc");
    sprintf(para.db, "C15.db");
    para.groupSig = true;
    para.groupScl = false;

    cout << "Setting Parameters" << endl;
    MLOptimiser opt;
    opt.setPara(para);

    cout << "MPISetting" << endl;
    opt.setMPIEnv();

    cout << "Run" << endl;
    opt.run();

    MPI_Finalize();
}
