//vbo only use
//为简化起见，我们有一个称为顶点数组对象（VAO）的新事物，
//它可以记住您要使用的所有顶点缓冲区以及每个缓冲区的内存布局。
//我们为每个网格设置一次顶点数组对象。当我们想要绘制时，我们要做的就是绑定VAO并绘制。
//https://www.cnblogs.com/mazhenyu/p/6547219.html

//闭源驱动VBO单独就可以使用，并且有兼容模式，老版本OPENGL和新版本都可以一起使用
#include "common/common.h"
#include "common/shader.h"

const char *vs = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fs = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

unsigned int VBO;
void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    glGenBuffers(1, &VBO);
    // 0. 复制顶点数组到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 1. 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);// 解除当前绑定的VBO
}
int shaderProgram = -1;
int main()
{
    initGlfw();
    createWindow();

    ShaderManager::instance()->create_shader(vs,GL_VERTEX_SHADER);
    ShaderManager::instance()->create_shader(fs,GL_FRAGMENT_SHADER);
    // link shaders
    int shaderProgram = ShaderManager::instance()->create_program();

    setData();
    // render loop
    while (!glfwWindowShouldClose(gl_window))
    {
        processInput(gl_window);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle// 2. 当我们渲染一个物体时要使用着色器程序
        glUseProgram(shaderProgram);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(gl_window);
        glfwPollEvents();
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // 使用完要解除VBO绑定
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

