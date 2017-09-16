#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true);

public:

	bool Init();
	bool CleanUp();

private:

	Mix_Music*			music = nullptr;
	list<Mix_Chunk*>	fx;

public:

	// Play a music file
	bool			PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);
	// Load a WAV in memory
	unsigned int	LoadFx(const char* path);
	// Play a previously loaded WAV
	bool			PlayFx(unsigned int fx,int channel = -1, int repeat = 0);

};

#endif // __ModuleAudio_H__