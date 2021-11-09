#ifndef ShaderManager_h
#define ShaderManager_h
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <fstream>

class ShaderManager{
public:
    ShaderManager(){}
    ~ShaderManager() {

        glDeleteProgram(shaderProgram);
    }
    enum Type{
        Compile,
        Link,
    };
    static ShaderManager * instance(){
        static ShaderManager shader;
        return &shader;
    }
    GLuint create_shader(const char * sc,GLenum shaderType){
        // build and compile our shader program
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &sc, nullptr);
        glCompileShader(shader);
        // check for shader compile errors
        if(checkCompileErrors(shader,Type::Compile)){
            if(shaderType == GL_VERTEX_SHADER)
                vertexShader = shader;
            else if(shaderType == GL_FRAGMENT_SHADER)
                fragmentShader = shader;
            return shader;
        }
        return -1;
    }
    GLuint create_program(){
        shaderProgram = glCreateProgram();
        std::cout<<"vs:"<<vertexShader<<std::endl;
        std::cout<<"frags:"<<fragmentShader<<std::endl;
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        if(checkCompileErrors(shaderProgram,Type::Link)){
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return shaderProgram;
        }
        return -1;
   }
    GLuint loadShaderFromFile(const char* filename, GLenum shaderType) {
       std::ifstream file(filename);
       if (!file) {
           std::cerr << "Unable to open file: " << filename << std::endl;
           return 0;
       }

       char line[256];
       std::string source;

       while (file) {
           file.getline(line, 256);
           source += line;
           source += '\n';
       }

       if (!file.eof()) {
           std::cerr << "Error reading the file: " << filename << std::endl;
           return 0;
       }
       return create_shader(source.c_str(),shaderType);
   }

    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(shaderProgram);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
private:
     bool checkCompileErrors(GLuint shader, Type type){
        GLint success;
        GLchar infoLog[1024];
        if(type == Type::Compile){
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success){
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                return false;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                return false;
            }
        }
        return true;
    }
private:
    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;
};


#endif
