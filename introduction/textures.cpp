//texture test
#include "common/common.h"
#include "common/shader.h"
#include "common/texture.h"
#include "common/filesystem.h"

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int tex1;
unsigned int tex2;

const char *vs = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "ourColor = aColor;\n"
        "TexCoord = vec2(aTexCoord.x, aTexCoord.y)\n;"
    "}\0";

const GLchar * fs = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    //FragColor =  texture(texture1, TexCoord) * vec4(ourColor, 1.0);
}

)";


void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //load and create a texture

    const std::string wall = "resources/wall.jpg";
    const std::string duck = "resources/duck.jpg";
    Texture texture;
    texture.createTexture2D(FileSystem::getPath(wall.c_str()), tex1);
    texture.createTexture2D(FileSystem::getPath(duck.c_str()), tex2);
}

int main()
{
    initGlfw();
    createWindow();
    GLuint vertexShader = ShaderManager::instance()->create_shader(vs,GL_VERTEX_SHADER);
    GLuint fragmentShader = ShaderManager::instance()->create_shader(fs,GL_FRAGMENT_SHADER);
    GLuint  shaderProgram = ShaderManager::instance()->create_program();

    setData();

    glUseProgram(shaderProgram);// don't forget to activate/use the shader before setting uniforms!
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gl_window))
    {
        // input
        // -----
        processInput(gl_window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2);

        // render container
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(gl_window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
