#include <cstdio>
#include <cstdlib>
#include <glad/glad.h>

#include "util/assert.h"
#include "color.h"
#include "list_view.h"
#include "matrix.h"
#include "point.h"

namespace {

#define UNREACHABLE(...) \
    do { \
        fprintf(stderr, "[UNREACHABLE] %s:%d: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        abort(); \
    } while(false)

constexpr int32_t position_index = 0;
constexpr int32_t color_index = 1;

constexpr const char* vertex_shader =
    "#version 400 core\n"
    "layout(location=0) in vec4 in_position;\n"
    "layout(location=1) in vec3 in_color;\n"
    "uniform mat4 u_projection;\n"
    "out vec4 vert_color;\n"
    "void main() {\n"
    "  gl_Position = u_projection * in_position;\n"
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
    Point position;
    Color color;
};

struct Quad
{
    Vertex bottomLeft;
    Vertex topLeft;
    Vertex topRight;
    Vertex bottomRight;
};

const char* getGlErrorMessage(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "Invalid framebuffer operation";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        case GL_STACK_UNDERFLOW:
            return "Stack underflow";
        case GL_STACK_OVERFLOW:
            return "Stack overflow";
        default:
            UNREACHABLE("Unknown OpenGL error: %d", error);
    }
}

#define GL_ASSERT(stmt) \
    while (glGetError() != GL_NO_ERROR); \
    stmt; \
    if (const GLenum error = glGetError(); error != GL_NO_ERROR) { \
        fprintf(stderr, "[GL_ASSERT] %s:%d: \"%s\" (%s)\n", \
                __FILE__, \
                __LINE__, \
                #stmt, \
                getGlErrorMessage(error)); \
        abort(); \
    }

const char* shaderTypeToString(GLenum shaderType)
{
    switch (shaderType)
    {
        case GL_VERTEX_SHADER: return "vertex shader";
        case GL_FRAGMENT_SHADER: return "fragment shader";
        default: UNREACHABLE("Unknown shader type: %d", shaderType);
    }
}

constexpr int32_t message_length = 1024;

uint32_t createShader(GLenum type, const char* source)
{
    ASSERT(source != nullptr);

    GL_ASSERT(uint32_t shader = glCreateShader(type));
    GL_ASSERT(glShaderSource(shader, 1, &source, nullptr));
    GL_ASSERT(glCompileShader(shader));

    GLint compileStatus;
    GL_ASSERT(glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus));
    if (compileStatus != GL_TRUE) {
        GLsizei logLength = 0;
        char message[message_length];
        GL_ASSERT(glGetShaderInfoLog(
            shader,
            message_length,
            &logLength,
            message));
        fprintf(
            stderr,
            "Compilation of %s failed: %s\n",
            shaderTypeToString(type),
            message);
        abort();
    }
    
    return shader;
}

uint32_t createShaderProgram(const char* vertexShaderSource,
                             const char* fragmentShaderSource)
{
    GL_ASSERT(uint32_t program = glCreateProgram());

    uint32_t vertexShader =
        createShader(GL_VERTEX_SHADER, vertexShaderSource);
    uint32_t fragmentShader =
        createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GL_ASSERT(glAttachShader(program, vertexShader));
    GL_ASSERT(glAttachShader(program, fragmentShader));
    GL_ASSERT(glLinkProgram(program));

    GLint linkStatus;
    GL_ASSERT(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));
    if (linkStatus != GL_TRUE) {
        GLsizei logLength = 0;
        char message[message_length];
        GL_ASSERT(glGetProgramInfoLog(
            program,
            message_length,
            &logLength,
            message));
        fprintf(stderr, "Linking of shader program failed: %s\n", message);
        abort();
    }
    GL_ASSERT(glUseProgram(program));

    GL_ASSERT(glDeleteShader(vertexShader));
    GL_ASSERT(glDeleteShader(fragmentShader));

    return program;
}

Matrix makeOrthogonalProjectionMatrix(float left,
                                      float right,
                                      float top,
                                      float bottom,
                                      float near,
                                      float far)
{
    const float scaleX = 2 / (right - left);
    const float scaleY = 2 / (top - bottom);
    const float scaleZ = 2 / (near - far);
    const float translateX = (left + right) / (left - right);
    const float translateY = (bottom + top) / (bottom - top);
    const float translateZ = (near + far) / (near - far);
    return makeMatrix(
        scaleX, 0,      0,      translateX,
        0,      scaleY, 0,      translateY,
        0,      0,      scaleZ, translateZ,
        0,      0,      0,      1);
}

uint32_t vao;
uint32_t vbo;
uint32_t ibo;
uint32_t program;

ListView<Quad> quads;

}

void initRenderer(uint32_t maxSpriteCount, uint32_t windowWidth, uint32_t windowHeight)
{
    GL_ASSERT(glGenVertexArrays(1, &vao));
    GL_ASSERT(glBindVertexArray(vao));

    GL_ASSERT(glGenBuffers(1, &vbo));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_ASSERT(glEnableVertexAttribArray(position_index));
    GL_ASSERT(glVertexAttribPointer(
        position_index,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        0));

    GL_ASSERT(glEnableVertexAttribArray(color_index));
    GL_ASSERT(glVertexAttribPointer(
        color_index,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, color)));

    GL_ASSERT(glBufferData(
        GL_ARRAY_BUFFER,
        maxSpriteCount * sizeof(Quad),
        nullptr,
        GL_DYNAMIC_DRAW));

    GL_ASSERT(glGenBuffers(1, &ibo));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

    ListView<uint32_t> indices =
        makeListView(maxSpriteCount * 6, new uint32_t[maxSpriteCount * 6]);
    for (uint32_t index = 0; index < maxSpriteCount; index++)
    {
        add(indices, (index * 4) + 0);
        add(indices, (index * 4) + 1);
        add(indices, (index * 4) + 2);
        add(indices, (index * 4) + 0);
        add(indices, (index * 4) + 2);
        add(indices, (index * 4) + 3);
    }
    GL_ASSERT(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.count * sizeof(uint32_t),
        indices.elems,
        GL_STATIC_DRAW));
    delete[] indices.elems;

    program = createShaderProgram(vertex_shader, fragment_shader);

    const Matrix projection =
        makeOrthogonalProjectionMatrix(0, windowWidth, 0, windowHeight, -1, 1);
    GL_ASSERT(int32_t location =
        glGetUniformLocation(program, "u_projection"));
    ASSERT(location != -1);
    GL_ASSERT(glUniformMatrix4fv(location, 1, GL_FALSE, projection.elems));

    quads = makeListView(maxSpriteCount, new Quad[maxSpriteCount]);
}

void destroyRenderer()
{
    delete[] quads.elems;
    GL_ASSERT(glDeleteProgram(program));
    GL_ASSERT(glDeleteBuffers(1, &ibo));
    GL_ASSERT(glDeleteBuffers(1, &vbo));
    GL_ASSERT(glDeleteVertexArrays(1, &vao));

    vao = 0;
    vbo = 0;
    ibo = 0;
    program = 0;
}

void beginDrawing()
{
    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));
    clear(quads);
}

void endDrawing()
{
    GL_ASSERT(glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        quads.count * sizeof(Quad),
        quads.elems));
    GL_ASSERT(glDrawElements(
        GL_TRIANGLES,
        quads.count * 6,
        GL_UNSIGNED_INT,
        0));
}

void drawQuad(float x, float y, float width, float height, const Color& color)
{
    ASSERT(width > 0);
    ASSERT(height > 0);
    ASSERT(color.r >= 0 && color.r <= 1);
    ASSERT(color.g >= 0 && color.g <= 1);
    ASSERT(color.b >= 0 && color.b <= 1);

    const Quad quad = Quad{
        Vertex{ Point{ x, y }, color },
        Vertex{ Point{ x, y + height }, color },
        Vertex{ Point{ x + width, y + height }, color },
        Vertex{ Point{ x + width, y }, color }
    };
    add(quads, quad);
}
