#pragma once

#include "R5RRasterizer.h"

#include <GLES3/gl3.h>

#include <numeric>
#include <vector>

#define VS                                                                                                             \
    "#version 300 es\n"                                                                                                \
    "precision mediump float;\n"                                                                                       \
    "layout(location = 0) in vec3 position;\n"                                                                         \
    "layout(location = 1) in vec2 texcoord;\n"                                                                         \
    "out vec2 _texcoord_;\n"                                                                                           \
    "void main() {\n"                                                                                                  \
    "  _texcoord_ = texcoord;\n"                                                                                       \
    "  gl_Position = vec4(position.x, -position.y, 0.0, 1.0);\n"                                                       \
    "}\n"

#define FS                                                                                                             \
    "#version 300 es\n"                                                                                                \
    "precision mediump float;\n"                                                                                       \
    "in vec2 _texcoord_;\n"                                                                                            \
    "uniform sampler2D tex0;\n"                                                                                        \
    "layout(location = 0) out vec4 fragColor;\n"                                                                       \
    "void main() {\n"                                                                                                  \
    "  fragColor = vec4(texture(tex0, _texcoord_).rgb, 1.0);\n"                                                        \
    "}\n"

const std::vector<float> verticesTexcoords = { 1.0,  -1.0, 0.0, 1.0, 0.0, -1.0, 1.0,  0.0, 0.0, 1.0,
                                               -1.0, -1.0, 0.0, 0.0, 0.0, 1.0,  -1.0, 0.0, 1.0, 0.0,
                                               1.0,  1.0,  0.0, 1.0, 1.0, -1.0, 1.0,  0.0, 0.0, 1.0 };

const std::vector<unsigned short> indices = { 0, 1, 2, 3, 4, 5 };

#define IMAGE_WIDTH  256
#define IMAGE_HEIGHT 256

#define RENDER_WIDTH  512
#define RENDER_HEIGHT 512

struct Renderer
{
    Renderer()
    {
        textureWidth  = IMAGE_WIDTH;
        textureHeight = IMAGE_HEIGHT;

        GLuint vsId     = glCreateShader(GL_VERTEX_SHADER);
        GLuint fsId     = glCreateShader(GL_FRAGMENT_SHADER);
        shaderProgramId = glCreateProgram();
        std::string       VSString(VS);
        std::string       FSString(FS);
        std::vector<char> vsSrcCode(VSString.begin(), VSString.end());
        std::vector<char> fsSrcCode(FSString.begin(), FSString.end());
        auto              vsSrc    = static_cast<const GLchar*>(vsSrcCode.data());
        auto              vsSrcLen = static_cast<const GLint>(vsSrcCode.size());
        auto              fsSrc    = static_cast<const GLchar*>(fsSrcCode.data());
        auto              fsSrcLen = static_cast<const GLint>(fsSrcCode.size());
        glShaderSource(vsId, 1, &vsSrc, &vsSrcLen);
        glCompileShader(vsId);
        glShaderSource(fsId, 1, &fsSrc, &fsSrcLen);
        glCompileShader(fsId);
        glAttachShader(shaderProgramId, vsId);
        glAttachShader(shaderProgramId, fsId);
        glLinkProgram(shaderProgramId);
        glDetachShader(shaderProgramId, vsId);
        glDetachShader(shaderProgramId, fsId);
        glDeleteShader(vsId);
        glDeleteShader(fsId);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
        ASSERT_ERROR(textureId != 0, "opengl texture id is zero, something went wrong in creating it");

        const size_t VERTEX_SIZE     = 3 * sizeof(float) + 2 * sizeof(float);
        const size_t POSITION_STRIDE = 0;
        const size_t TEXCOORD_STRIDE = POSITION_STRIDE + 3 * sizeof(float);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesTexcoords.size() * sizeof(float), &verticesTexcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void*)(POSITION_STRIDE));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void*)(TEXCOORD_STRIDE));
        glEnableVertexAttribArray(1);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    ~Renderer()
    {
        printf("[STATUS] deleting Renderer\n");
        glDeleteProgram(shaderProgramId);
        glDeleteTextures(1, &textureId);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
    }

    void recreateGLTexture()
    {
        glDeleteTextures(1, &textureId);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        ASSERT_ERROR(textureId != 0, "opengl texture id is zero, something went wrong in creating it");
    }

    void resizeGLTexture(int w, int h)
    {
        textureWidth  = w;
        textureHeight = h;
        recreateGLTexture();
    }

    void clearGLTextureData() { updateSubImageData(0, textureWidth, 0, textureHeight); }

    void updateSubImageData(size_t xmin, size_t xmax, size_t ymin, size_t ymax)
    {
        glBindTexture(GL_TEXTURE_2D, textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, xmin, ymin, xmax - xmin, ymax - ymin, GL_RGBA, GL_FLOAT, textureDataPtr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void renderToScreen()
    {
        glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramId);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shaderProgramId, "tex0"), 0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint shaderProgramId;
    GLuint textureId;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    float* textureDataPtr;
    size_t textureWidth;
    size_t textureHeight;
};
#undef VS
#undef FS