/****phng  = 环境境光 + 漫反射光 + 镜面光
 * 环境光本身不提供最明显的光照效果，但是漫反射光照(Diffuse Lighting)会对物体产生显著的视觉影响。
 * 漫反射光使物体上与光线排布越近的片段越能从光源处获得更多的亮度。
 * 1.法向量：一个垂直于顶点表面的向量。
 * 2.定向的光线：作为光的位置和片段的位置之间的向量差的方向向量。为了计算这个光线，我们需要光的位置向量和片段的位置向量。
******/
#include "common/common.h"
#include "common/shader.h"
#include "common/texture.h"
#include "common/camera.h"

#include <iostream>

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
                           struct Material {
                                vec3 ambient;
                                vec3 diffuse;
                                vec3 specular;
                                float shininess;
                           };

                           struct Light {
                                vec3 position;
                                vec3 ambient;
                                vec3 diffuse;
                                vec3 specular;
                           };
                           out vec4 color;

                           in vec3 Normal;
                           in vec3 FragPos;

                           uniform vec3 viewPos;
                           uniform Material material;
                           uniform Light light;

                           void main()
                           {
                               // Ambient
                               vec3 ambient = light.ambient * material.ambient;

                               // Diffuse
                               vec3 norm = normalize(Normal);
                               vec3 lightDir = normalize(light.position - FragPos);
                               float diff = max(dot(norm, lightDir), 0.0);
                               vec3 diffuse = light.diffuse * (diff * material.diffuse);

                                // Specular
                               vec3 viewDir = normalize(viewPos - FragPos);
                               vec3 reflectDir = reflect(-lightDir, norm);
                               int shininess = 32;//镜面光的散射因子
                               float spec = pow(max(dot(reflectDir, viewDir), 0), material.shininess);
                               vec3 specular = light.specular * (spec * material.specular);

                               vec3 result = ambient + diffuse + specular;
                               color = vec4(result, 1.0f);
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
// lighting 光源位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
unsigned int VBO, cubeVAO;
unsigned int lightCubeVAO;
void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
       // ------------------------------------------------------------------
    GLfloat vertices[] = {
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
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

int main()
{
    initGlfw();
    createWindow();
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    ShaderManager lighting_shader;
    lighting_shader.create_shader(lighting_vs,GL_VERTEX_SHADER);
    lighting_shader.create_shader(lighting_fs,GL_FRAGMENT_SHADER);
    GLuint lighting_pro = lighting_shader.create_program();
    ShaderManager lamp_shader;
    lamp_shader.create_shader(lamp_vs,GL_VERTEX_SHADER);
    lamp_shader.create_shader(lamp_fs,GL_FRAGMENT_SHADER);
    GLuint lamp_pro = lamp_shader.create_program();

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
        do_movement();
        // render
        // -
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(lighting_pro);
        // set lights properties
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // Decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // Low influence

        lighting_shader.setVec3("light.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
        lighting_shader.setVec3("light.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
        lighting_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        lighting_shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        lighting_shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        lighting_shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        lighting_shader.setFloat("material.shininess",  32.f);

        glm::vec3 viewPos(1.0f, 0.0f, 5.0f);
        camera.setCameraLookAt(viewPos);
        lighting_shader.setVec3("viewPos", -2.0f, -1.0f, 5.0f);
        lighting_shader.setVec3("light.position", lightPos);

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
        glBindVertexArray(0);

        // also draw the lamp object
        glUseProgram(lamp_pro);
        lamp_shader.setMat4("projection", projection);
        lamp_shader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lamp_shader.setMat4("model", model);

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
