/** @file
 *  @author Mingxu Hu
 *  @author Shouqing Li
 *  @version 1.4.11.081025
 *  @copyright THUNDER Non-Commercial Software License Agreement
 *
 *  ChangeLog
 *  AUTHOR      | TIME       | VERSION       | DESCRIPTION
 *  ------      | ----       | -------       | -----------
 *  Mingxu Hu   | 2015/03/23 | 0.0.1.050323  | new file
 *  Shouqing Li | 2018/10/25 | 1.4.11.081025 | add test for directory 
 *  Mingxu Hu   | 2018/10/30 | 1.4.11.081030 | solve conflict during merging
 *  
 *  @brief thunder.cpp initiates the MPI, following the completion of reading and logging the json files. And according to the set parameters, thunder.cpp will carry out computation chosen from three models,namely 2D classification, 3D classification and 3D refinement. In the final, the results will be exported to the file wrote in json. 
 *
 */

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

#include <json/json.h>

#include "Config.h"
#include "Logging.h"
#include "Macro.h"
#include "Projector.h"
#include "Reconstructor.h"
#include "FFT.h"
#include "ImageFile.h"
#include "Particle.h"
#include "CTF.h"
#include "Optimiser.h"

using namespace std;

inline Json::Value JSONCPP_READ_ERROR_HANDLER(const Json::Value src, const std::string missingKey)
{
    if (src == Json::nullValue)
    {
        //REPORT_ERROR("INVALID JSON PARAMETER FILE KEY");
        CLOG(FATAL, "LOGGER_SYS") << "Json parameter file KEY \""
                                  << missingKey 
                                  << "\" is not exit. Please make sure of it. ";

        abort();
    }
    else
    {
        return src;
    }
}

/**
 *  This function is added by huabin
 *  This function is used to covert seconds to day:hour:min:sec format

void fmt_time(int timeInSeconds, char *outputBuffer)
{
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    int inputSeconds = timeInSeconds;

    day = timeInSeconds / (24 * 3600);
    timeInSeconds = timeInSeconds % (24 * 3600);
    hour = timeInSeconds/3600;
    timeInSeconds = timeInSeconds%3600;
    min = timeInSeconds/60;
    timeInSeconds = timeInSeconds%60;
    sec = timeInSeconds;
    snprintf(outputBuffer, 512, "%ds (%d days:%d hours:%d mins:%d seconds)\n", inputSeconds, day, hour, min, sec);
}
***/

template <size_t N>
static inline void copy_string(char (&array)[N], const std::string& source)
{
    if (source.size() + 1 >= N)
    {
        CLOG(FATAL, "LOGGER_SYS") << "String too large to fit in parameter. "
                                  << "Destination length is "
                                  << N
                                  << ", while source length is "
                                  << source.size() + 1;

        abort();
                                  
        return;
    }
    memcpy(array, source.c_str(), source.size() + 1);
}

void readPara(OptimiserPara& dst,
              const Json::Value src)
{
    dst.nThreadsPerProcess = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_N_THREADS_PER_PROCESS], KEY_N_THREADS_PER_PROCESS).asInt();

    if (JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_MODE], KEY_MODE).asString() == "2D")
    {
        dst.mode = MODE_2D;
    }
    else if (JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_MODE], KEY_MODE).asString() == "3D")
    {
        dst.mode = MODE_3D;
    }
    else
    {
        REPORT_ERROR("INEXISTENT MODE");

        abort();
    }

    dst.gSearch = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_G_SEARCH], KEY_G_SEARCH).asBool();
    dst.lSearch = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_L_SEARCH], KEY_L_SEARCH).asBool();
    dst.cSearch = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_C_SEARCH], KEY_C_SEARCH).asBool();

    dst.k = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_K], KEY_K).asInt();
    dst.size = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_SIZE], KEY_SIZE).asInt();
    dst.pixelSize = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_PIXEL_SIZE], KEY_PIXEL_SIZE).asFloat();
    dst.maskRadius = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_MASK_RADIUS], KEY_MASK_RADIUS).asFloat();
    dst.transS = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_TRANS_S], KEY_TRANS_S).asFloat();
    dst.initRes = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_INIT_RES], KEY_INIT_RES).asFloat();
    dst.globalSearchRes = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_GLOBAL_SEARCH_RES], KEY_GLOBAL_SEARCH_RES).asFloat();
    copy_string(dst.sym, JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_SYM], KEY_SYM).asString());
    copy_string(dst.initModel, JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_INIT_MODEL], KEY_INIT_MODEL).asString());
    copy_string(dst.db, JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_DB], KEY_DB).asString());
    copy_string(dst.parPrefix, JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_PAR_PREFIX], KEY_PAR_PREFIX).asString());
    copy_string(dst.dstPrefix, JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_DST_PREFIX], KEY_DST_PREFIX).asString());
    copy_string(dst.outputDirectory, JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_OUTPUT_DIRECTORY], KEY_OUTPUT_DIRECTORY).asString());
    copy_string(dst.outputFilePrefix, JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_OUTPUT_FILE_PREFIX], KEY_OUTPUT_DIRECTORY).asString());

    dst.coreFSC = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_CORE_FSC], KEY_CORE_FSC).asBool();
    dst.maskFSC = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_MASK_FSC], KEY_MASK_FSC).asBool();
    dst.parGra = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_PAR_GRA], KEY_PAR_GRA).asBool();
    dst.refAutoRecentre = JSONCPP_READ_ERROR_HANDLER(src["Basic"][KEY_REF_AUTO_RECENTRE], KEY_REF_AUTO_RECENTRE).asBool();

    dst.performMask = JSONCPP_READ_ERROR_HANDLER(src["Reference Mask"][KEY_PERFORM_MASK], KEY_PERFORM_MASK).asBool();
    dst.globalMask = JSONCPP_READ_ERROR_HANDLER(src["Reference Mask"][KEY_GLOBAL_MASK], KEY_GLOBAL_MASK).asBool();
    copy_string(dst.mask, JSONCPP_READ_ERROR_HANDLER(src["Reference Mask"][KEY_MASK], KEY_MASK).asString());

    dst.subtract = JSONCPP_READ_ERROR_HANDLER(src["Subtract"][KEY_SUBTRACT], KEY_SUBTRACT).asBool();
    copy_string(dst.regionCentre, JSONCPP_READ_ERROR_HANDLER(src["Subtract"][KEY_REGION_CENTRE], KEY_REGION_CENTRE).asString());

    dst.iterMax = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_ITER_MAX], KEY_ITER_MAX).asInt();
    dst.goldenStandard = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_GOLDEN_STANDARD], KEY_GOLDEN_STANDARD).asBool();
    dst.pf = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_PF], KEY_PF).asInt();
    dst.a = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_A], KEY_A).asFloat();
    dst.alpha = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_ALPHA], KEY_ALPHA).asFloat();

    if (dst.mode == MODE_2D)
    {
        dst.mS = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_M_S_2D], KEY_M_S_2D).asInt();

        dst.mLR = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_M_L_R_2D], KEY_M_L_R_2D).asInt();
    }
    else if (dst.mode == MODE_3D)
    {
        dst.mS = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_M_S_3D], KEY_M_S_3D).asInt();

        dst.mLR = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_M_L_R_3D], KEY_M_L_R_3D).asInt();
    }
    else
    {
        REPORT_ERROR("INEXISTENT MODE");

        abort();
    }

    dst.saveRefEachIter = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_SAVE_REF_EACH_ITER], KEY_SAVE_REF_EACH_ITER).asBool();
    dst.saveTHUEachIter = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_SAVE_THU_EACH_ITER], KEY_SAVE_THU_EACH_ITER).asBool();
    dst.mLT = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_M_L_T], KEY_M_L_T).asInt();
    dst.mLD = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_M_L_D], KEY_M_L_D).asInt();
    dst.mReco = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_M_RECO], KEY_M_RECO).asInt();
    dst.ignoreRes = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_IGNORE_RES], KEY_IGNORE_RES).asFloat();
    dst.sclCorRes = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_SCL_COR_RES], KEY_SCL_COR_RES).asFloat();
    dst.thresCutoffFSC = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_THRES_CUTOFF_FSC], KEY_THRES_CUTOFF_FSC).asFloat();
    dst.thresReportFSC = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_THRES_REPORT_FSC], KEY_THRES_REPORT_FSC).asFloat();
    dst.thresSclCorFSC = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_THRES_SCL_COR_FSC], KEY_THRES_SCL_COR_FSC).asFloat();
    dst.groupSig = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_GROUP_SIG], KEY_GROUP_SIG).asBool();
    dst.groupScl = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_GROUP_SCL], KEY_GROUP_SCL).asBool();
    dst.zeroMask = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_ZERO_MASK], KEY_ZERO_MASK).asBool();
    dst.ctfRefineS = JSONCPP_READ_ERROR_HANDLER(src["Advanced"][KEY_CTF_REFINE_S], KEY_CTF_REFINE_S).asFloat();

    dst.transSearchFactor = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_TRANS_SEARCH_FACTOR], KEY_TRANS_SEARCH_FACTOR).asFloat();
    dst.perturbFactorL = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_PERTURB_FACTOR_L], KEY_PERTURB_FACTOR_L).asFloat();
    dst.perturbFactorSGlobal = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_PERTURB_FACTOR_S_GLOBAL], KEY_PERTURB_FACTOR_S_GLOBAL).asFloat();
    dst.perturbFactorSLocal = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_PERTURB_FACTOR_S_LOCAL], KEY_PERTURB_FACTOR_S_LOCAL).asFloat();
    dst.perturbFactorSCTF = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_PERTURB_FACTOR_S_CTF], KEY_PERTURB_FACTOR_S_CTF).asFloat();
    dst.skipE = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_SKIP_E], KEY_SKIP_E).asBool();
    dst.skipM = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_SKIP_M], KEY_SKIP_M).asBool();
    dst.skipR = JSONCPP_READ_ERROR_HANDLER(src["Professional"][KEY_SKIP_R], KEY_SKIP_R).asBool();
}

void logPara(const Json::Value src)
{
    Json::Value::Members mem = src.getMemberNames();

    for (size_t i = 0; i < mem.size(); i++)
    {
        if (src[mem[i]].type() == Json::objectValue)
        {
            logPara(src[mem[i]]);
        }
        else if (src[mem[i]].type() == Json::arrayValue)
        {
            for (int j = 0; j < (int)src[mem[i]].size(); j++)
                logPara(src[mem[i]][j]);
        }
        else if (src[mem[i]].type() == Json::stringValue)
        {
            CLOG(INFO, "LOGGER_SYS") << "[JSON PARAMTER] " << mem[i] << " : " << src[mem[i]].asString();
        }
        else if (src[mem[i]].type() == Json::realValue)
        {
            CLOG(INFO, "LOGGER_SYS") << "[JSON PARAMTER] " << mem[i] << " : " << src[mem[i]].asFloat();
        }
        else if (src[mem[i]].type() == Json::uintValue)
        {
            CLOG(INFO, "LOGGER_SYS") << "[JSON PARAMTER] " << mem[i] << " : " << src[mem[i]].asUInt();
        }
        else
        {
            CLOG(INFO, "LOGGER_SYS") << "[JSON PARAMTER] " << mem[i] << " : " << src[mem[i]].asInt();
        }
    }
}

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        cout << "Welcome to THUNDER "
             << THUNDER_VERSION_MAJOR
             << "."
             << THUNDER_VERSION_MINOR
             << "."
             << THUNDER_VERSION_ADDIT
             << "!"
             << endl;

        return 0;
    }
    else if (argc != 2)
    {
        cout << "Wrong Number of Parameters Input!"
             << endl;

        return -1;
    }

    Json::Reader jsonReader;
    Json::Value jsonRoot;
    OptimiserPara thunderPara;
    ifstream jsonFile(argv[1], ios::binary);
    if (!jsonFile.is_open())
    {
        fprintf(stderr, "Fail to Open JSON [%s] Parameter File\n", argv[1]);
        abort();
    }

    if(jsonReader.parse(jsonFile, jsonRoot))
    {
        readPara(thunderPara, jsonRoot);
    }
    
    fprintf(stderr, "output directory: %s, output file prefix: %s\n", thunderPara.outputDirectory, thunderPara.outputFilePrefix);
   
    return 0; 

    



    loggerInit(argc, argv);

    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        CLOG(INFO, "LOGGER_SYS") << "THUNDER is Initiallised With "
                                 << size
                                 << " Processes";

        if (size <= 2)
        {
            CLOG(FATAL, "LOGGER_SYS") << "THUNDER REQUIRES AT LEAST 3 PROCESSES IN MPI";

            abort();
        }
        else if (size == 4)
        {
            CLOG(WARNING, "LOGGER_SYS") << "2 PROCESSES IN HEMISPHERE A, 1 PROCESS IN HEMISPHERE B, SEVERE INBALANCE";
        }
    }

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "THUNDER v"
                                            << THUNDER_VERSION_MAJOR
                                            << "." 
                                            << THUNDER_VERSION_MINOR
                                            << "."
                                            << THUNDER_VERSION_ADDIT;

#ifdef VERBOSE_LEVEL_1
    CLOG(INFO, "LOGGER_SYS") << "Initialising Processes";
#endif

    
#ifdef VERBOSE_LEVEL_1
    CLOG(INFO, "LOGGER_SYS") << "Process " << rank << " Initialised";
#endif

    Json::Reader reader;
    Json::Value root;

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Initialising Threads Setting in FFTW";

    ifstream in(argv[1], ios::binary);

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Openning Parameter File";

    if (!in.is_open())
    {
        if (rank == 0) CLOG(FATAL, "LOGGER_SYS") << "Fail to Open Parameter File";

        abort();
    }

    OptimiserPara para;

    if (reader.parse(in, root))
    {
        readPara(para, root);
    }
    else
    {
        if (rank == 0) CLOG(FATAL, "LOGGER_SYS") << "Fail to Parse Parameter File";

        abort();
    }

    if (rank == 0)
    {
        CLOG(INFO, "LOGGER_SYS") << "Logging JSON Parameters";

        logPara(root);

        DIR * dir;
        string dstSet = para.dstPrefix;
        
        if((dir = opendir(dstSet.c_str())) == NULL)
        {
            
            CLOG(WARNING, "LOGGER_SYS") << "DIRECTORY TO SAVE THE RESULT DOES NOT EXIST, MAKING THIS DIRECTORY.";

            size_t index;
            string dstSetMake;
            int post = 0;
            int flag;

            while(1)
            {
                index = dstSet.find_first_of('/',post);

                if(index == string::npos)
                {
                    break;
                }

                dstSetMake = dstSet.substr(0, index + 1);
                    
                if((dir = opendir(dstSetMake.c_str())) == NULL)
                {
                    const char* command = dstSetMake.c_str();
                    CLOG(INFO, "LOGGER_SYS") << "Building " << dstSetMake;
                    flag = mkdir(command, 00755);
                        
                    if(flag == -1)
                    {
                        REPORT_ERROR(strerror(errno));
                        abort();
                    }
                }

                post = index + 1; 
            }                
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Setting Maximum Number of Threads Per Process";

    omp_set_num_threads(para.nThreadsPerProcess);

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Maximum Number of Threads in a Process is " << omp_get_max_threads();

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Initialising Threads Setting in FFTW";

    if (TSFFTW_init_threads() == 0)
    {
        REPORT_ERROR("ERROR IN INITIALISING FFTW THREADS");

        abort();
    }

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Setting Time Limit for Creating FFTW Plan";

    TSFFTW_set_timelimit(60);

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Setting Parameters";
    
    Optimiser opt;

    opt.setPara(para);

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Setting MPI Environment";

    opt.setMPIEnv();

    if (rank == 0) CLOG(INFO, "LOGGER_SYS") << "Running";

    opt.run();

    MPI_Finalize();

    TSFFTW_cleanup_threads();

    return 0;
}
