#ifndef MANGO_TEXTURE_H
#define MANGO_TEXTURE_H

#include <glad/glad.h>

namespace tomato {
    class Texture {
    public:
        enum class Format
        {
            // LDR(Low Dynamic Range)
            RGB8,           // 투명도 없는 이미지 (JPG)
            RGBA8,          // 투명도 있는 표준 이미지 (PNG)
            SRGBA8,         // 감마 보정이 적용된 표준 이미지

            // HDR(High Dynamic Range)
            RGBA16F,        // 반정밀도 부동소수점

            R8,             // 흑백, 마스크, 글꼴, 그림자 맵

            Depth24Stencil8 // 깊이 + 스텐실
        };

        constexpr static const char* PrimitiveName = "Texture::Primitive";

    private:
        Texture();
        Texture(const char* filename, Format format = Format::RGBA8);

    public:
        ~Texture();

        static void Initialize() {}
        static void Cleanup() {}
        static void Create();
        static void Create(const char* filename, Format format = Format::RGBA8);

        void Bind() const;

    private:
        struct GLFormat
        {
            GLenum internalFormat;  // GPU가 VRAM에 저장하는 방식
            GLenum format;          // 데이터의 채널 구성
            GLenum type;            // 데이터의 자료형
            int channels;           // 채널 개수
        };

        static GLFormat ConvertFormatGL(Format format);

        GLuint textureID_{0};
        GLFormat format_;
    };
}

#endif //MANGO_TEXTURE_H