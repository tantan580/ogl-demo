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
/**片段的位置(Position),我们会在世界空间中进行所有的光照计算，因此我们需要一个在世界空间中的顶点位置
我们可以通过把顶点位置属性乘以模型矩阵(Model Matrix,只用模型矩阵不需要用观察和投影矩阵)来把它变换到世界空间坐标
在环境光照部分，光照表现没问题，这是因为我们没有对物体本身执行任何缩放操作，因而不是非得使用正规矩阵不可，
用模型矩阵乘以法线也没错。可是，如果你进行了不等比缩放，使用正规矩阵去乘以法向量就是必不可少的了。
**/
//attention
/*
 * 对于着色器来说，逆矩阵也是一种开销比较大的操作，因此，无论何时，在着色器中只要可能就应该尽量避免逆操作，因为它们必须为你场景中的每个顶点进行这样的处理。
 * 以学习的目的这样做很好，但是对于一个对于效率有要求的应用来说，在绘制之前，你最好用CPU计算出正规矩阵，然后通过uniform把值传递给着色器(和模型矩阵一样)。
*/
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
                               int shininess = 32;//镜面光的散射因子
                               float spec = pow(max(dot(reflectDir, viewDir), 0), shininess);
                               vec3 specular = specularStrength * spec * lightColor;

                               vec3 result = (ambient + diffuse + specular) * objectColor;
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
        lighting_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lighting_shader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        glm::vec3 viewPos(1.0f, 0.0f, 5.0f);
        camera.setCameraLookAt(viewPos);
        lighting_shader.setVec3("viewPos", -2.0f, -1.0f, 5.0f);
        lighting_shader.setVec3("lightPos", lightPos);

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
