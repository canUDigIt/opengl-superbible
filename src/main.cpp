#include <application.h>

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
        rendering_program = compile_shaders();
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

        const GLfloat attrib[] = { (float)sin(currentTime) * 0.5f,
                                   (float)cos(currentTime) * 0.6f,
                                   0.0f, 0.0f };

        glVertexAttrib4fv(0, attrib);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

private:
    GLuint compile_shaders()
    {
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program;

        // Source code for vertex shader
        static const GLchar* vertex_shader_source[] = 
        {
            "#version 450 core                                \n"
            "                                                 \n"
            "                                                 \n"
            "layout (location = 0) in vec4 offset;            \n"
            "                                                 \n"
            "void main(void)                                  \n"
            "{                                                \n"
            "   // Declare a hard-coded array of positions    \n"
            "   const vec4 vertices[3] = {                    \n"
            "       vec4(  0.25, -0.25, 0.5, 1.0 ),           \n"
            "       vec4( -0.25, -0.25, 0.5, 1.0 ),           \n"
            "       vec4(  0.25,  0.25, 0.5, 1.0 )            \n"
            "   };                                            \n"
            "                                                 \n"
            "   // Index into our array using gl_VertexID     \n"
            "   gl_Position = vertices[gl_VertexID] + offset; \n"
            "}                                                \n"
        };

        // Source code for fragment shader
        static const GLchar* fragment_shader_source[] = 
        {
            "#version 450 core                    \n"
            "                                     \n"
            "out vec4 color;                      \n"
            "                                     \n"
            "void main(void)                      \n"
            "{                                    \n"
            "   color = vec4(0.0, 0.8, 1.0, 1.0); \n"
            "}                                    \n"
        };

        // Create and compile vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        GLint success = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            GLint log_length;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> log_data(log_length);
            glGetShaderInfoLog(vertex_shader, log_length, nullptr, log_data.data());
            std::string log(std::begin(log_data), std::end(log_data));
            std::cout << log << std::endl;
        }

        // Create and compile fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            GLint log_length;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> log_data(log_length);
            glGetShaderInfoLog(fragment_shader, log_length, nullptr, log_data.data());
            std::string log(std::begin(log_data), std::end(log_data));
            std::cout << log << std::endl;
        }

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
