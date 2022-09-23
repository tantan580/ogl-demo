#ifndef COMMON_H
#define COMMON_H
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"

GLFWwindow* gl_window = nullptr;
// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool    keys[1024];
// mouse
bool firstMouse = true;

namespace timing
{
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //add motion
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, timing::deltaTime);
}

// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, timing::deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, timing::deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, timing::deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, timing::deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
//     camera.ProcessMouseScroll(yoffset);
// }

void initGlfw(bool opengl = true)
{
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

int createWindow(std::string caption = "ogl-demo")
{
    // glfw window creation
    // --------------------
    gl_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, caption.c_str(), NULL, NULL);
    if (gl_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(gl_window);
    //glfwSetFramebufferSizeCallback(gl_window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(gl_window, mouse_callback);
    //glfwSetScrollCallback(gl_window, scroll_callback);

    // tell GLFW to capture our mouse
   // glfwSetInputMode(gl_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    return 0;
}

#endif
