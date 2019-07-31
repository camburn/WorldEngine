#include "engine.hpp"
#include "renderer_api.hpp"

#include "Engine/renderer/renderer.hpp"
#include "Platform/OpenGL/gl_renderer_api.hpp"

namespace engine {
    RendererAPI::API RendererAPI::api = RendererAPI::API::OpenGL;
    RendererAPI* Renderer::renderer_api = new glengine::GLRendererAPI;
}