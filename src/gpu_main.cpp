//
// Created by efarhan on 12/25/22.
//
#include <random>

#include "SDL.h"
#include "constant.h"

#include <GL/glew.h>
#include <fmt/core.h>
#include <regex>
#include "optick.h"


std::string GetGlError(GLenum err)
{
    std::string log;
    // Process/log the error.
    switch (err)
    {
    case GL_INVALID_ENUM:
        log = "GL Invalid Enum";
        break;
    case GL_INVALID_VALUE:
        log = "GL Invalid Value";
        break;
    case GL_INVALID_OPERATION:
        log = "GL Invalid Operation";
        break;
    case GL_OUT_OF_MEMORY:
        log = "GL Out Of Memory";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        log = "GL Invalid Framebuffer Operation";
        break;
    default:
        break;
    }
    return log;
}

void CheckGlError(const char* file, int line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        auto errorLog = GetGlError(err);
        if (errorLog.empty())
            continue;
        fmt::print("{}, in file: {} line: {}", errorLog, file, line);
    }
}
#define RELEASE
#ifdef RELEASE
#define glCheckError() void
#else
#define glCheckError() CheckGlError(__FILE__, __LINE__)
#endif
constexpr auto computeShaderContent = R"(#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba8, binding = 0) uniform image2D imgInput;
layout(rgba8, binding = 1) uniform image2D imgOutput;

const int width = ''width'';
const int height = ''height'';
const ivec2 neighbor_delta[8] = ivec2[8](
    ivec2(-1,-1),
    ivec2(-1,0),
    ivec2(0,-1),
    ivec2(0,1),
    ivec2(1,0),
    ivec2(1,-1),
    ivec2(-1,1),
    ivec2(1,1)
);

void main() 
{
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	bool pixel = imageLoad(imgInput, pixel_coords).x < 0.5;
    int count = 0;
    for(int i = 0; i < neighbor_delta.length(); i++)
    {
        ivec2 d = neighbor_delta[i];
        ivec2 neighbor = ivec2((pixel_coords.x+d.x+width)%width,(pixel_coords.y+d.y+height)%height);
        count += int(imageLoad(imgInput, neighbor).x < 0.5);
    }
    bool next_cell = (!pixel && count == 3) || (pixel && count >= 2 && count <= 3);
    float new_color = next_cell?0.0:1.0;  
    vec4 new_pixel = vec4(new_color, new_color, new_color, new_color);    
    imageStore(imgOutput, pixel_coords, new_pixel);
}
)";

//#define COPY_BACK
#ifdef COPY_BACK
constexpr auto copyBackShaderContent = R"(#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba8, binding = 1) uniform image2D imgInput;
layout(rgba8, binding = 0) uniform image2D imgOutput;

void main() 
{
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 pixel = imageLoad(imgInput, pixel_coords);    
    imageStore(imgOutput, pixel_coords, pixel);
}
)";
#endif
constexpr auto fragmentShaderContent = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

constexpr auto vertexShaderContent = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";



int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


    // Turn on double buffering with a 24bit Z buffer.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


    constexpr auto flags = SDL_WINDOW_OPENGL;


    SDL_Window* window_ = SDL_CreateWindow(
        "Game Of Life GPU",
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
    const SDL_GLContext glRenderContext_ = SDL_GL_CreateContext(window_);

    SDL_GL_MakeCurrent(window_, glRenderContext_);

    const GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::abort();
    }
    SDL_GL_SetSwapInterval(0);
    
    //Compute Shader
    const auto computeShader = glCreateShader(GL_COMPUTE_SHADER);
    std::string computeShaderContentAnnotated =computeShaderContent;
    computeShaderContentAnnotated = std::regex_replace(computeShaderContentAnnotated, std::regex("''width''"), fmt::to_string(width));
    computeShaderContentAnnotated = std::regex_replace(computeShaderContentAnnotated, std::regex("''height''"), fmt::to_string(height));

    const auto* ptr = computeShaderContentAnnotated.data();
    glShaderSource(computeShader, 1, &ptr, nullptr);
    glCompileShader(computeShader);
    int  success;
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        std::string infoLog(512u, '\0');
        glGetShaderInfoLog(computeShader, 512, nullptr, infoLog.data());
        fmt::print("{}\nShader content:\n{}", infoLog, computeShaderContentAnnotated);
        std::abort();
    }
    //Compute Program
    const auto computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
    if (!success) 
    {
        std::string infoLog(512u, '\0');
        glGetProgramInfoLog(computeProgram, 512, nullptr, infoLog.data());
        std::abort();
    }
    glDeleteShader(computeShader);

#ifdef COPY_BACK
    //Copy Shader
    const auto copyShader = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(copyShader, 1, &copyBackShaderContent, nullptr);
    glCompileShader(copyShader);
    glGetShaderiv(copyShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::string infoLog(512u, '\0');
        glGetShaderInfoLog(copyShader, 512, nullptr, infoLog.data());
        fmt::print("{}\nShader content:\n{}", infoLog, copyBackShaderContent);
        std::abort();
    }
    //Copy Program
    const auto copyProgram = glCreateProgram();
    glAttachShader(copyProgram, copyShader);
    glLinkProgram(copyProgram);
    glGetProgramiv(copyProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::string infoLog(512u, '\0');
        glGetProgramInfoLog(copyProgram, 512, nullptr, infoLog.data());
        std::abort();
    }
    glDeleteShader(copyShader);
#endif
    //Create quad vao
    constexpr float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    constexpr unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //vertex shader
    const auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, & vertexShaderContent, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::string infoLog(512u, '\0');
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog.data());
        fmt::print("{}\nShader content:\n{}", infoLog, vertexShaderContent);
        std::abort();
    }
    //framgent shader
    const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderContent, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::string infoLog(512u, '\0');
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog.data());
        fmt::print("{}\nShader content:\n{}", infoLog, fragmentShaderContent);
        std::abort();
    }
    //Show Program
    const auto showProgram = glCreateProgram();
    glAttachShader(showProgram, vertexShader);
    glAttachShader(showProgram, fragmentShader);
    glLinkProgram(showProgram);
    glGetProgramiv(showProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::string infoLog(512u, '\0');
        glGetProgramInfoLog(showProgram, 512, nullptr, infoLog.data());
        std::abort();
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(showProgram);
    const auto textureLocation = glGetUniformLocation(showProgram, "ourTexture");
    glCheckError();
    //create previous texture
    unsigned int previousTexture;
    glGenTextures(1, &previousTexture);
    glBindTexture(GL_TEXTURE_2D, previousTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    std::vector<unsigned> previousMap(size, 0u);
    {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> distrib(0, 1);
        
        for (auto& cell : previousMap)
        {
            cell = distrib(gen)?0xFFFFFFFF:0;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, previousMap.data());
    glCheckError();
    //create current texture
    GLuint currentTexture;
    glGenTextures(1, &currentTexture);
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        nullptr);
    glCheckError();
    bool isOpen = true;
    while (isOpen)
    {
        OPTICK_FRAME("GameFrame");
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isOpen = false;
            }
        }
        
        {
            OPTICK_EVENT("Computation");
            glBindImageTexture(0, previousTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
            glBindImageTexture(1, currentTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
            //Compute
            glUseProgram(computeProgram);
            glDispatchCompute(static_cast<GLuint>(width), static_cast<GLuint>(height), 1);

            //Waiting for compute to finish
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }
#ifdef COPY_BACK
        {
            OPTICK_EVENT("Copy Back");
            glBindImageTexture(0, previousTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
            glBindImageTexture(1, currentTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
            //Compute
            glUseProgram(copyProgram);
            glDispatchCompute(static_cast<GLuint>(width), static_cast<GLuint>(height), 1);

            //Waiting for compute to finish
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            glCheckError();
        }
#endif
        
        {
            OPTICK_EVENT("Graphics Commands");
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);


            glUseProgram(showProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, currentTexture);
            glUniform1i(textureLocation, 0);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glCheckError();
        }
#ifndef COPY_BACK
        std::swap(previousTexture, currentTexture);
#endif
        {
            OPTICK_EVENT("Swap Window");
            SDL_GL_SwapWindow(window_);
            glCheckError();
        }
    }

    glUseProgram(0);
    glDeleteProgram(computeProgram);
#ifdef COPY_BACK
    glDeleteProgram(copyProgram);
#endif

    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &previousTexture);
    glDeleteTextures(1, &currentTexture);

    glCheckError();

    SDL_GL_DeleteContext(glRenderContext_);

    SDL_DestroyWindow(window_);
    SDL_Quit();
    return 0;
}