#include "../include/application.h"

#include <cmath>
#include <memory>
#include <iostream>
#include <string>
#include <vector>

class my_app : public application
{
public:
    void startup()
    {
        rendering_program = compileShaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);
    }

    void shutdown()
    {
        glBindVertexArray(0);
        glDeleteProgram(rendering_program);
        glDeleteVertexArrays(1, &vertex_array_object);
    }

    void render(double currentTime)
    {
        const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                                  (float)cos(currentTime) * 0.5f + 0.5f,
                                  0.0f, 0.0 };

        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);

        const GLfloat attrib[] = {
            (float)sin(currentTime) * 0.5f,
            (float)cos(currentTime) * 0.6f,
            0.0f, 0.0f
        };

        const GLfloat triangle_color[] = { 0.0f, 0.8f, 1.0f, 1.0f };

        glVertexAttrib4fv(0, attrib);
        glVertexAttrib4fv(1, triangle_color);

        glDrawArrays(GL_TRIANGLES, 0, 3);
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

            layout (location = 0) in vec4 offset;
            layout (location = 1) in vec4 color;

            out VS_OUT
            {
               vec4 color;
            } vs_out;

            void main(void)
            {
               // Declare a hard-coded array of positions
               const vec4 vertices[3] = {
                   vec4(  0.25, -0.25, 0.5, 1.0 ),
                   vec4( -0.25, -0.25, 0.5, 1.0 ),
                   vec4(  0.25,  0.25, 0.5, 1.0 )
               };

               // Index into our array using gl_VertexID
               gl_Position = vertices[gl_VertexID] + offset;

               // Output a fixed value for vs_out
               vs_out.color = color;
            })glsl";

        // Source code for fragment shader
        const char* fragment_shader_source = R"glsl(
            #version 450 core

            // Input from the vertex shader
            in VS_OUT
            {
                vec4 color;
            } fs_in;

            // Output to the framebuffer
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
    GLuint vertex_array_object;
};

int main(int argc, char* argv[])
{
    std::unique_ptr<my_app> pApp(new my_app);
    auto exit_status = pApp->run();

    return exit_status;
}
