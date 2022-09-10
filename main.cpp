#include <iostream>
#include <fstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// callbacks copied(modified to use std::ifstream) from SFML/Graphics/ImageLoader.cpp
// stb_image callbacks that operate on a sf::InputStream
namespace
{
    int read(void* user, char* data, int size)
    {
        auto* stream = static_cast<std::ifstream*>(user);
        stream->read(data, std::streamsize{ size });
        return stream->gcount();
    }

    void skip(void* user, int size)
    {
        auto* stream = static_cast<std::ifstream*>(user);
        stream->seekg(stream->tellg() + std::streamoff{ size });
    }

    int eof(void* user)
    {
        auto* stream = static_cast<std::ifstream*>(user);
        return stream->eof();
    }

    // stb_image callback for constructing a buffer
    void bufferFromCallback(void* context, void* data, int size)
    {
        auto* source = static_cast<char*>(data);
        auto* dest = static_cast<std::vector<char>*>(context);
        std::copy(source, source + size, std::back_inserter(*dest));
    }
}

int main()
{
    auto stream = std::ifstream{ "../../../hard_vacuum_mid.bmp", std::ios::binary };
    assert(stream.is_open());
    assert(stream.good());
    auto pixels = std::vector<char>{};

    // Setup the stb_image callbacks
    stbi_io_callbacks callbacks;
    callbacks.read = &read;
    callbacks.skip = &skip;
    callbacks.eof = &eof;

    // Load the image and get a pointer to the pixels in memory
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* ptr = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &channels, STBI_rgb_alpha);

    if (ptr)
    {
        if (width && height)
        {
            // Copy the loaded pixels to the pixel buffer
            pixels.resize(static_cast<std::size_t>(width * height * 4));
            memcpy(pixels.data(), ptr, pixels.size());
        }

        // Free the loaded pixels (they are now in our own pixel buffer)
        stbi_image_free(ptr);

        return EXIT_SUCCESS;
    }
    else
    {
        // Error, failed to load the image
        std::cerr << "Failed to load image from stream. Reason: " << stbi_failure_reason() << std::endl;

        return EXIT_FAILURE;
    }
}
