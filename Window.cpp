#include "Window.hpp"

Window::Window() : m_pWindow(nullptr) {}

Window::Window(std::string_view title, uint32_t width, uint32_t height) {
    m_pWindow = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
}

Window::Window(Window&& other) : m_pWindow(other.m_pWindow) {
    other.m_pWindow = nullptr;
}

Window::~Window() {
    if(m_pWindow != nullptr) {
        SDL_DestroyWindow(m_pWindow);
    }
}

Window& Window::operator = (Window&& rhs) {
    m_pWindow = rhs.m_pWindow;
    rhs.m_pWindow = nullptr;
    return *this;
}

Window::operator SDL_Window*() {
    SDL_assert(m_pWindow);
    return m_pWindow;
}

void Window::show() {
    SDL_assert(m_pWindow);
    SDL_ShowWindow(m_pWindow);
}

void Window::hide() {
    SDL_assert(m_pWindow);
    SDL_HideWindow(m_pWindow);
}

Window createWindow(std::string_view title, uint32_t width, uint32_t height) {
    return Window(title, width, height);
}

bool Window::pollEvent(Events& events) {

    return true;
}