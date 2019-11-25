#pragma once
#include <iostream>
#include <irrKlang.h>
#include <string>
#include <unordered_map>
#define AUDIO_ENGINE_CLASS

 enum class AudioEffect { None, Chorus, Compressor, Distortion, Echo, Flanger, Gargle, Reverb3DL2, ParamEq, WavesReverb };

class AudioEngine {

public:

	AudioEngine() = default;
	~AudioEngine() = default;

	void Init();
	void Shutdown();

	void Play(std::string src, const char* key, bool looping = false);
	void Pause(std::string key);
	void Play(std::string key);
	void Stop(std::string key);

	void PauseAll();
	void ResumeAll();
	void StopAll();

	void AddEffect(std::string key, AudioEffect effectType);
	void RemoveEffects(std::string key);
	void SetVolume(std::string key, int volume);

	void PrintRegisteredSounds();		

private:
	
	irrklang::ISoundEngine* m_engine;
	std::unordered_map <std::string , irrklang::ISound*> m_audioSource;
};