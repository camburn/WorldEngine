#ifndef _BUFFER_HPP
#define _BUFFER_HPP
#include <string>
#include <vector>
#include <memory>
//#include <cstdint>
#include <map>

#include "Engine/renderer/texture.hpp"

namespace engine {

enum class ShaderDataType {
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Mat4,
    Mat3,
    Bool,
    Char,
    uChar,
    Short,
    uShort,
    uInt,
};

struct DataTypeDescriptor {
    uint32_t size;
    uint32_t components;
};

static std::map<ShaderDataType, DataTypeDescriptor> shader_types {
    {ShaderDataType::Float,  {4,  1}},
    {ShaderDataType::Float2, {8,  2}},
    {ShaderDataType::Float3, {12, 3}},
    {ShaderDataType::Float4, {16, 4}},
    {ShaderDataType::Int,    {4,  1}},
    {ShaderDataType::Int2,   {8,  2}},
    {ShaderDataType::Int3,   {12, 3}},
    {ShaderDataType::Int4,   {16, 4}},
    {ShaderDataType::Mat3,   {36, 9}},
    {ShaderDataType::Mat4,   {64, 16}},
    {ShaderDataType::Bool,   {1,  1}},
};

class BufferElement {
public:
    ShaderDataType type;
    std::string name;
    uint32_t size;
    intptr_t offset;
    bool normalised;

    BufferElement() {}

    BufferElement(ShaderDataType type, const std::string& name, bool normalised = false)
            : type(type), name(name), size(shader_types[type].size), offset(0), normalised(normalised)
    {
    }

    uint32_t get_component_count() const { return shader_types[type].components; }
};


class BufferLayout{
public:
    BufferLayout() {}
    BufferLayout(const std::vector<BufferElement>& elements)
            : elements(elements) {
        calculate_offset_stride();
    }
    BufferLayout(const std::initializer_list<BufferElement>& elements)
            : elements(elements) {
        calculate_offset_stride();
    }
    BufferLayout(uint32_t stride)
            : stride(stride) {
    }

    inline uint32_t get_stride() const { return stride; }
    inline const std::vector<BufferElement>& get_elements() const { return elements; }

    void add_element(BufferElement &element) { elements.push_back(element); }

    std::vector<BufferElement>::iterator begin() { return elements.begin(); }
    std::vector<BufferElement>::iterator end() { return elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return elements.end(); }

private:
    void calculate_offset_stride() {
        uint32_t offset = 0;
        stride = 0;
        for (auto& element: elements) {
            element.offset = offset;
            offset += element.size;
            stride += element.size;
        }
    }

    std::vector<BufferElement> elements;
    uint32_t stride = 0;
};

class VertexBuffer {
public:
    virtual ~VertexBuffer() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void read_data() = 0;

    virtual const engine::BufferLayout& get_layout() const = 0;
    virtual void set_layout(const engine::BufferLayout& layout) = 0;

    static std::shared_ptr<VertexBuffer> create(void* vertices, uint32_t size);
};

class IndexBuffer {
public:
    virtual ~IndexBuffer() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual uint32_t get_count() const = 0;
    virtual ShaderDataType get_type() const = 0;
    virtual uint32_t get_offset() const = 0;

    static std::shared_ptr<IndexBuffer> create(std::shared_ptr<IndexBuffer> buffer, uint32_t count, uint32_t offset);
    static std::shared_ptr<IndexBuffer> create(uint32_t* indices, uint32_t count);
    static std::shared_ptr<IndexBuffer> create(void* indices, uint32_t count, uint32_t size);
    static std::shared_ptr<IndexBuffer> create(void* indices, uint32_t count, uint32_t size, uint32_t offset);
    static std::shared_ptr<IndexBuffer> create(void* indices, uint32_t count, uint32_t size, uint32_t offset, ShaderDataType type);
};

class FrameBuffer {
public: 
    virtual ~FrameBuffer() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;
    virtual void resize(uint32_t width, uint32_t height) = 0;

    static std::shared_ptr<FrameBuffer> create(uint32_t width, uint32_t height);
    static std::shared_ptr<FrameBuffer> create(const std::shared_ptr<TextureDepth>& texture);
};

}  // namespace
#endif