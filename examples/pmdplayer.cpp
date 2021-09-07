/*
    PMDPlayer by LeadRDRK
    Simple command line PMD player and renderer.

    This file is in the public domain.
    You may use it however you want.
*/

#include <iostream>
#include <unordered_map>
#include <csignal>
#include <thread>
#include "../pmdwin/pmdwin.h"

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#endif

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#define PLAYBACK_RATE 44100
#define PCM_BUFFER_SIZE 4096

static int gotSignal = -1;
void signalHandler(int signal)
{
    gotSignal = signal;
}

#define TIME_BUFFER_SIZE 16
void getTimeStr(float seconds, char* timeStr)
{
    int minutes = seconds/60;
    seconds -= minutes*60;
    snprintf(timeStr, TIME_BUFFER_SIZE, "%02d:%02d", minutes, (int)seconds);
}

void fillProgressBar(char c, int count, char* buf)
{
    for (int i = 0; i < count; ++i)
    {
        buf[i] = c;
    }
    buf[count] = '\0';
}

#define PROG_BAR_SIZE 30
#define PROG_FULL_CHAR '#'
#define PROG_EMP_CHAR ' '
void printTimePos(float current, float duration)
{
    char curStr[TIME_BUFFER_SIZE];
    char durStr[TIME_BUFFER_SIZE];

    getTimeStr(current, curStr);
    getTimeStr(duration, durStr);

    int progFullCount = (current/duration) * PROG_BAR_SIZE;
    int progEmpCount = PROG_BAR_SIZE - progFullCount;
    char progFull[progFullCount + 1]; // including null terminator
    char progEmp[progEmpCount + 1];

    fillProgressBar(PROG_FULL_CHAR, progFullCount, progFull);
    fillProgressBar(PROG_EMP_CHAR, progEmpCount, progEmp);

    std::cout << "\r" << curStr << "/" << durStr << " [" << progFull << progEmp << "]" << std::flush;
}

struct PlaybackData
{
    PMDWIN* pmdwin;
    float current;
    float duration;
    bool loop;

    float prevUpdate = -1;
    bool finished = false;
};

void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    PlaybackData* data = reinterpret_cast<PlaybackData*>(pDevice->pUserData);
    PMDWIN* pmdwin = data->pmdwin;
    pmdwin->getpcmdata(reinterpret_cast<int16_t*>(pOutput), frameCount);

    if (data->current >= data->duration)
    {
        if (data->loop)
        {
            data->current = 0;
            data->prevUpdate = -1;
        }
        else
        {
            data->finished = true;
            ma_device_uninit(pDevice);
            return;
        }
    }

    data->current += (float)frameCount / PLAYBACK_RATE;
    if (data->current - data->prevUpdate >= 1)
    {
        printTimePos(data->current, data->duration);
        data->prevUpdate = data->current;
    }
}

bool initAudioDevice(ma_device* device, PlaybackData* data)
{
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_s16;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate        = PLAYBACK_RATE;
    deviceConfig.dataCallback      = dataCallback;
    deviceConfig.pUserData         = data;

    // 100ms buffer
    deviceConfig.periodSizeInMilliseconds = 100;

    if (ma_device_init(NULL, &deviceConfig, device) != MA_SUCCESS)
    {
        std::cout << "Failed to initialize audio device." << std::endl;
        return false;
    }

    if (ma_device_start(device) != MA_SUCCESS)
    {
        std::cout << "Failed to start audio device." << std::endl;
        return false;
    }

    return true;
}

inline void printProgramName()
{
    std::cout << "PMDPlayer v1.0 by LeadRDRK\n\n";
}

inline void printHelp()
{
    std::cout <<
        "Usage: pmdplayer [options] <file>\n"
        "Options:\n"
        "    -h, --help: display this help message\n"
        "    -o, --output <file>: specify output file\n"
        "    -w, --wav: create .wav file output\n"
        "    -l, --loop: turn on looping (playback only)\n" <<
    std::endl;
}

enum ArgType
{
    ARG_HELP,
    ARG_OUTPUT,
    ARG_WAVOUT,
    ARG_LOOP
};

static std::unordered_map<std::string, ArgType> argMap = {
    {"-h", ARG_HELP},   {"--help", ARG_HELP},
    {"-o", ARG_OUTPUT}, {"--output", ARG_OUTPUT},
    {"-w", ARG_WAVOUT}, {"--wav", ARG_WAVOUT},
    {"-l", ARG_LOOP},   {"--loop", ARG_LOOP},
};

enum OutputType
{
    OUTPUT_STREAM,
    OUTPUT_WAV
};

struct ProgramOptions
{
    char* input = nullptr;
    OutputType outType = OUTPUT_STREAM;
    char* output = nullptr;
    bool loop = false;
};

int main(int argc, char** argv)
{
    // skip over argv[0]
    --argc;
    ++argv;

    printProgramName();

    if (argc < 1)
    {
        std::cout << "No arguments provided.\n";
        printHelp();
        return 1;
    }

    ProgramOptions options;
    for (int i = 0; i < argc; ++i)
    {
        auto it = argMap.find(argv[i]);
        if (it == argMap.end())
        {
            if (i == argc - 1)
            {
                options.input = argv[i];
            }
            else
            {
                std::cout << "Invalid argument: " << argv[i] << "\n";
                printHelp();
                return 1;
            }
            break;
        }

        switch (it->second)
        {
        case ARG_HELP:
            printHelp();
            return 0;

        case ARG_OUTPUT:
            options.output = argv[++i];
            break;

        case ARG_WAVOUT:
            options.outType = OUTPUT_WAV;
            break;

        case ARG_LOOP:
            options.loop = true;
            break;

        }
    }

    if (!options.input)
    {
        std::cout << "Error: No input provided." << "\n";
        printHelp();
        return 1;
    }

    if (options.output && options.outType == OUTPUT_STREAM)
    {
        // assume wav output
        options.outType = OUTPUT_WAV;
    }

    //  Main  //
    char* workDir;
    if ((workDir = getcwd(NULL, 0)) == NULL)
    {
        std::cout << "Failed to get current working directory." << std::endl;
        return 1;
    }

    PMDWIN pmdwin;
    // rhythm samples will be loaded from workDir
    pmdwin.init(workDir);
    free(workDir);
    int result = pmdwin.music_load(options.input);

    switch (result)
    {
    // ignore success and warnings
    case PMDWIN_OK:
    case WARNING_PPC_ALREADY_LOAD:
    case WARNING_P86_ALREADY_LOAD:
    case WARNING_PPS_ALREADY_LOAD:
    case WARNING_PPZ1_ALREADY_LOAD:
    case WARNING_PPZ2_ALREADY_LOAD:
        break;

    // handle errors
    default:
        std::cout << "Failed to load " << options.input << " (error " << result << ")" << std::endl;
        return 1;

    }

    int lengthTmp, loop;
    pmdwin.getlength(options.input, &lengthTmp, &loop);
    // convert to seconds
    float length = lengthTmp/1000.f;

    // Start
    pmdwin.music_start();
    if (options.outType == OUTPUT_STREAM)
    {
        std::cout << "Playing: " << options.input << std::endl;

        ma_device device;
        PlaybackData data = {&pmdwin, 0, length, options.loop};
        if (!initAudioDevice(&device, &data)) return 1;

        // stall
        signal(SIGINT, signalHandler);
        for (;;)
        {
            if (gotSignal == SIGINT)
            {
                std::cout << "\nGot SIGINT, exiting." << std::endl;
                break;
            }
            if (data.finished)
            {
                std::cout << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::duration<float>(0.1));
        }

        if (!data.finished)
            ma_device_uninit(&device);
    }
    else
    {
        if (!options.output)
        {
            std::cout << "Error: Audio file output enabled but no file path was specified.\n";
            printHelp();
            return 1;
        }

        ma_resource_format format;
        switch (options.outType)
        {
        default:
        case OUTPUT_WAV:
            format = ma_resource_format_wav;
            break;

        }

        ma_encoder_config config;
        config = ma_encoder_config_init(format, ma_format_s16, 2, PLAYBACK_RATE);

        ma_encoder encoder;
        if (ma_encoder_init_file(options.output, &config, &encoder) != MA_SUCCESS)
        {
            std::cout << "Failed to initialize the encoder." << std::endl;
            return 1;
        }

        int samplesLeft = length * PLAYBACK_RATE;
        int16_t buffer[PCM_BUFFER_SIZE];

        std::cout << "Writing file..." << std::endl;
        while (samplesLeft > 0)
        {
            int size = std::min(PCM_BUFFER_SIZE, samplesLeft);
            pmdwin.getpcmdata(buffer, size);
            ma_encoder_write_pcm_frames(&encoder, buffer, size);

            samplesLeft -= PCM_BUFFER_SIZE;
            printTimePos(length - (float)samplesLeft/PLAYBACK_RATE, length);
        }
        std::cout << "\nDone." << std::endl;

        ma_encoder_uninit(&encoder);
    }
    pmdwin.music_stop();
}