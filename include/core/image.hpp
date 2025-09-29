#ifndef LUADIO_IMAGE_HPP
#define LUADIO_IMAGE_HPP

#include <string>
#include <cstdint>

namespace luadio 
{
    class image
	{
    public:
        image();
        image(const std::string &filepath);
        image(const uint8_t *compressedData, size_t size);
        image(const uint8_t *uncompressedData, size_t size, uint32_t width, uint32_t height, uint32_t channels);
        image(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a);
        image(const image &other);
        image(image &&other) noexcept;
        image& operator=(const image &other);
        image& operator=(image &&other) noexcept;
        ~image();
        uint8_t *get_data() const;
        uint32_t get_width() const;
        uint32_t get_height() const;
        uint32_t get_channels() const;
        size_t get_data_size() const;
        bool is_loaded() const;
    private:
        uint8_t *data;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        bool hasLoaded;
        bool load_from_file(const std::string &filepath);
        bool load_from_memory(const uint8_t *data, size_t size);
        bool load(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a);
    };
}

#endif