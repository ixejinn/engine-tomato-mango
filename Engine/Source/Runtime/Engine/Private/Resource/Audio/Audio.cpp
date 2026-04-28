#include <filesystem>
#include <bit>
#include "Resource/Audio/Audio.h"
#include "Resource/AssetRegistry.h"
#include "Resource/AssetHash.h"
#include "Utils/Logger.h"

namespace tomato {
    ma_engine Audio::engine_;
    ma_result Audio::engineInit_{MA_ERROR};

    Audio::Audio(const char *filename, unsigned int simultaneousCnt) {
        if (engineInit_ != MA_SUCCESS)
         Initialize();

        if (!std::has_single_bit(simultaneousCnt)) {
            auto msbPos = std::bit_width(simultaneousCnt);
            simultaneousCnt = (1 << msbPos);

            TMT_WARN << "simultaneousCnt is not a power of 2 (adjusted to " << simultaneousCnt << ")";
        }

        if (simultaneousCnt > 64) {
            simultaneousCnt = 64;

            TMT_WARN << "simultaneousCnt exceeds the limit of 64 (clamped to 64)";
        }

        mask_ = simultaneousCnt - 1;
        sounds_.resize(simultaneousCnt);

        ma_result result;
        for (auto& sound : sounds_) {
            result = ma_sound_init_from_file(&engine_, filename, MA_SOUND_FLAG_STREAM, NULL, NULL, &sound);
            if (result != MA_SUCCESS)
                break;
        }

        if (result != MA_SUCCESS)
            TMT_WARN << "failed to initialize sound from file: " << filename;
    }

    Audio::~Audio() {
        for (auto& sound : sounds_)
            ma_sound_uninit(&sound);
    }

    void Audio::Initialize() {
        engineInit_ = ma_engine_init(NULL, &engine_);
         if (engineInit_ != MA_SUCCESS)
             TMT_WARN << "failed to initialize engine";
    }

    void Audio::Cleanup() {
        ma_engine_uninit(&engine_);
    }

    AssetID Audio::Create(const char* filename, unsigned int simultaneousCnt) {
        std::unique_ptr<Audio> ptr{new Audio(filename, simultaneousCnt)};
        AssetRegistry<Audio>::GetInstance().Register(filename, std::move(ptr));

        return GetAssetID(filename);
    }

    void Audio::SetLooping(bool repeat) {
        if (sounds_.size() == 1)
            ma_sound_set_looping(&sounds_[0], repeat ? MA_TRUE : MA_FALSE);
        else
            TMT_WARN << "SFX cannot set looping";
    }

    void Audio::Start(bool fromBegin) {
        static unsigned int idx = 0;
        auto curIdx = idx & mask_;

        if (ma_sound_is_playing(&sounds_[curIdx])) {
            TMT_WARN << "Exceeds SFX max counts";
            return;
        }
        ++idx;

        if (fromBegin)
            ma_sound_seek_to_pcm_frame(&sounds_[curIdx], 0);
        ma_sound_start(&sounds_[curIdx]);
    }

    void Audio::Stop() {
        if (sounds_.size() == 1)
            ma_sound_stop(&sounds_[0]);
        else
            TMT_WARN << "SFX cannot stop";
    }
}
