#include "blur_shader2.h"
#include "common/shader.h"
#include "common/texture.h"
#include "common/camera.h"
#include "gaussian.h"

//最主要是kernel的生成，它决定着BLUR的效果如何
unsigned int VBO, cubeVAO;
unsigned int tex0,fbotex1,fbotex2;
unsigned int fbo1,fbo2;
void setData()
{
    GLfloat vdata[] = {
            -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, 1.0f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
            1.0f, -1.0f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

            -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, -1.0f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
        };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    //texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),(const GLvoid*)(5*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
//    //vertexColor
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
//            (const GLvoid*)(2*sizeof(GLfloat)));
    //create texture and fbo
    createTexture("texture2.jpg",tex0);
    //framebuffer configuration1
    glGenFramebuffers(1, &fbo1);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
    glGenTextures(1,&fbotex1);
    glBindTexture(GL_TEXTURE_2D, fbotex1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex1, 0);
    //framebuffer configuration2

    glGenFramebuffers(1, &fbo2);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    glGenTextures(1,&fbotex2);
    glBindTexture(GL_TEXTURE_2D, fbotex2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex2, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// must be odd
static GLint radius = 3;
static GLfloat kernel[41];

static void build_gaussian_blur_kernel(GLint* pradius, GLfloat* offset, GLfloat* weight)
{
    GLint radius = *pradius;
    radius += (radius + 1) % 2;
    GLint sz = (radius+2)*2-1;
    GLint N = sz-1;

    GLfloat sum = powf(2, N);
    weight[radius+1] = 1.0;
    for (int i = 1; i < radius+2; i++) {
        weight[radius-i+1] = weight[radius-i+2] * (N-i+1) / i;
    }
    sum -= (weight[radius+1] + weight[radius]) * 2.0;

    std::cerr << "N = " << N << ", sum = " << sum << std::endl;

    GLfloat bias = 1.5;
    for (int i = 0; i < radius; i++) {
        offset[i] = (GLfloat)i*bias;
        weight[i] /= sum;
    }

    *pradius = radius;
}
//1.draw a quad with a texture 绘制屏幕 ，将场景绘制到纹理中，这里直接读取一张图片作为资源
//2.fbo h  对1中生成的纹理进行垂直高斯模糊运算，并将结果存储在另一张临时纹理中
//3.fbo ver and draw to screen 对2中生成的纹理进行水平方向的高斯模糊运算，然后直接绘制到屏幕上

int main()
{
    std::vector<float> weights,offsets;
    kernel_binom_linear(offsets,weights,9,2,2);
    initGlfw();
    createWindow();
    //shader configuration
    ShaderManager shader;
    shader.create_shader(vs,GL_VERTEX_SHADER);
    shader.create_shader(fs,GL_FRAGMENT_SHADER);
    GLuint pro = shader.create_program();
    std::cout<<"shader"<<std::endl;

    ShaderManager shader_linear_filter;
    shader_linear_filter.create_shader(vs,GL_VERTEX_SHADER);
    shader_linear_filter.create_shader(linear_filter_fs,GL_FRAGMENT_SHADER);
    GLuint linear_filter_pro = shader_linear_filter.create_program();
    std::cout<<"shader_linear_filter"<<std::endl;
    //set data
    setData();
    //build_gaussian_blur_kernel(&radius, &kernel[1], &kernel[21]);
    //kernel[0] = radius;
//    GLfloat* kernels = &kernel[21];
//    GLfloat * offsets = &kernel[1];
    glUseProgram(pro);// don't forget to activate/use the shader before setting uniforms!
    glUniform1i(glGetUniformLocation(pro, "texture1"), 0);
    //linear_vert_pro
    glUseProgram(linear_filter_pro);
    auto* monitor = glfwGetPrimaryMonitor();
    auto* mode = glfwGetVideoMode(monitor);
    float resx =mode->width*0.25;
    float resy =mode->height*0.25;

    glUniform1i(glGetUniformLocation(linear_filter_pro,"image"),0);
    glUniform2f(glGetUniformLocation(linear_filter_pro, "resolution"),(GLfloat)resx, (GLfloat)resy);
    //glUniform1fv(glGetUniformLocation(linear_filter_pro, "weight"), weights.size(), &weights[0]);
    //glUniform1fv(glGetUniformLocation(linear_filter_pro, "offset"), offsets.size(), &offsets[0]);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   // glLineWidth(3.f);

    while (!glfwWindowShouldClose(gl_window))
    {
        processInput(gl_window);
        //1.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
        // {
        //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, tex0);//input-->tex0
        //     glBindVertexArray(cubeVAO);
        //     glUseProgram(pro);
        //     glDrawArrays(GL_TRIANGLES, 0, 6);

        // }
#if 1
        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);//output-->fbotex1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);//input-->tex0
        glUseProgram(linear_filter_pro);
        glUniform1i(glGetUniformLocation(linear_filter_pro,"filterMode"),0);//default is vertical filter
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //2
        glBindFramebuffer(GL_FRAMEBUFFER, fbo2);//output-->fbotex2
        glBindTexture(GL_TEXTURE_2D, fbotex1);//input-->fbotex1
        glUseProgram(linear_filter_pro);
        glUniform1i(glGetUniformLocation(linear_filter_pro,"filterMode"),1);//1 is horizontal filter
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //3 output screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//output-->screen
        glBindTexture(GL_TEXTURE_2D, fbotex2);//input-->fbotex2
        glUseProgram(pro);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glow
        //3
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);//output-->screen
//        glBindTexture(GL_TEXTURE_2D, fbotex2);//input-->fbotex2
//        glUseProgram(pro);
//        glUniform1i(glGetUniformLocation(linear_filter_pro,"filterMode"),2);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, tex0);//input-->tex0
//        glUniform1i(glGetUniformLocation(linear_filter_pro,"image2"),1);
        glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
        glfwSwapBuffers(gl_window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
