////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


//#undef NS_MINIMUM_LOG_LEVEL
//#define NS_MINIMUM_LOG_LEVEL 0
//#define MAL_LOG_LEVEL MAL_LOG_LEVEL_VERBOSE


#include "Audio.h"


#ifdef NS_PLATFORM_WINDOWS_DESKTOP
    #define HAVE_AUDIO
    #define MAL_COINIT_VALUE COINIT_APARTMENTTHREADED
    #include "Unknwn.h"
#endif

#ifdef NS_PLATFORM_LINUX
    #define HAVE_AUDIO
#endif

#ifdef NS_PLATFORM_IPHONE
    #define HAVE_AUDIO
#endif

#ifdef NS_PLATFORM_ANDROID
    #define HAVE_AUDIO
#endif

#ifdef NS_PLATFORM_OSX
    #define HAVE_AUDIO
#endif

#ifdef HAVE_AUDIO

#include <NsCore/Log.h>
#include <NsCore/Ptr.h>
#include <NsCore/Error.h>
#include <NsCore/Atomic.h>
#include <NsCore/Stream.h>
#include <NsCore/File.h>
#include <NsGui/IntegrationAPI.h>

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4127)
NS_GCC_CLANG_WARNING_DISABLE("-Wtype-limits")
NS_GCC_CLANG_WARNING_DISABLE("-Wsign-compare")
NS_GCC_CLANG_WARNING_DISABLE("-Wconversion")

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO
#include "dr_wav.h"

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO
#include "dr_mp3.h"

#define MINI_AL_IMPLEMENTATION
#define MAL_NO_STDIO
#define MAL_NO_AVX2
#define MAL_NO_AVX512
#include "mini_al.h"

NS_WARNING_POP

using namespace Noesis;
using namespace NoesisApp;

#define FORMAT mal_format_f32
#define CHANNELS 2
#define SAMPLE_RATE 44100
#define MAX_VOICES 16

struct Voice
{
    AtomicInteger active;
    float volume;
    mal_decoder decoder;
    Ptr<Stream> stream;
};

static Voice voices[MAX_VOICES];
static mal_device device;
static int32_t initCount;

////////////////////////////////////////////////////////////////////////////////////////////////////
static mal_uint32 OnSendAudioDataToDevice(mal_device*, mal_uint32 frameCount, void* out_)
{
    memset(out_, 0, frameCount * CHANNELS * sizeof(float));

    float* out = (float*)out_;
    float tmp[1024 * CHANNELS];

    uint32_t offset = 0;
    uint32_t pendingFrames = frameCount;

    while (pendingFrames)
    {
        uint32_t frameCount_ = eastl::min_alt(pendingFrames, (uint32_t)NS_COUNTOF(tmp) / CHANNELS);

        for (uint32_t i = 0; i < MAX_VOICES; i++)
        {
            Voice* voice = voices + i;

            if (voice->active)
            {
                const float volume = voice->volume;
                mal_uint64 frames = mal_decoder_read(&voice->decoder, frameCount_, tmp);
                for (uint32_t j = 0; j < frames * CHANNELS; j++)
                {
                    out[j + offset] += tmp[j] * volume;
                }

                if (frames < frameCount_)
                {
                    NS_LOG_TRACE("[%d] <- Finished", i);
                    mal_result r = mal_decoder_uninit(&voice->decoder);
                    NS_ASSERT(r == MAL_SUCCESS);
                    voice->stream.Reset();
                    voice->active = false;
                }
            }
        }

        offset += frameCount_ * CHANNELS;
        pendingFrames -= frameCount_;
    }

    return frameCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnLog(mal_context*, mal_device*, const char *message)
{
    NS_LOG_ERROR("%s", message);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Audio::Init()
{
    if (++initCount == 1)
    {
        mal_context_config contextConfig = mal_context_config_init(&OnLog);
        mal_device_config deviceConfig = mal_device_config_init_playback(FORMAT, CHANNELS,
            SAMPLE_RATE, &OnSendAudioDataToDevice);
        mal_result r = mal_device_init_ex(nullptr, 0, &contextConfig, mal_device_type_playback,
            nullptr, &deviceConfig, nullptr, &device);
        NS_ASSERT(r == MAL_SUCCESS);

        NS_LOG_DEBUG("Creating '%s' audio device: %s", mal_get_backend_name(
            device.pContext->backend), device.name);
        NS_LOG_TRACE(" Format: %s", mal_get_format_name(device.internalFormat));
        NS_LOG_TRACE(" Sample rate: %dHz x %d", device.internalSampleRate, device.internalChannels);
        NS_LOG_TRACE(" Buffer size: %d frames", device.bufferSizeInFrames);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static size_t OnRead(mal_decoder* decoder, void* buffer, size_t size)
{
    Stream* stream = ((Voice*)decoder->pUserData)->stream;
    return (size_t)stream->Read(buffer, (uint32_t)size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static mal_bool32 OnSeek(mal_decoder* decoder, int offset, mal_seek_origin origin)
{
    Stream* stream = ((Voice*)decoder->pUserData)->stream;

    switch (origin)
    {
        case mal_seek_origin_start:
        {
            stream->SetPosition(offset);
            break;
        }
        case mal_seek_origin_current:
        {
            stream->SetPosition(stream->GetPosition() + offset);
            break;
        }
        default: NS_ASSERT_UNREACHABLE;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Audio::Play(const char* filename, float volume)
{
    if (!mal_device_is_started(&device))
    {
        mal_result r = mal_device_start(&device); 
        NS_ASSERT(r == MAL_SUCCESS);
    }

    for (int i = 0; i < MAX_VOICES; i++)
    {
        Voice* voice = voices + i;

        if (!voice->active)
        {
            Ptr<Stream> stream = GUI::LoadXamlResource(filename);
            if (stream != 0)
            {
                NS_LOG_TRACE("[%d] -> Playing '%s'", i, filename);
                voice->volume = volume;
                voice->stream = stream;

                mal_decoder_config config = mal_decoder_config_init(FORMAT, CHANNELS, SAMPLE_RATE);
                mal_result r = mal_decoder_init(&OnRead, &OnSeek, voice, &config, &voice->decoder);
                NS_ASSERT(r == MAL_SUCCESS);

                voice->active = true;
            }

            return;
        }
    }

    NS_LOG_WARNING("Free voice slot not found. Increase MAX_VOICES constant");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Audio::Pause()
{
    if (mal_device_is_started(&device))
    {
        mal_result r = mal_device_stop(&device);
        NS_ASSERT(r == MAL_SUCCESS);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Audio::Resume()
{
    if (!mal_device_is_started(&device))
    {
        mal_result r = mal_device_start(&device);
        NS_ASSERT(r == MAL_SUCCESS);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Audio::Shutdown()
{
    if (--initCount == 0)
    {
        mal_device_uninit(&device);

        for (int i = 0; i < MAX_VOICES; i++)
        {
            Voice* voice = voices + i;

            if (voice->active)
            {
                mal_result r = mal_decoder_uninit(&voice->decoder);
                NS_ASSERT(r == MAL_SUCCESS);
                voice->stream.Reset();
            }
        }
    }
}

#else

void NoesisApp::Audio::Init() {}
void NoesisApp::Audio::Play(const char*, float) {}
void NoesisApp::Audio::Pause() {}
void NoesisApp::Audio::Resume() {}
void NoesisApp::Audio::Shutdown() {}

#endif
