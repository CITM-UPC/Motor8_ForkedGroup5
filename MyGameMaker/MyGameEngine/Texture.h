#pragma once

#include <memory>
#include <string>
#include "Image.h"
#include "TextureImporter.h"

class Texture {
public:
    enum WrapModes { Repeat, MirroredRepeat, Clamp };
    WrapModes wrapMode = Repeat;

    enum Filters { Nearest, Linear };
    Filters filter = Nearest;

private:
    std::shared_ptr<Image> _img_ptr;
    unsigned int id1;
    int width, height;

public:
    unsigned int id() const { return _img_ptr ? _img_ptr->id() : 0; }
    void bind() const;

    // Set image from shared_ptr
    void setImage(const std::shared_ptr<Image>& img_ptr) {
        _img_ptr = img_ptr;
        if (_img_ptr) {
            width = _img_ptr->width();
            height = _img_ptr->height();
        }
    }

    const auto& image() const { return *_img_ptr; }

    // Load from file using TextureImporter
    bool loadFromFile(const std::string& path);

    // Save to a binary file
    bool saveToBinaryFile(const std::string& filePath) const;

    // Load from a binary file
    bool loadFromBinaryFile(const std::string& filePath);
};
