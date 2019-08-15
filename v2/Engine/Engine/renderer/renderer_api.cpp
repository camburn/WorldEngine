#include "engine.hpp"
#include "renderer_api.hpp"

#include "Engine/renderer/renderer.hpp"
#ifdef OPENGL_COMPATIBILITY
#include "Platform/OpenGL2/gl_renderer_api.hpp"
#else
#include "Platform/OpenGL/gl_renderer_api.hpp"
#endif

namespace engine {
    #ifdef OPENGL_COMPATIBILITY
    RendererAPI::API RendererAPI::api = RendererAPI::API::OpenGL2;
    RendererAPI* Renderer::renderer_api = new glengine::GL2RendererAPI;
    #else
    RendererAPI::API RendererAPI::api = RendererAPI::API::OpenGL;
    RendererAPI* Renderer::renderer_api = new glengine::GLRendererAPI;
    #endif
}