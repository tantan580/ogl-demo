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
    this->data = gdk_pixbuf_get_pixels(this->pixbuf);
    this->channels = gdk_pixbuf_get_n_channels(this->pixbuf);
    this->width = gdk_pixbuf_get_width(this->pixbuf);
    this->height = gdk_pixbuf_get_height(this->pixbuf);
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

static void createBC6Buffer(GLsizei w, GLsizei h, GLsizei d, uint8_t *buffer)
{
    uint32_t slice = w * h;
    uint32_t block_num = w * h * d / 16;
    for (uint32_t i = 0; i < block_num; i++)
    {
        buffer[i * 16 + 0] = 0;
        buffer[i * 16 + 1] = 0;
        buffer[i * 16 + 2] = 0;
        buffer[i * 16 + 3] = 0;
        buffer[i * 16 + 4] = 0;
        buffer[i * 16 + 5] = 0;
        buffer[i * 16 + 6] = 0;
        buffer[i * 16 + 7] = 0;
        buffer[i * 16 + 8] = 0;
        buffer[i * 16 + 9] = 0;
        buffer[i * 16 + 10] = 0;
        buffer[i * 16 + 11] = 0;
        buffer[i * 16 + 12] = 0;
        buffer[i * 16 + 13] = 0;
        buffer[i * 16 + 14] = 0;
        buffer[i * 16 + 15] = i;
    }
    
    // for (uint32_t z = 0; z < d; z++)
    // {
    //     for (uint32_t y = 0; y < h / 4; y++)
    //     {
    //         for (uint32_t x = 0; x < w / 4; x++)
    //         {
    //             uint32_t index = z * slice + y * w + x;
    //             buffer[index] = (slice * z + y * w + x) / 16;
    //         }
    //     }
    // }
    for (uint32_t z = 0; z < d; z++)
    {
        printf("z: %d \n",z);
        for (uint32_t y = 0; y < h; y++)
        {
            for (uint32_t x = 0; x < w; x++)
            {
                uint32_t index = z * slice + y * w + x;
                printf("%d ", buffer[index]);
                if (x % 16 == 15)
                {
                    printf(", ");
                }
            }
            printf("\n");
        }
    }   
}

uint32_t color[][4] = {
    {0x0000ffff, 0x00000000, 0xf800f800, 0x00000000}, // red   0
    {0x0000ffff, 0x00000000, 0xffffffff, 0x00000000}, // white 1
    {0x0000ffff, 0x00000000, 0x07e007e0, 0x00000000}, // green 2
    {0x0000ffff, 0x00000000, 0x000f0010, 0xaebaabba}, // blue  3
    {0x0000ffff, 0x00000000, 0x7bef8410, 0xaeaabaaa}, // grey  4
    {0x0000ffff, 0x00000000, 0xffe0ffe0, 0x00000000}, // yellow 5
    {0x0000ffff, 0x00000000, 0xcfefcfef, 0x00000000}, // light green  6
    {0x0000ffff, 0x00000000, 0xdddddddd, 0x00000000}, // pink 7
    {0x0000ffff, 0x00000000, 0x55555555, 0x00000000}, // dark red 8
    {0x0000ffff, 0x00000000, 0x57775777, 0x00000000}, // 9 cyan??
    {0x0000ffff, 0x00000000, 0xefffefff, 0x00000000}, // 10 white alike
    {0x0000ffff, 0x00000000, 0x99999999, 0x00000000}, // 11 purple
};

static void createBC6Buffer2(GLsizei w, GLsizei h, GLsizei d, uint8_t *buffer)
{
    // 每个slice 有多少个128 bit
    uint32_t block_num = w * h / 16; //
    uint32_t slice = w * h; // bytes per slice 
    for (size_t i = 0; i < d; i++)
    {
        uint32_t color1, color2, color3, color4;

        color1 = color[i][0];
        color2 = color[i][1];
        color3 = color[i][2];
        color4 = color[i][3]; 
 
        for (size_t j = 0; j < block_num; j++)
        {
            uint32_t index = j * 16 + i * slice;
            
            buffer[index + 0] = (uint8_t)(color1);
            buffer[index + 1] = (uint8_t)(color1 >> 8);
            buffer[index + 2] = (uint8_t)(color1 >> 16);
            buffer[index + 3] = (uint8_t)(color1 >> 24);

            buffer[index + 4] = (uint8_t)(color2);
            buffer[index + 5] = (uint8_t)(color2 >> 8);
            buffer[index + 6] = (uint8_t)(color2 >> 16);
            buffer[index + 7] = (uint8_t)(color2 >> 24);

            buffer[index + 8] = (uint8_t)(color3);
            buffer[index + 9] = (uint8_t)(color3 >> 8);
            buffer[index + 10] = (uint8_t)(color3 >> 16);
            buffer[index + 11] = (uint8_t)(color3 >> 24);

            buffer[index + 12] = (uint8_t)(color4);
            buffer[index + 13] = (uint8_t)(color4 >> 8);
            buffer[index + 14] = (uint8_t)(color4 >> 16);
            buffer[index + 15] = (uint8_t)(color4 >> 24);
        }
    }

    for (uint32_t z = 0; z < d; z++)
    {
        printf("z: %d \n",z);
        for (uint32_t y = 0; y < h; y++)
        {
            for (uint32_t x = 0; x < w; x++)
            {
                uint32_t index = z * slice + y * w + x;
                printf("%d ", buffer[index]);
                if (x % 16 == 15)
                {
                    printf(", ");
                }
            }
            printf("\n");
        }
    }   
}

void Texture::createCompressedTexture3D(std::string filename, unsigned int &tex)
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_3D, tex);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    gli::gl GL(gli::gl::PROFILE_GL32);
    gli::texture3d texture(gli::load_dds(filename.c_str()));
    gli::gl::format const Format = GL.translate(texture.format(), texture.swizzles());
    // GLenum intermaformat = gli::gl::INTERNAL_RGBA_DXT5;//gli::gl::INTERNAL_RGB_BP_UNSIGNED_FLOAT;//0x8E8F; 0x79 / UBC5
    if (texture.data() != nullptr)
    {
        // GLsizei w = 32;
        // GLsizei h = 32;
        // GLsizei d = 3;
        // GLsizei const imgsize = w * h * d;
        // void *buf = malloc(imgsize);
        // memset(buf, 0, imgsize);

        // //createBC6Buffer(w, h, d, (uint8_t*)buf);
        // createBC6Buffer2(w, h, d, (uint8_t*)buf);
        // glCompressedTexImage3D(GL_TEXTURE_3D, 0, intermaformat, w, h, d, 0,
        //                         imgsize, buf);
        GLsizei w = static_cast<GLsizei>(texture.extent().x);
        GLsizei h = static_cast<GLsizei>(texture.extent().y);
        GLsizei d = static_cast<GLsizei>(texture.extent().z);
        GLsizei const imgsize = w * h * d;
        glCompressedTexImage3D(GL_TEXTURE_3D, 0, Format.Internal, w, h, d, 0,
                               imgsize, texture.data());
    }
    // else
    // {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
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