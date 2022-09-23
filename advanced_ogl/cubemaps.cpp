#define GLEW_STATIC
#include "common/common.h"
#include "common/shader.h"
#include "common/texture.h"
#include "common/filesystem.h"
#include "common/window.h"

// cube VAO
unsigned int cubeVAO, cubeVBO;
// skybox VAO
unsigned int skyboxVAO, skyboxVBO;
// textures
unsigned int cubeTexture;
unsigned int cubemapTexture;

const char *cube_vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texcoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const GLchar *cube_fs = R"(
    #version 330 core
    in vec2 texcoords;
    out vec4 color;

    uniform sampler2D  texture1;
    void main()
    {
        color = texture(texture1, texcoords);
    }
)";

//注意，顶点着色器有意思的地方在于我们把输入的位置向量作为输出给片段着色器的纹理坐标。片段着色器就会把它们作为输入去采样samplerCube：
const char* sky_vs = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    out vec3 texcoords;

    uniform mat4 projection;
    uniform mat4 view;
    void main()
    {
        gl_Position = projection * view * vec4(position, 1.0);
        texcoords = position;
    }
)";

const GLchar *sky_fs = R"(
    #version 330 core
    in vec3 texcoords;
    out vec4 color;

    uniform samplerCube skybox;
    void main()
    {
        color = texture(skybox, texcoords);
    }
)";

void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // cube
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // skybox
    glGenVertexArrays(1,&skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // load textures
    Texture texture;
    texture.createTexture2D(FileSystem::getPath("resources/textures/container.jpg"), cubeTexture);
    
    std::vector<std::string> faces
    {
        FileSystem::getPath("resources/textures/skybox/right.jpg"),
        FileSystem::getPath("resources/textures/skybox/left.jpg"),
        FileSystem::getPath("resources/textures/skybox/top.jpg"),
        FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
        FileSystem::getPath("resources/textures/skybox/front.jpg"),
        FileSystem::getPath("resources/textures/skybox/back.jpg")
    };

   texture.createCubeMapTexture(faces, cubemapTexture);

}

int main()
{
    OGLWindow window;
    GLFWwindow *gl_window = window.createWindow(800, 600, "cube-map");
    
    setData();

    // shader
    ShaderManager skyshader;
    skyshader.create_shader(sky_vs, GL_VERTEX_SHADER);
    skyshader.create_shader(sky_fs, GL_FRAGMENT_SHADER);
    GLuint skyprogram = skyshader.create_program();
    skyshader.use();
    skyshader.setInt("skybox", 0);

    ShaderManager cubeshader;
    cubeshader.create_shader(cube_vs, GL_VERTEX_SHADER);
    cubeshader.create_shader(cube_fs, GL_FRAGMENT_SHADER);
    GLuint cubeprogram = cubeshader.create_program();
    cubeshader.use();
    cubeshader.setInt("texture1", 0);

    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // configure global opengl state
    while (!glfwWindowShouldClose(gl_window))
    {
        // Check and call events
        processInput(gl_window);
        // Clear buffers
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        // Set MVP
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
        //camera.setCameraLookAt(glm::vec3(0, 0, 2), glm::vec3(0, -1, 0));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Draw cubes
        cubeshader.use();
        cubeshader.setMat4("view", view);
        cubeshader.setMat4("projection", projection);
        cubeshader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        // 采样器的序号要与激活的纹理序号对应
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyshader.use();
        camera.setCameraLookAt(glm::vec3(0, 0, -3), glm::vec3(0, -1, 0));
        view = camera.GetViewMatrix();
        skyshader.setMat4("view", view);
        skyshader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glfwSwapBuffers(gl_window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glfwTerminate();
    return 0;
}