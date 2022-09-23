#ifndef __texture__h
#define __texture__h
#include <glad/glad.h>
#if defined(_WIN32)
#else
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#endif
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class Image
{
public:
    Image();
    void load(std::string file);
    void free();
    
#if defined(__unix__)
    GdkPixbuf *pixbuf{0};
#endif
    unsigned char *data{0};
    int width;
    int height;
    int channels;
};

class Texture
{
public:
    Texture();
    ~Texture();
    void createTexture2D(std::string filename, unsigned int &tex);
    void createCubeMapTexture(std::vector<std::string> faces, unsigned int &tex);
    void createFramebufferTexture(GLuint &fbo, GLuint &fbtex);
};

#endif
