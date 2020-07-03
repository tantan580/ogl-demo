#include "common.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "blur_common.h"

int void main()
{
    auto* monitor = glfwGetPrimaryMonitor();
    auto* mode = glfwGetVideoMode(monitor);
    glViewport(0, 0, ctx.width, ctx.height);
}
