
#include "common/shader.h"
#include "common/texture.h"
#include "common/camera.h"
#include "common/window.h"
#include "common/common.h"

const char *lighting_vs = R"(
                       #version 330 core
                       layout (location = 0) in vec3 position;
                       layout (location = 1) in vec3 normal;
                       uniform mat4 model;
                       uniform mat4 view;
                       uniform mat4 projection;

                       out vec3 FragPos;
                       out vec3 Normal;
                       void main()
                       {
                           gl_Position = projection * view * model * vec4(position, 1.0);
                           FragPos = vec3(model * vec4(position, 1.f));
                           Normal = mat3(transpose(inverse(model))) * normal;//
                       }
                       )";

const char * lighting_fs = R"(
                           #version 330 core
                           out vec4 color;

                           in vec3 Normal;
                           in vec3 FragPos;

                           uniform vec3 lightPos;
                           uniform vec3 lightColor;
                           uniform vec3 objectColor;
                           uniform vec3 viewPos;

                           void main()
                           {
                               // Ambient
                               float ambientStrength = 0.1f;
                               vec3 ambient = ambientStrength * lightColor;

                               // Diffuse
                               vec3 norm = normalize(Normal);
                               vec3 lightDir = normalize(lightPos - FragPos);
                               float diff = max(dot(norm, lightDir), 0.0);
                               vec3 diffuse = diff * lightColor;

                                // Specular
                               float specularStrength = 0.5f;
                               vec3 viewDir = normalize(viewPos - FragPos);
                               vec3 reflectDir = reflect(-lightDir, norm);
                               float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
                               vec3 specular = specularStrength * spec * lightColor;


                               vec3 result = (ambient + diffuse + specular) * objectColor;
                               color = vec4(specular, 1.0f);
                           }
)";
const char * lamp_vs=R"(#version 330 core
                      layout (location = 0) in vec3 aPos;

                      uniform mat4 model;
                      uniform mat4 view;
                      uniform mat4 projection;

                      void main()
                      {
                          gl_Position = projection * view * model * vec4(aPos, 1.0);
                      })";

const char * lamp_fs=R"(#version 330 core
                      out vec4 FragColor;

                      void main()
                      {
                          FragColor = vec4(1.0); // set alle 4 vector values to 1.0
                      })";

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    OGLWindow window;
    GLFWwindow *gl_window = window.createWindow(800, 600, "spec");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    ShaderManager lighting_shader;
    lighting_shader.create_shader(lighting_vs,GL_VERTEX_SHADER);
    lighting_shader.create_shader(lighting_fs,GL_FRAGMENT_SHADER);
    lighting_shader.create_program();
    ShaderManager lamp_shader;
    lamp_shader.create_shader(lamp_vs,GL_VERTEX_SHADER);
    lamp_shader.create_shader(lamp_fs,GL_FRAGMENT_SHADER);
    lamp_shader.create_program();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lighting_shader.use();
        lighting_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lighting_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lighting_shader.setVec3("lightPos", lightPos);
        lighting_shader.setVec3("viewPos", -2.0f, -1.0f, 5.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lighting_shader.setMat4("projection", projection);
        lighting_shader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lighting_shader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        lamp_shader.use();
        lamp_shader.setMat4("projection", projection);
        lamp_shader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lamp_shader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(gl_window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
