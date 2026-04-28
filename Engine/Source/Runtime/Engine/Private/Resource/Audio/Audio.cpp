#include <filesystem>
#include "Resource/Audio/Audio.h"
#include "Resource/AssetRegistry.h"
#include "Resource/AssetHash.h"
#include "Utils/Logger.h"

namespace tomato {
    ma_engine Audio::engine_;
    ma_result Audio::engineResult_;

    Audio::Audio(const char *filename) {
        // if (engineResult_ != MA_SUCCESS)
            // Initialize();

        engineResult_ = ma_engine_init(NULL, &engine_);
        if (engineResult_ != MA_SUCCESS)
            TMT_WARN << "failed to initialize engine";

        TMT_INFO << "Initializing Audio";
        if (ma_sound_init_from_file(&engine_, filename, MA_SOUND_FLAG_STREAM, NULL, NULL, &sound_) != MA_SUCCESS)
            TMT_WARN << "failed to initialize sound from file: " << filename;
    }

    Audio::~Audio() {
        ma_sound_uninit(&sound_);
    }

    void Audio::Initialize() {
        // TMT_INFO << "Initializing Audio engine";
        // engineResult_ = ma_engine_init(NULL, &engine_);
        // if (engineResult_ != MA_SUCCESS)
        //     TMT_WARN << "failed to initialize engine";
    }

    void Audio::Cleanup() {
        ma_engine_uninit(&engine_);
    }

    AssetID Audio::Create(const char* filename) {
        std::unique_ptr<Audio> ptr{new Audio(filename)};
        AssetRegistry<Audio>::GetInstance().Register(filename, std::move(ptr));

        return GetAssetID(filename);
    }

    void Audio::SetLooping(bool repeat) {
        ma_sound_set_looping(&sound_, repeat ? MA_TRUE : MA_FALSE);
    }

    void Audio::Start(bool fromBegin) {
        if (fromBegin)
            ma_sound_seek_to_pcm_frame(&sound_, 0);
        ma_sound_start(&sound_);
    }

    void Audio::Stop() {
        ma_sound_stop(&sound_);
    }
}
