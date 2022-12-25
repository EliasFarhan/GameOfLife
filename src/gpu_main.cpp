//
// Created by efarhan on 12/25/22.
//
#include "SDL.h"
#include "constant.h"

#include <GL/glew.h>


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    SDL_Window* window_;
    SDL_GLContext glRenderContext_;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


    const auto flags = SDL_WINDOW_RESIZABLE |
                       SDL_WINDOW_OPENGL;


    window_ = SDL_CreateWindow(
            "GL Playground",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            static_cast<int>(width),
            static_cast<int>(height),
            flags
    );
    if(window_ == nullptr)
    {
        std::abort();
    }
    glRenderContext_ = SDL_GL_CreateContext(window_);

    SDL_GL_MakeCurrent(window_, glRenderContext_);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::abort();
    }
    SDL_GL_SetSwapInterval(0);

    //TODO load shaders and programs


    bool isOpen = true;
    while (isOpen)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isOpen = false;
            }
        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(window_);
    }
    SDL_GL_DeleteContext(glRenderContext_);

    SDL_DestroyWindow(window_);
    SDL_Quit();
    return 0;
}