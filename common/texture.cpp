#include "common/texture.h"

#if defined(_WIN32)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
Image::Image()
{

}

void Image::load(std::string filename)
{
    std::cout << "texture path:" << filename << std::endl;
    // load image ,create texture and generate mipmaps
#if defined(__unix__)
    GError *error = nullptr;
    this->pixbuf = gdk_pixbuf_new_from_file(filename.c_str(), &error);
    if ( this->pixbuf == 0)
    {
        std::cout << ("load texture failed\n");
    }
    this->data = gdk_pixbuf_get_pixels(texture.pixbuf);
    this->channels = gdk_pixbuf_get_n_channels(texture.pixbuf);
    this->width = gdk_pixbuf_get_width(texture.pixbuf);
    this->height = gdk_pixbuf_get_height(texture.pixbuf);
#else
    stbi_set_flip_vertically_on_load(
        true); // tell stb_image.h to flip loaded texture's on the y-axis.
    this->data = stbi_load(filename.c_str(), &this->width, &this->height,
                             &this->channels, 0);
#endif
    std::cout << "texture width :" << this->width << std::endl;
    std::cout << "texture height :" << this->height << std::endl;
}

void Image::free()
{
#if defined(__unix__)
    g_object_unref(this->pixbuf);
#else
    stbi_image_free(this->data);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////
Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::createTexture2D(std::string filename, unsigned int& tex)
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Image image;
    image.load(filename);

    if (image.data != nullptr)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, image.data);
        glGenerateMipmap(
            GL_TEXTURE_2D); //这会为当前绑定的纹理自动生成所有需要的多级渐远纹理
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    image.free();
}

void Texture::createCubeMapTexture(std::vector<std::string> faces, unsigned int &tex)
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    for (size_t i = 0; i < faces.size(); ++i)
    {
        Image image;
        image.load(faces[i]);

        if (image.data != nullptr)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.width,
                         image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
            image.free();
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i]
                      << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Texture::createFramebufferTexture(GLuint &fbo, GLuint &fbtex)
{
    glGenTextures(1, &fbtex);
    glBindTexture(GL_TEXTURE_2D, fbtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1024, 1024, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fbtex, 0);
    {
        GLenum status;
        status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        switch (status)
        {
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