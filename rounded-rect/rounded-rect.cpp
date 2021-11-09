#include "common/window.h"
#include "common/shader.h"
#include "common/camera.h"
#include "common/filesystem.h"

//https://note.xiexuefeng.cc/post/draw-webgl-rounded-rectangle/
//https://www.cnblogs.com/constantince/p/15430286.html


unsigned int VBO, cubeVAO;
static glm::vec3 eye(0.0f, 0.5f, 4.f);

static Camera camera(eye);
int w = 800; int h = 600;  
static ShaderManager shader;

void setData()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
       // ------------------------------------------------------------------
    float vertices[] = {
        1, 1, 0,
        -1, 1, 0,
        -1, -1, 0,
        1, -1, 0,
    };
    glGenVertexArrays(1, &cubeVAO);
    //1、绑定vao
    glBindVertexArray(cubeVAO);
    //2. 把顶点数组复制到缓冲中供OpenGL使用
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute// 3. 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void renderCallback() {

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)w / (float)h, 0.1f, 1000.0f);
    //正交矩阵，用作裁剪
    //glm::mat4 projection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 100.f );

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    //eye pos
    shader.setVec3("eye", eye);

    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

int main()
{
    //1、create window
    std::string caption("rounded-rect");
    OGLWindow window;
    GLFWwindow *gl_window = window.createWindow(w, h, caption);
    //2、编译shader
    const std::string vs = "rounded-rect/circle-cut.vert";
    const std::string fs = "rounded-rect/circle-cut.frag";
    std::cout<<FileSystem::getPath(vs)<<std::endl;
    shader.loadShaderFromFile(FileSystem::getPath(vs).c_str(),GL_VERTEX_SHADER);
    shader.loadShaderFromFile(FileSystem::getPath(fs).c_str(),GL_FRAGMENT_SHADER);
    GLuint pro = shader.create_program();
    //开启透明
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST); // 关闭深度测试
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // 基于源象素alpha通道值的半透明混合函数
    
    setData();
   
    // render loop
    // 当我们渲染一个物体时要使用着色器程序
    glUseProgram(pro);

    window.render(renderCallback);
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // 使用完要解除VBO绑定
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(pro);
    glDisable(GL_BLEND);
    glfwTerminate();
    return 0;
}
