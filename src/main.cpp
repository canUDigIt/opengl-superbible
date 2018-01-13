#include "application.h"

#include <gli/gli.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <cstring>
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <random>

struct droplet
{
    float x_offset;
    float rotation_speed;
    float fall_speed;
};

class my_app : public application
{
public:
    void startup()
    {
        rendering_program = compileShaders();
        glCreateVertexArrays(1, &vao);

        /* texture_name = createTexture("../bin/media/aliens.ktx"); */
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture_name);

        glBindTextureUnit(0, texture_name);

        gli::texture alien_texture = gli::load("media/aliens.ktx");

        gli::gl GL(gli::gl::PROFILE_GL33);
        gli::gl::format format = GL.translate(alien_texture.format(), alien_texture.swizzles());
        glm::tvec3<GLsizei> const extent(alien_texture.extent());

        glTextureStorage3D(
                texture_name,
                1,
                format.Internal,
                extent.x,
                extent.y,
                alien_texture.layers());

        for(std::size_t layer = 0; layer < alien_texture.layers(); ++layer)
        {
            glTextureSubImage3D(
                    texture_name,
                    0,
                    0,
                    0,
                    static_cast<GLint>(layer),
                    extent.x,
                    extent.y,
                    extent.z,
                    format.External,
                    format.Type,
                    alien_texture.data(layer, 0, 0));
        }

        glCreateBuffers(1, &alien_data);
        glNamedBufferStorage(alien_data, 256 * sizeof(glm::vec4), nullptr, GL_MAP_WRITE_BIT);

        droplets.reserve(256);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0, 1.0);
        for(unsigned int i = 0; i < 256; ++i)
        {
            // add data here
            droplets.push_back({
                dist(gen) * 2.0f - 1.0f,
                (dist(gen) + 0.5f) * ((i & 1) ? -3 : 3),
                dist(gen) + 0.2f
            });
        }

        glBindVertexArray(vao);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void shutdown()
    {
        glBindVertexArray(0);
        glDeleteProgram(rendering_program);
        glDeleteVertexArrays(1, &vao);
    }

    void render(double currentTime)
    {
        const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float t = static_cast<float>(currentTime);

        glClearBufferfv(GL_COLOR, 0, black);

        glUseProgram(rendering_program);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, alien_data);
        glm::vec4* data = static_cast<glm::vec4*>(glMapBufferRange(
                    GL_UNIFORM_BUFFER,
                    0,
                    256 * sizeof(glm::vec4),
                    GL_MAP_WRITE_BIT |
                    GL_MAP_INVALIDATE_BUFFER_BIT));

        for (int i = 0; i < 256; ++i) {
            data[i][0] = droplets[i].x_offset;
            data[i][1] = 2.0f - fmodf((t + 1) * droplets[i].fall_speed, 4.31f);
            data[i][2] = t * droplets[i].rotation_speed;
            data[i][3] = 0;
        }

        glUnmapBuffer(GL_UNIFORM_BUFFER);

        for (int i = 0; i < 256; ++i) {
            glVertexAttribI1i(0, i);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    void resize(int width, int height)
    {
        mWidth = width;
        mHeight = height;
        float aspect = (float)mWidth / (float)mHeight;

        glViewport(0, 0, mWidth, mHeight);
    }
private:
    GLuint createTexture(char const* filename)
    {
        GLuint texture_name = 0;

        gli::texture texture = gli::load(filename);
        if(texture.empty())
        {
            std::cerr << "Failed to load " << filename << std::endl;
        }

        gli::gl GL(gli::gl::PROFILE_GL33);
        gli::gl::format const format = GL.translate(texture.format(), texture.swizzles());
        GLenum target = GL.translate(texture.target());

        glGenTextures(1, &texture_name);
        glBindTexture(target, texture_name);
        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);

        glm::tvec3<GLsizei> const extent(texture.extent());
        GLsizei const face_total = static_cast<GLsizei>(texture.layers() * texture.faces());

        switch (texture.target())
        {
            case gli::TARGET_1D:
                glTexStorage1D(
                        target,
                        static_cast<GLint>(texture.levels()),
                        format.Internal,
                        extent.x);

                break;
            case gli::TARGET_1D_ARRAY:
            case gli::TARGET_2D:
            case gli::TARGET_CUBE:
                glTexStorage2D(
                        target,
                        static_cast<GLint>(texture.levels()),
                        format.Internal,
                        extent.x,
                        texture.target() == gli::TARGET_2D ? extent.y : face_total);
                break;
            case gli::TARGET_2D_ARRAY:
            case gli::TARGET_3D:
            case gli::TARGET_CUBE_ARRAY:
                glTexStorage3D(
                        target,
                        static_cast<GLint>(texture.levels()),
                        format.Internal,
                        extent.x,
                        extent.y,
                        texture.target() == gli::TARGET_3D ? extent.z : face_total);
                break;
            default:
                assert(0);
                break;

        }

        for(std::size_t layer = 0; layer < texture.layers(); ++layer)
        for(std::size_t face = 0; face < texture.faces(); ++face)
        for(std::size_t level = 0; level < texture.levels(); ++level)
        {
            GLsizei const layerGL = static_cast<GLsizei>(layer);
            glm::tvec3<GLsizei> extent(texture.extent(level));
            target = gli::is_target_cube(texture.target())
                ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face)
                : target;

            switch (texture.target())
            {
                case gli::TARGET_1D:
                    if(gli::is_compressed(texture.format()))
                    {
                        glCompressedTexSubImage1D(
                                target,
                                static_cast<GLint>(level),
                                0,
                                extent.x,
                                format.Internal,
                                static_cast<GLsizei>(texture.size(level)),
                                texture.data(layer, face, level));
                    }
                    else
                    {
                        glTexSubImage1D(
                                target,
                                static_cast<GLint>(level),
                                0,
                                extent.x,
                                format.External,
                                format.Type,
                                texture.data(layer, face, level));
                    }
                    break;
                case gli::TARGET_1D_ARRAY:
                case gli::TARGET_2D:
                case gli::TARGET_CUBE:
                    if(gli::is_compressed(texture.format()))
                    {
                        glCompressedTexSubImage2D(
                                target,
                                static_cast<GLint>(level),
                                0,
                                0,
                                extent.x,
                                texture.target() == gli::TARGET_1D_ARRAY ? layerGL : extent.y,
                                format.Internal,
                                static_cast<GLsizei>(texture.size(level)),
                                texture.data(layer, face, level));
                    }
                    else
                    {
                        glTexSubImage2D(
                                target,
                                static_cast<GLint>(level),
                                0,
                                0,
                                extent.x,
                                texture.target() == gli::TARGET_1D_ARRAY ? layerGL : extent.y,
                                format.External,
                                format.Type,
                                texture.data(layer, face, level));
                    }
                    break;
                case gli::TARGET_2D_ARRAY:
                case gli::TARGET_3D:
                case gli::TARGET_CUBE_ARRAY:
                    if(gli::is_compressed(texture.format()))
                    {
                        glCompressedTexSubImage3D(
                                target,
                                static_cast<GLint>(level),
                                0,
                                0,
                                0,
                                extent.x,
                                extent.y,
                                texture.target() == gli::TARGET_3D ? extent.z : layerGL,
                                format.Internal,
                                static_cast<GLsizei>(texture.size(level)),
                                texture.data(layer, face, level));
                    }
                    else
                    {
                        glTexSubImage3D(
                                target,
                                static_cast<GLint>(level),
                                0,
                                0,
                                0,
                                extent.x,
                                extent.y,
                                texture.target() == gli::TARGET_3D ? extent.z : layerGL,
                                format.External,
                                format.Type,
                                texture.data(layer, face, level));
                    }
                    break;
                default:
                    assert(0);
                    break;
            }
        }

        return texture_name;
    }

    GLuint createShader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            GLint log_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> log_data(log_length);
            glGetShaderInfoLog(shader, log_length, nullptr, log_data.data());
            std::string log(std::begin(log_data), std::end(log_data));
            std::cout << log << std::endl;
        }
        return shader;
    }

    GLuint compileShaders()
    {
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program;

        // Source code for vertex shader
        const char* vertex_shader_source = R"glsl(
            #version 420 core

            layout (location = 0) in int alien_index;

            out VS_OUT
            {
                flat int alien;
                vec2 tc;
            } vs_out;

            struct droplet_t
            {
                float x_offset;
                float y_offset;
                float orientation;
                float unused;
            };

            layout (std140) uniform droplets
            {
                droplet_t droplet[256];
            };

            void main(void)
            {
                const vec2[4] position = vec2[4](vec2(-0.5, -0.5),
                                                 vec2( 0.5, -0.5),
                                                 vec2(-0.5,  0.5),
                                                 vec2( 0.5,  0.5));
                vs_out.tc = position[gl_VertexID].xy + vec2(0.5);
                float co = cos(droplet[alien_index].orientation);
                float so = sin(droplet[alien_index].orientation);
                mat2 rot = mat2(vec2(co, so),
                                vec2(-so, co));
                vec2 pos = 0.25 * rot * position[gl_VertexID];
                gl_Position = vec4(pos.x + droplet[alien_index].x_offset,
                                   pos.y + droplet[alien_index].y_offset,
                                   0.5, 1.0);
                vs_out.alien = alien_index % 64;
            })glsl";

        // Source code for fragment shader
        const char* fragment_shader_source = R"glsl(
            #version 420 core

            layout (location = 0) out vec4 color;

            in VS_OUT
            {
                flat int alien;
                vec2 tc;
            } fs_in;

            layout (binding = 0) uniform sampler2DArray tex_aliens;

            void main(void)
            {
                color = texture(tex_aliens, vec3(fs_in.tc, float(fs_in.alien)));
                //color = vec4(1.0, 0.0, 0.0, 1.0);
            })glsl";

        // Create and compile shaders
        vertex_shader = createShader(GL_VERTEX_SHADER, vertex_shader_source);
        fragment_shader = createShader(GL_FRAGMENT_SHADER, fragment_shader_source);

        // Create program and attach shaders to it, and link it
        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        // Delete the shaders as the program has them now
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return program;
    }

    GLuint rendering_program;
    GLuint vao;
    GLuint texture_name;
    GLuint alien_data;

    std::vector<droplet> droplets;
};

int main(int argc, char* argv[])
{
    std::unique_ptr<my_app> pApp(new my_app);
    auto exit_status = pApp->run();

    return exit_status;
}
