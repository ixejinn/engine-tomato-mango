#ifndef MANGO_TEXTUREATLAS_H
#define MANGO_TEXTUREATLAS_H

namespace tomato {
    class TextureAtlas
    {
    public:
        TextureAtlas(int w = 2048, int h = 2048);
        ~TextureAtlas();

        bool Allocate(int w, int h, int& outX, int& outY); //packing
        void Upload(int x, int y, int w, int h, unsigned char* data);
        void Bind(unsigned int unit = 0) const;
        void Clear();

        unsigned int GetID() const { return textureID_; }

        int GetWidth() const { return width_; }
        int GetHeight() const { return height_; }

        bool Empty() const { return isEmpty_; }

    private:
        unsigned int textureID_;
        int width_, height_;

        // For Shelf Packing
        int currentX_{0};
        int currentY_{0};
        int maxRowHeight_{0};

        bool isEmpty_{true};
    };
}

#endif //MANGO_TEXTUREATLAS_H