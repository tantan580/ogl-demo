#ifndef __texture__h
#define __texture__h
#if defined(_WIN32)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#else
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void createTexture(std::string filename,unsigned int &tex){
    std::cout<<"map path:"<<filename<<std::endl;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D,tex);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image ,create texture and generate mipmaps
    unsigned char * data = nullptr;
    int w,h,n;
#if defined(__unix__)
    GError *error = nullptr;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename.c_str(), &error);
    if (!pixbuf) {
        std::cout<<("load texture failed\n");
    }
    data = gdk_pixbuf_get_pixels(pixbuf);
    n = gdk_pixbuf_get_n_channels(pixbuf);
    w = gdk_pixbuf_get_width(pixbuf);
    h = gdk_pixbuf_get_height(pixbuf);  
    std::cout<<"texture width :"<<w<<std::endl;
    std::cout<<"texture height :"<<h<<std::endl;
#else
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load(filename.c_str(), &w, &h, &n, 0);   
#endif
    if(data){
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w, h, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);//这会为当前绑定的纹理自动生成所有需要的多级渐远纹理
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D,0);   
#if defined(__unix__)
    g_object_unref(pixbuf); 
#else
    stbi_image_free(data);  
#endif
}

void createFramebuffer(GLuint& fbo, GLuint &fbtex) {
    glGenTextures(1, &fbtex);
    glBindTexture(GL_TEXTURE_2D, fbtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1024, 1024, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbtex, 0);
    {
        GLenum status;
        status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        switch (status) {
            case GL_FRAMEBUFFER_COMPLETE:
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cerr << "Error: unsupported framebuffer format" << std::endl;
                exit(0);
            default:
                std::cerr << "Error: invalid framebuffer config" << std::endl;
                exit(0);
        }
    }
}
#endif


