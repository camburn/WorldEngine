#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

#include <cstdint>
#include <memory>

namespace engine {

class Texture {
public:
    virtual ~Texture() = default;
    virtual uint32_t get_width() const = 0;
    virtual uint32_t get_height() const = 0;

    virtual void bind(uint32_t slot = 0) const = 0;
};

class Texture2D: public Texture {
public:
    static std::shared_ptr<Texture2D> create(const std::string& path);
};

class TextureHDR: public Texture {
public:
    static std::shared_ptr<TextureHDR> create(const std::string& path);
};

class TextureCubeMap: public Texture {
public:
    virtual void set_data(uint32_t face_index) = 0;

    static std::shared_ptr<TextureCubeMap> create(uint32_t width, uint32_t height);
};

class TextureDepth: public Texture {
public:
    static std::shared_ptr<TextureDepth> create(uint32_t width, uint32_t height);
};

} // namespace

#endif
