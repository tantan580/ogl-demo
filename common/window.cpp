#include "common/window.h"
#include "common/camera.h"
extern Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout<<"framebuffer_size_callback"<<std::endl;
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(yoffset);
}

OGLWindow::OGLWindow()
{
    std::cout<<"OGLWindow"<<std::endl;
    init();
}

GLFWwindow* OGLWindow::createWindow(int w, int h, std::string caption)
{
    // glfw window creation
    // --------------------
    gl_window = glfwCreateWindow(w, h, caption.c_str(), NULL, NULL);
    if (gl_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(gl_window);
    glfwSetFramebufferSizeCallback(gl_window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(gl_window, mouse_callback);
   glfwSetScrollCallback(gl_window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(gl_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return gl_window;
}

void OGLWindow::setBackColor(float r, float g, float b, float a)
{
    gl_backColor = glm::vec4(r, g, b, a);
}

void OGLWindow::render(const std::function<void()>& f)
{
    uniformCallback = f;
    while (!glfwWindowShouldClose(gl_window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        uniformCallback();
        glfwSwapBuffers(gl_window);
        glfwPollEvents();       
    }   
}

void OGLWindow::init(bool opengl)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    if (opengl == true)
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    else
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

}
