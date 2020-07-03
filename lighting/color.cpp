#include "common.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

const char *color_vs = R"(
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

const char * color_fs = R"(#version 330 core
                         out vec4 FragColor;

                         uniform vec3 objectColor;
                         uniform vec3 lightColor;

                         void main()
                         {
                             FragColor = vec4(lightColor * objectColor, 1.0);
                         }
)";

const char * light_vs=R"(#version 330 core
                      layout (location = 0) in vec3 aPos;

                      uniform mat4 model;
                      uniform mat4 view;
                      uniform mat4 projection;

                      void main()
                      {
                          gl_Position = projection * view * model * vec4(aPos, 1.0);
                      })";

const char * light_fs=R"(#version 330 core
                      out vec4 FragColor;

                      void main()
                      {
                          FragColor = vec4(1.0); // set alle 4 vector values to 1.0
                      })";

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
unsigned int VBO, cubeVAO;
unsigned int lightCubeVAO;
void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
       // ------------------------------------------------------------------
       float vertices[] = {
           -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
           -0.5f,  0.5f, -0.5f,
           -0.5f, -0.5f, -0.5f,

           -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
           -0.5f,  0.5f,  0.5f,
           -0.5f, -0.5f,  0.5f,

           -0.5f,  0.5f,  0.5f,
           -0.5f,  0.5f, -0.5f,
           -0.5f, -0.5f, -0.5f,
           -0.5f, -0.5f, -0.5f,
           -0.5f, -0.5f,  0.5f,
           -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

           -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
           -0.5f, -0.5f,  0.5f,
           -0.5f, -0.5f, -0.5f,

           -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
           -0.5f,  0.5f,  0.5f,
           -0.5f,  0.5f, -0.5f,
       };
       glGenVertexArrays(1, &cubeVAO);
       glGenBuffers(1, &VBO);
       glBindBuffer(GL_ARRAY_BUFFER, VBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

       glBindVertexArray(cubeVAO);
       // position attribute
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);

       glGenVertexArrays(1, &lightCubeVAO);
       glBindVertexArray(lightCubeVAO);

       // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
       glBindBuffer(GL_ARRAY_BUFFER, VBO);

       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);
}

int main()
{
    initGlfw();
    createWindow();

    ShaderManager color_shader;
    color_shader.buildShader(color_vs,GL_VERTEX_SHADER);
    color_shader.buildShader(color_fs,GL_FRAGMENT_SHADER);
    GLuint color_pro = color_shader.link();
    ShaderManager light_shader;
    light_shader.buildShader(light_vs,GL_VERTEX_SHADER);
    light_shader.buildShader(light_fs,GL_FRAGMENT_SHADER);
    GLuint light_pro = light_shader.link();

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(color_pro);
        color_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        color_shader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        color_shader.setMat4("projection", projection);
        color_shader.setMat4("view", view);
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        color_shader.setMat4("model", model);
        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // also draw the lamp object
        glUseProgram(light_pro);
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        light_shader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(gl_window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
