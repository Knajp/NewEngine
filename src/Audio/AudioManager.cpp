#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#include "AudioManager.hpp"
#include <iostream>

void ke::Audio::AudioManager::init()
{
    mDevice = alcOpenDevice(nullptr);
    mContext = alcCreateContext(mDevice, nullptr);
    
    alcMakeContextCurrent(mContext);
}

uint16_t ke::Audio::AudioManager::createAudio(const std::string& filepath, ALboolean looping, float volume, float pitch, const std::string& name)
{
    uint16_t idx = static_cast<uint16_t>(mAudioList.size());
    mAudioList.emplace_back(filepath, looping, volume, pitch);   

    mIndexMap[name] = idx;
    return idx;
}

uint16_t ke::Audio::AudioManager::getAudioIndex(const std::string& name)
{
    return mIndexMap[name];
}

void ke::Audio::AudioManager::PlayAudio(uint16_t idx) const
{
    mAudioList[idx].Play();
}

void ke::Audio::AudioManager::PauseAudio(uint16_t idx) const
{
    mAudioList[idx].Pause();
}

void ke::Audio::AudioManager::StopAudio(uint16_t idx) const
{
    mAudioList[idx].Stop();
}

void ke::Audio::AudioManager::terminate()
{
    mAudioList.clear();
    
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);
}

ke::Audio::Audio::Audio(const std::string& filepath, ALboolean looping, float volume, float pitch)
{
    alGenBuffers(1, &mBuffer);

    std::filesystem::path path(filepath);

    if(path.extension() == ".wav")
    {
        drwav wav;
        if(!drwav_init_file(&wav, path.string().c_str(), nullptr))
            std::cerr << "Failed to open wav file!";

        std::vector<int16_t> pcmData(wav.totalPCMFrameCount * wav.channels);
        drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcmData.data());
        drwav_uninit(&wav);

        ALenum format = (wav.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        alBufferData(mBuffer, format, pcmData.data(), pcmData.size() * sizeof(int16_t), wav.sampleRate);
    }
    else if(path.extension() == ".mp3")
    {
        drmp3 mp3;
        if(!drmp3_init_file(&mp3, path.string().c_str(), nullptr))
            std::cerr << "Failed to open mp3 file!";
        
        drmp3_uint64 frameCount = drmp3_get_pcm_frame_count(&mp3);
        std::vector<int16_t> pcmData(frameCount * mp3.channels);
        drmp3_read_pcm_frames_s16(&mp3, frameCount, pcmData.data());
        drmp3_uninit(&mp3);

        ALenum format = (mp3.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        alBufferData(mBuffer, format, pcmData.data(), pcmData.size() * sizeof(int16_t), mp3.sampleRate);
    }
    else std::cerr << "Invalid audio file format!";

    alGenSources(1, &mSource);
    alSourcei(mSource, AL_BUFFER, mBuffer);
    alSourcei(mSource, AL_LOOPING, looping);
    alSourcei(mSource, AL_PITCH, pitch);
    alSourcei(mSource, AL_GAIN, volume);

}

ke::Audio::Audio::~Audio()
{
    alDeleteSources(1, &mSource);
    alDeleteBuffers(1, &mBuffer);
}

void ke::Audio::Audio::Play() const
{
    alSourcePlay(mSource);
}

void ke::Audio::Audio::Pause() const
{
    alSourcePause(mSource);
}

void ke::Audio::Audio::Stop() const
{
    alSourceStop(mSource);
}
