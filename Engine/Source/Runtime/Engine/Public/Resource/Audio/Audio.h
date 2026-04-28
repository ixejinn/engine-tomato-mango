#ifndef MANGO_AUDIO_H
#define MANGO_AUDIO_H

#include <vector>
#include <miniaudio/miniaudio.h>
#include "Resource/ResourceFwd.h"

namespace tomato {
    class Audio {
        explicit Audio(const char* filename, unsigned int simultaneousCnt);

    public:
        ~Audio();

        static void Initialize();
        static void Cleanup();
        static AssetID Create(const char* filename, unsigned int simultaneousCnt = 1);

        void SetLooping(bool repeat = true);

        void Start(bool fromBegin = true);
        void Stop();

    private:
        static ma_engine engine_;
        static ma_result engineInit_;

        std::vector<ma_sound> sounds_;
        std::size_t mask_;
    };
}

#endif //MANGO_AUDIO_H