#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

class WindowEvent {

};

using Events = std::vector<WindowEvent>;

class Window {

    private:
        SDL_Window* m_pWindow;

    public:
        Window();
        Window(std::string_view title, uint32_t width, uint32_t height);
        Window(const Window& other) = delete;
        Window(Window&& other);
        ~Window();

    public:
        Window& operator = (const Window& rhs) = delete;
        Window& operator = (Window&& rhs);

        operator SDL_Window*();

        void show();
        void hide();

        bool pollEvent(Events& events);
};