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
unsigned int bcntexture;

const char *vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoords;

out vec3 texcoords;

void main()
{
    texcoords = aTexCoords;    
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
)";

const GLchar *fs = R"(
    #version 330 core
    in vec3 texcoords;
    out vec4 color;

    uniform sampler3D tex;
    void main()
    {
        color = texture3D(tex, vec3(texcoords));
    }
)";

void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    
    float cubeVertices[] = {
        // positions          // texture Coords
         -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,

         -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.5f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.5f,

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    };

    // cube
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    // load textures
    Texture texture;
    const std::string bcndds = "resources/bcn/BC6H_UF16_3D.dds";
    texture.createCompressedTexture3D(FileSystem::getPath(bcndds.c_str()), bcntexture);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("error: param is less! \n");
        return 0;
    }
    printf("param is %s\n", argv[1]);
    int p = atoi(argv[1]);
    OGLWindow window;
    GLFWwindow *gl_window = window.createWindow(800, 600, "bcn3d");
    
    setData();

    ShaderManager cubeshader;
    cubeshader.create_shader(vs, GL_VERTEX_SHADER);
    cubeshader.create_shader(fs, GL_FRAGMENT_SHADER);
    GLuint cubeprogram = cubeshader.create_program();
    cubeshader.use();
    cubeshader.setInt("tex", 0);

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
        camera.setCameraLookAt(glm::vec3(0, 0, 2), glm::vec3(0, -1, 0));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Draw cubes
        cubeshader.use();
        // cubeshader.setMat4("view", view);
        // cubeshader.setMat4("projection", projection);
        // cubeshader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        // 采样器的序号要与激活的纹理序号对应
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, bcntexture);
        // 0, 6 -- 0slice; 6,6 -- 1slice; 12, 6----2slice.
        glDrawArrays(GL_TRIANGLES, 6 *p, 6);
        glBindVertexArray(0);

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