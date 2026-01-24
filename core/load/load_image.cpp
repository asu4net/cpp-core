#include "load_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

auto Image::white() -> const Image&
{
    static u8 g_white_pixels[] = { 255, 255, 255, 255 };
    static Image image = { g_white_pixels, 1, 1, 4 };
    return image;
}

Image::Image(u8* data, i32 width, i32 height, i32 channels)
    : m_data(data)
    , m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_is_owner(false)
{
}

Image::Image(std::string_view filename)
{
    if (filename.empty())
    return;

    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(filename.data(), &m_width, &m_height, &m_channels, 0);
    dbg_check(data, "Image load failed!\n");
    m_data = data;
    m_is_owner = true;
}

Image::~Image()
{
    if (m_data && m_is_owner)
    {
        stbi_image_free(m_data);
        m_data = nullptr;
        m_is_owner = false;
    }
}

auto Image::valid() const -> bool
{
    bool is_valid = m_data != nullptr &&
        m_width    > 0 &&
        m_height   > 0 &&
        (m_channels == 1 || m_channels == 3 || m_channels == 4);
    return is_valid;
}
