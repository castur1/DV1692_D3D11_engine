#include "renderer.hpp"
#include "logging.hpp"

Renderer::Renderer() {}

Renderer::~Renderer() {}

bool Renderer::Initialize() {
    LogInfo(" > Creating renderer...\n");

    LogInfo(" > Renderer created\n");

    return true;
}