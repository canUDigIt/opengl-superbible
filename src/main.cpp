#include <application.h>

#include <cmath>
#include <memory>

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
        static const GLfloat color[] = { (float)std::sin(currentTime) * 0.5f + 0.5f, 
                                         (float)std::cos(currentTime) * 0.5f + 0.5f,
                                         0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);

        glPointSize(40.0f);
        glDrawArrays(GL_POINTS, 0, 1);
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
            "#version 450 core \n",
            "\n",
            "void main(void) \n",
            "{ \n",
            "   gl_Position = vec4(0.0, 0.0, 0.0, 1.0); \n",
            "} \n"
        };

        // Source code for fragment shader
        static const GLchar* fragment_shader_source[] = 
        {
            "#version 450 core \n",
            " \n", 
            "out vec4 color; \n",
            " \n",
            "void main(void) \n", 
            "{ \n"
            "   color = vec4(0.0, 0.8, 1.0, 1.0); \n",
            "} \n"
        };
        
        // Create and compile vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        // Create and compile fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);

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
