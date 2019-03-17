////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_AUDIO_H__
#define __APP_AUDIO_H__


#include <NsCore/Noesis.h>


namespace NoesisApp
{
namespace Audio
{

/// Initializes audio subsystem
void Init();

/// Plays audio file
void Play(const char* filename, float volume);

/// Halts the currently playing audio
void Pause();

/// Resumes playback
void Resume();

/// Close audio subsystem
void Shutdown();

}
}

#endif
