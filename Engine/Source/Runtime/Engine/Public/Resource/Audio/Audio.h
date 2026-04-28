#ifndef MANGO_AUDIO_H
#define MANGO_AUDIO_H

#include <miniaudio/miniaudio.h>
#include "Resource/ResourceFwd.h"

namespace tomato {
    class Audio {
        Audio(const char* filename);

    public:
        ~Audio();

        static void Initialize();
        static void Cleanup();
        static AssetID Create(const char* filename);

        void SetLooping(bool repeat = true);

        void Start(bool fromBegin = true);
        void Stop();

    private:
        static ma_engine engine_;
        static ma_result engineResult_;

        ma_sound sound_;
    };
}

#endif //MANGO_AUDIO_H