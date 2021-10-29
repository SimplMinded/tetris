#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <glad/glad.h>
#include <SDL.h>

#include "list_view.h"

constexpr const char* vertex_shader = 
    "#version 400 core\n"
    "layout(location=0) in vec4 in_position;\n"
    "layout(location=1) in vec3 in_color;\n"
    "out vec4 vert_color;\n"
    "void main() {\n"
    "  gl_Position = in_position;\n"
    "  vert_color = vec4(in_color, 1);\n"
    "}\n";

constexpr const char* fragment_shader =
    "#version 400 core\n"
    "in vec4 vert_color;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "  frag_color = vert_color;\n"
    "}\n";

struct Vertex
{
    float posX;
    float posY;
    float r;
    float g;
    float b;
};

struct Quad
{
    Vertex bottomLeft;
    Vertex topLeft;
    Vertex topRight;
    Vertex bottomRight;
};

static const char* getGlErrorMessage(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR: return "No error";
        case GL_INVALID_ENUM: return "Invalid enum";
        case GL_INVALID_VALUE: return "Invalid value";
        case GL_INVALID_OPERATION: return "Invalid operation";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
        case GL_OUT_OF_MEMORY: return "Out of memory";
        case GL_STACK_UNDERFLOW: return "Stack underflow";
        case GL_STACK_OVERFLOW: return "Stack overflow";
        default: return "Unknown error";
    }
}

#define GL_ASSERT(x) \
    while (glGetError() != GL_NO_ERROR); \
    x; \
    if (const GLenum error = glGetError(); error != GL_NO_ERROR) \
        fprintf(stderr, "[GL_ASSERT] %s:%d: \"%s\" (%s)\n", __FILE__, __LINE__, #x, getGlErrorMessage(error))

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // TODO: Logging.
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);

    SDL_Window* window = SDL_CreateWindow("Tetris", 0, 0, 720, 480, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        // TODO: Logging.
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (SDL_GL_CreateContext(window) == nullptr) {
        // TODO: Logging.
        fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        // TODO: Logging.
        fprintf(stderr, "GLAD initialization failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    glClearColor(1, 0, 1, 1);

    // Rendering.
    uint32_t vao;
    GL_ASSERT(glGenVertexArrays(1, &vao));
    GL_ASSERT(glBindVertexArray(vao));

    uint32_t vbo;
    GL_ASSERT(glGenBuffers(1, &vbo));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_ASSERT(glEnableVertexAttribArray(0));
    GL_ASSERT(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));

    GL_ASSERT(glEnableVertexAttribArray(1));
    GL_ASSERT(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)));

    GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

    uint32_t ibo;
    GL_ASSERT(glGenBuffers(1, &ibo));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

    uint32_t indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW));

    GL_ASSERT(uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER));
    GL_ASSERT(glShaderSource(vertexShader, 1, &vertex_shader, nullptr));
    GL_ASSERT(glCompileShader(vertexShader));

    GLint vertexCompiled;
    GL_ASSERT(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexCompiled));
    if (vertexCompiled != GL_TRUE) {
        GLsizei logLength = 0;
        char message[1024];
        GL_ASSERT(glGetShaderInfoLog(vertexShader, 1024, &logLength, message));
        fprintf(stderr, "Vertex shader compilation error: %s\n", message);
    }

    GL_ASSERT(uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));
    GL_ASSERT(glShaderSource(fragmentShader, 1, &fragment_shader, nullptr));
    GL_ASSERT(glCompileShader(fragmentShader));

    GLint fragmentCompiled;
    GL_ASSERT(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentCompiled));
    if (fragmentCompiled != GL_TRUE) {
        GLsizei logLength = 0;
        char message[1024];
        GL_ASSERT(glGetShaderInfoLog(fragmentShader, 1024, &logLength, message));
        fprintf(stderr, "Fragment shader compilation error: %s\n", message);
    }

    GL_ASSERT(uint32_t program = glCreateProgram());
    GL_ASSERT(glAttachShader(program, vertexShader));
    GL_ASSERT(glAttachShader(program, fragmentShader));
    GL_ASSERT(glLinkProgram(program));

    GLint programLinked;
    GL_ASSERT(glGetProgramiv(program, GL_LINK_STATUS, &programLinked));
    if (programLinked != GL_TRUE) {
        GLsizei logLength = 0;
        char message[1024];
        GL_ASSERT(glGetProgramInfoLog(program, 1024, &logLength, message));
        fprintf(stderr, "Shader program linking error: %s\n", message);
    }

    GL_ASSERT(glUseProgram(program));

    GL_ASSERT(glDeleteShader(vertexShader));
    GL_ASSERT(glDeleteShader(fragmentShader));

    ListView<Quad> quads = makeListView(4, new Quad[4]);

    while (!SDL_QuitRequested())
    {
        Quad quad = Quad{
            Vertex{ -0.5f, -0.5f, 0, 1, 0 },
            Vertex{ -0.5f, 0.5f, 0, 1, 0 },
            Vertex{ 0.5f, 0.5f, 0, 1, 0 },
            Vertex{ 0.5, -0.5f, 0, 1, 0 }
        };

        glClear(GL_COLOR_BUFFER_BIT);
        clear(quads);
        add(quads, quad);
        GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, quads.count * sizeof(Quad), quads.elems));
        GL_ASSERT(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        SDL_GL_SwapWindow(window);
    }

    GL_ASSERT(glDeleteProgram(program));
    GL_ASSERT(glDeleteBuffers(1, &vbo));
    GL_ASSERT(glDeleteVertexArrays(1, &vao));

    SDL_Quit();
}
