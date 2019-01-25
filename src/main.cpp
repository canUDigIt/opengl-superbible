#include <application.h>

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
            -0.25f,  0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f,  0.25f,

            -0.25f,  0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
             0.25f,  0.25f,  0.25f,

             0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f, -0.25f
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

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void shutdown()
    {
        glBindVertexArray(0);
        glDeleteProgram(rendering_program);
        glDeleteVertexArrays(1, &vao);
    }

    void render(double currentTime)
    {
        const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 0.0 };
        GLfloat one = 1.0f;

        glClearBufferfv(GL_COLOR, 0, green);
        glClearBufferfv(GL_DEPTH, 0, &one);

        glUseProgram(rendering_program);

        glUniformMatrix4fv(glGetUniformLocation(rendering_program, "proj_matrix"), 1, GL_FALSE, glm::value_ptr(proj_matrix));

        GLuint mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
        for (int i = 0; i < 24; ++i)
        {
            float k = (float)i + (float)currentTime * 0.3f;
            glm::mat4 mv_matrix = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, -20.0f });
            mv_matrix = glm::rotate(mv_matrix, (float)currentTime * glm::radians(45.0f), { 0.0f, 1.0f, 0.0f });
            mv_matrix = glm::rotate(mv_matrix, (float)currentTime * glm::radians(21.0f), { 1.0f, 0.0f, 0.0f });
            mv_matrix = glm::translate(mv_matrix, {
                std::sin(2.1f * k) * 2.0f,
                std::cos(1.7f * k) * 2.0f,
                std::sin(1.3f * k) * std::cos(1.5f * k) * 2.0f });

            glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    void resize(int width, int height)
    {
        mWidth = width;
        mHeight = height;
        float aspect = (float)mWidth / (float)mHeight;
        proj_matrix = glm::perspective(50.0f, aspect, 0.1f, 1000.0f);

        glViewport(0, 0, mWidth, mHeight);
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

            std::string log(log_length, ' ');
            glGetShaderInfoLog(shader, log_length, nullptr, &log[0]);
            std::cout << log << "\n";
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
