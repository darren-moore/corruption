#include "AudioEngine.h"

void AudioEngine::Init()
{
	m_engine = irrklang::createIrrKlangDevice();
}

void AudioEngine::Shutdown()
{
	m_engine->drop();
	m_audioSource.clear();
}

void AudioEngine::Play(std::string src, const char* key, bool looping) 
{	

	irrklang::ISound*  newSound = m_engine->play2D(src.c_str(), looping, false, true, irrklang::ESM_AUTO_DETECT, true);	

	if (newSound) 
	{		
		this->m_audioSource.insert({ key, newSound }); 		
		
		//std::cout << looping << std::endl;	
	}
	
}

void AudioEngine::Pause(std::string key) {

	m_audioSource[key]->setIsPaused();

}

void AudioEngine::PauseAll()
{
	m_engine->setAllSoundsPaused();

}

void AudioEngine::ResumeAll()
{
	m_engine->setAllSoundsPaused(false);

}

void AudioEngine::StopAll()
{
	m_engine->stopAllSounds();

}

void AudioEngine::AddEffect(std::string key, AudioEffect effectType)
{
	auto sound_idx = m_audioSource.find(key);

	if (sound_idx == m_audioSource.end()) {

		std::cerr << "Error locating sound file by key: "  << key << std::endl;

	}
	else {

		irrklang::ISoundEffectControl* control = sound_idx->second->getSoundEffectControl();

		if (!control) return;

		switch (effectType) {
		
		case AudioEffect::None:

			break;

		case AudioEffect::Gargle:

			control->enableGargleSoundEffect(500, false);

			break;

		case AudioEffect::Chorus:

			control->enableChorusSoundEffect();

			break;

		case AudioEffect::Distortion:

			control->enableDistortionSoundEffect();

			break;

		case AudioEffect::Echo:

			control->enableEchoSoundEffect();

			break;

		case AudioEffect::ParamEq:

			control->enableParamEqSoundEffect();

			break;

		case AudioEffect::Flanger:

			control->enableFlangerSoundEffect();

			break;

		case AudioEffect::WavesReverb:

			control->enableWavesReverbSoundEffect();

			break;

		case AudioEffect::Compressor:

			control->enableCompressorSoundEffect();

			break;

		case AudioEffect::Reverb3DL2:

			control->enableI3DL2ReverbSoundEffect();

			break;

		default:

			break;

		}
			
	}

}

void AudioEngine::RemoveEffects(std::string key)
{
	auto sound_idx = m_audioSource.find(key);

	if (sound_idx == m_audioSource.end()) {


		std::cerr << "Error. Could not find sound file with key: "  << key << std::endl;

	}
	else {

		irrklang::ISoundEffectControl* control = sound_idx->second->getSoundEffectControl();

		if (!control) return;

		control->disableAllEffects();
	}

}

void AudioEngine::SetVolume(std::string key, int volume)
{

	auto sound_idx = m_audioSource.find(key);

	if (sound_idx == m_audioSource.end()) {

		std::cerr << "Error. Could not find sound file with key: " << key << std::endl;

	}
	else {
				
		sound_idx->second->setVolume(volume);	
		
	}

}

void AudioEngine::Play(std::string key) {
	
	this->m_audioSource[key]->setIsPaused(false);

}

void AudioEngine::Stop(std::string key)
{

	auto idx = m_audioSource.find(key);

	if (!(idx == m_audioSource.end())) {
	
		idx->second->stop();
		
	}

}

void AudioEngine::PrintRegisteredSounds() {

	for (auto sound : m_audioSource) {
	
		std::cout << sound.first << " " << sound.second << std::endl;
	
	}

}
