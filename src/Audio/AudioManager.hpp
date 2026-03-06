#pragma once

#include <dr/dr_mp3.h>
#include <dr/dr_wav.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <filesystem>
#include <vector>
#include <unordered_map>

namespace ke
{
    namespace Audio
    {
        class Audio
        {
        public:
            Audio() = default;
            Audio(const std::string& filepath, ALboolean looping, float volume, float pitch);
            ~Audio();

            void Play() const;
            void Pause() const;
            void Stop() const;

            Audio(const Audio&) = delete;
            Audio& operator=(const Audio&) = delete;

            Audio(Audio&& other) noexcept
                :mBuffer(other.mSource), mSource(other.mSource)
            {
                other.mBuffer = 0;
                other.mSource = 0;
            }

            Audio& operator=(Audio&& other) noexcept
            {
                if(this == &other) return *this;

                alDeleteSources(1, &mSource);
                alDeleteBuffers(1, &mBuffer);

                mBuffer = other.mBuffer;
                mSource = other.mSource;

                other.mBuffer = 0;
                other.mSource = 0;

                return *this;
            }

        private:
            ALuint mBuffer;
            ALuint mSource;
        };

        class AudioManager
        {
        public:
            static AudioManager& getInstance()
            {
                static AudioManager instance;
                return instance;
            }
            void init();

            uint16_t createAudio(const std::string& filepath, ALboolean looping, float volume, float pitch, const std::string& name);
            uint16_t getAudioIndex(const std::string& name);

            void PlayAudio(uint16_t idx) const;
            void PauseAudio(uint16_t idx) const;
            void StopAudio(uint16_t idx) const;

            void terminate();
        private:
            AudioManager() = default;

            std::vector<Audio> mAudioList;
            std::unordered_map<std::string, uint16_t> mIndexMap;
            
            ALCdevice* mDevice = nullptr;
            ALCcontext* mContext = nullptr;
        };
    }
}