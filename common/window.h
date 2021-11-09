#ifndef __window__h__
#define __window__h__

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class OGLWindow
{
public:
    OGLWindow();
    GLFWwindow* createWindow(int w, int h, std::string caption);
    void setBackColor(float r, float g, float b, float a);
    void render(const std::function<void()>& f);
private:
    void init();
private:
    GLFWwindow *gl_window {0};
    glm::vec4 gl_backColor;
    std::function<void()> uniformCallback;
};

#endif
