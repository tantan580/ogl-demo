#include "common/common.h"
#include "common/shader.h"
#include "common/texture.h"
#include "common/camera.h"

const char *vs = R"(
                       #version 330 core
                       layout (location = 0) in vec3 aPos;

                       uniform mat4 model;
                       uniform mat4 view;
                       uniform mat4 projection;

                       void main()
                       {
                           gl_Position = projection * view * model * vec4(aPos, 1.0);
                       }
                       )";

const char * fs = R"(#version 330 core
                         out vec4 FragColor;

                         uniform vec3 objectColor;
                         uniform vec3 lightColor;

                         void main()
                         {
                             FragColor = vec4(lightColor * objectColor, 1.0);
                         }
)";

unsigned int VBO, cubeVAO;

void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
       // ------------------------------------------------------------------
       float vertices[] = {
            1, 1, 0,
           -1, 1, 0,
           -1, -1, 0,
           1, -1, 0,

           0.5, 0.5, 0,
          -0.5, 0.5, 0,
          -0.5, -0.5, 0,
          0.5, -0.5, 0,
       };
       glGenVertexArrays(1, &cubeVAO);
       glGenBuffers(1, &VBO);
       glBindBuffer(GL_ARRAY_BUFFER, VBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

       glBindVertexArray(cubeVAO);
       // position attribute
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);

       // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
       glBindBuffer(GL_ARRAY_BUFFER, VBO);

       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);
}

int main()
{
    initGlfw();
    createWindow();

    ShaderManager shader;
    shader.buildShader(vs,GL_VERTEX_SHADER);
    shader.buildShader(fs,GL_FRAGMENT_SHADER);
    GLuint pro = shader.link();
    setData();
    //render loop
    while (!glfwWindowShouldClose(gl_window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        timing::deltaTime = currentFrame - timing::lastFrame;
        timing::lastFrame = currentFrame;
        // input
        // -----
        processInput(gl_window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glUseProgram(pro);
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        shader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        // render the cube
        glBindVertexArray(cubeVAO);
        //draw stencil
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0x01);
        glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);  // INVERT = even-odd rule
        glStencilFunc(GL_ALWAYS, 0, ~0);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

        //draw triangle
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilFunc(GL_EQUAL, 0x01, 0x01);
        glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisable(GL_STENCIL_TEST);
        glfwSwapBuffers(gl_window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
