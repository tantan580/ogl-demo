#include "blur_shader.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

//最主要是kernel的生成，它决定着BLUR的效果如何
unsigned int VBO, cubeVAO;
void setData()
{
    GLfloat vdata[] = {
            -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, 1.0f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
            1.0f, -1.0f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

            -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, -1.0f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
        };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    //texture attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
            (const GLvoid*)(5*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //create texture and fbo

}
//1.draw a quad with a texture 绘制屏幕 ，将场景绘制到纹理中，这里直接读取一张图片作为资源
//2.fbo h  对1中生成的纹理进行垂直高斯模糊运算，并将结果存储在另一张临时纹理中
//3.fbo ver and draw to screen 对2中生成的纹理进行水平方向的高斯模糊运算，然后直接绘制到屏幕上
int main()
{
    initGlfw();
    createWindow();
    //shader configuration
    ShaderManager shader;

    //set data
    setData();

    while (!glfwWindowShouldClose(gl_window))
    {
        processInput(gl_window);

        glfwSwapBuffers(gl_window);
        glfwPollEvents();
    }


}
