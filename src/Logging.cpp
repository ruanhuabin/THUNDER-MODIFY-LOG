#include "Logging.h"
#include "Utils.h"

static el::Configurations conf;

void loggerInit(const char *logFileFullName)
{

    std::cout << "Log File will be Put: " << logFileFullName << std::endl;
    conf.setToDefault();
    conf.set(el::Level::All, el::ConfigurationType::Filename, logFileFullName);
    conf.set(el::Level::Info, el::ConfigurationType::ToFile, "true");
    conf.set(el::Level::Info, el::ConfigurationType::ToStandardOutput, "false");
    el::Loggers::setDefaultConfigurations(conf, true);

    const char* loggerNames[] = {"LOGGER_SYS","LOGGER_INIT","LOGGER_ROUND","LOGGER_COMPARE",
                                 "LOGGER_RECO","LOGGER_MPI","LOGGER_FFT", "LOGGER_GPU", "LOGGER_MEM"};
    for (size_t i = 0; i < sizeof(loggerNames) / sizeof(*loggerNames); ++i)
    {
        el::Loggers::getLogger(loggerNames[i]); // Force creation of loggers
    }

}

void loggerInit(int argc, const char* const * argv)
{
    using std::string;

    string loggerFileName;

    char buf[FILE_NAME_LENGTH];
    GETCWD_ERROR_HANDLER(getcwd(buf, sizeof(buf)));

    loggerFileName = buf;
    string appname(argv[0]);

    if (appname.rfind('/') == std::string::npos)
        loggerFileName += '/' + appname + ".log";
    else
        loggerFileName += appname.substr(appname.rfind('/')) + ".log";

    std::cout << "Log File will be Put: " << loggerFileName << std::endl;

    conf.setToDefault();
    conf.set(el::Level::All,
             el::ConfigurationType::Filename,
             loggerFileName);
    conf.set(el::Level::Info,
             el::ConfigurationType::ToFile,
             "true");
    conf.set(el::Level::Info,
             el::ConfigurationType::ToStandardOutput,
             "false");
    el::Loggers::setDefaultConfigurations(conf, true);

    const char* loggerNames[] = {"LOGGER_SYS","LOGGER_INIT","LOGGER_ROUND","LOGGER_COMPARE",
                                 "LOGGER_RECO","LOGGER_MPI","LOGGER_FFT", "LOGGER_GPU", "LOGGER_MEM"};
    for (size_t i = 0; i < sizeof(loggerNames) / sizeof(*loggerNames); ++i) {
        el::Loggers::getLogger(loggerNames[i]); // Force creation of loggers
    }
}

long memoryCheckParseLine(char* line)
{
    // This assumes that a digit will be found and the line ends in " Kb".
    long i = strlen(line);

    const char* p = line;

    while (*p <'0' || *p > '9') p++;

    line[i - 3] = '\0';

    i = atoi(p);

    return i;
}

long memoryCheckVM()
{
    FILE* file = fopen("/proc/self/status", "r");

    if (file == NULL)
    {
        REPORT_ERROR("FAIL TO OPEN /proc/self/status");

        abort();
    }

    long result = -1;

    char line[128];

    while (fgets(line, 128, file) != NULL)
    {
        if (strncmp(line, "VmSize:", 7) == 0)
        {
            result = memoryCheckParseLine(line);

            break;
        }
    }

    fclose(file);

    return result;
}

long memoryCheckRM()
{
    FILE* file = fopen("/proc/self/status", "r");

    if (file == NULL)
    {
        REPORT_ERROR("FAIL TO OPEN /proc/self/status");

        abort();
    }

    long result = -1;

    char line[128];

    while (fgets(line, 128, file) != NULL)
    {
        if (strncmp(line, "VmRSS:", 6) == 0)
        {
            result = memoryCheckParseLine(line);

            break;
        }
    }

    fclose(file);

    return result;
}
