#include "../include/application.h"

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

class my_app : public application
{
public:
    void startup()
    {
        float aspect = (float)mWidth / (float)mHeight;
        proj_matrix = glm::perspective(50.0f, aspect, 0.1f, 1000.0f);

        rendering_program = compileShaders();
        glCreateVertexArrays(1, &vao);

        static const GLfloat vertices[] = {
            -1.0f,-1.0f,-1.0f, // triangle 1 : begin
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, // triangle 1 : end
             1.0f, 1.0f,-1.0f, // triangle 2 : begin
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f, // triangle 2 : end
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f
        };

        glCreateBuffers(1, &geometry);
        glNamedBufferStorage(geometry, sizeof(vertices), vertices, GL_MAP_WRITE_BIT);

        void* ptr = glMapNamedBuffer(geometry, GL_WRITE_ONLY);
        std::memcpy(ptr, vertices, sizeof(vertices));
        glUnmapNamedBuffer(geometry);

        GLuint position_index = glGetAttribLocation(rendering_program, "position");
        GLuint vertices_index = 0;
        glVertexArrayAttribFormat(vao, position_index, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, position_index, vertices_index);
        glVertexArrayVertexBuffer(vao, vertices_index, geometry, 0, 3 * sizeof(GLfloat));
        glEnableVertexArrayAttrib(vao, position_index);

        glBindVertexArray(vao);
    }

    void shutdown()
    {
        glBindVertexArray(0);
        glDeleteProgram(rendering_program);
        glDeleteVertexArrays(1, &vao);
    }

    void render(double currentTime)
    {
        const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                                  (float)cos(currentTime) * 0.5f + 0.5f,
                                  0.0f, 0.0 };

        glClearBufferfv(GL_COLOR, 0, color);

        float k = (float)currentTime * glm::pi<float>() * 0.1f;
        glm::mat4 mv_matrix = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, -4.0f });
        mv_matrix = glm::translate(mv_matrix, { std::sinf(2.1f * k) * 0.5f, std::cosf(1.7f* k) * 0.5f, std::sinf(1.3f * k) * 0.5f });
        mv_matrix = glm::rotate(mv_matrix, (float)currentTime * glm::radians(45.0f), { 0.0f, 1.0f, 0.0f });
        mv_matrix = glm::rotate(mv_matrix, (float)currentTime * glm::radians(81.0f), { 1.0f, 0.0f, 0.0f });
        mv_matrix = glm::scale(mv_matrix, { 0.25, 0.25, 0.25 });

        glUseProgram(rendering_program);

        glUniformMatrix4fv(glGetUniformLocation(rendering_program, "mv_matrix"), 1, GL_FALSE, glm::value_ptr(mv_matrix));
        glUniformMatrix4fv(glGetUniformLocation(rendering_program, "proj_matrix"), 1, GL_FALSE, glm::value_ptr(proj_matrix));

        glBindBuffer(GL_ARRAY_BUFFER, geometry);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void resize(int width, int height)
    {
        mWidth = width;
        mHeight = height;
        float aspect = (float)mWidth / (float)mHeight;
        proj_matrix = glm::perspective(50.0f, aspect, 0.1f, 1000.0f);
    }
private:
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
            #version 450 core

            layout (location = 0) in vec4 position;

            out VS_OUT
            {
                vec4 color;
            } vs_out;

            uniform mat4 mv_matrix;
            uniform mat4 proj_matrix;

            void main(void)
            {
                gl_Position = proj_matrix * mv_matrix * position;
                vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
            })glsl";

        // Source code for fragment shader
        const char* fragment_shader_source = R"glsl(
            #version 450 core

            in VS_OUT
            {
                vec4 color;
            } fs_in;

            out vec4 color;

            void main(void)
            {
                color = fs_in.color;
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
    GLuint geometry;
    GLuint vao;

    glm::mat4 proj_matrix;
};

int main(int argc, char* argv[])
{
    std::unique_ptr<my_app> pApp(new my_app);
    auto exit_status = pApp->run();

    return exit_status;
}
