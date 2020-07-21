/*
 * main.cpp
 *
 *  Created on: Sep 6, 2010
 *      Author: aqnuep
 */

#include <iostream>
#include <fstream>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include "tga.h"

using namespace std;

GLuint loadShaderFromFile(const char* filename, GLenum shaderType) {

	ifstream file(filename);
	if (!file) {
		cerr << "Unable to open file: " << filename << endl;
		return 0;
	}

	char line[256];
	string source;

	while (file) {
		file.getline(line, 256);
		source += line;
		source += '\n';
	}

    if (!file.eof()) {
    	cerr << "Error reading the file: " << filename << endl;
    	return 0;
    }

    GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar**)&source, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		cerr << "Failed to compile shader: " << filename << endl;
		GLchar log[10000];
		glGetShaderInfoLog(shader, 10000, NULL, log);
		cerr << log << endl;
		exit(1);
	}

	return shader;
}

GLuint createProgramFromShaders(GLuint vert, GLuint frag) {
	GLint status;
	GLuint prog = glCreateProgram();

	glAttachShader(prog, vert);
	glAttachShader(prog, frag);

	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		cerr << "Failed to link shaders: " << endl;
		GLchar log[10000];
		glGetProgramInfoLog(prog, 10000, NULL, log);
		cerr << log << endl;
	}

	return prog;
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
				cerr << "Error: unsupported framebuffer format" << endl;
				exit(0);
			default:
				cerr << "Error: invalid framebuffer config" << endl;
				exit(0);
		}
	}
}

int main() {

	sf::ContextSettings Settings(0, 0, 0, 3, 0);
	sf::Window App(sf::VideoMode(1024, 1024, 32), "Gaussian filter demo - SPACE = cycle through modes: no filter, discrete sampling, linear sampling & 1-9 = filter steps", sf::Style::Close, Settings);
	sf::Clock Clock;
	const sf::Input& Input = App.GetInput();

	GLenum glewError;
	if ((glewError = glewInit()) != GLEW_OK) {
		cerr << "Error: " << glewGetErrorString(glewError) << endl;
		return 0;
	}

	if (!GLEW_VERSION_3_0) {
		cerr << "Error: OpenGL 3.0 is required" << endl;
		return 0;
	}

	// load shaders
	GLuint vert, frag, fragDiscreteV, fragDiscreteH, fragLinearV, fragLinearH, prog[5];
	vert = loadShaderFromFile("passthrough.vs", GL_VERTEX_SHADER);
	frag = loadShaderFromFile("passthrough.fs", GL_FRAGMENT_SHADER);
	fragDiscreteV = loadShaderFromFile("discrete_vert.fs", GL_FRAGMENT_SHADER);
	fragLinearV = loadShaderFromFile("linear_vert.fs", GL_FRAGMENT_SHADER);
	fragDiscreteH = loadShaderFromFile("discrete_horiz.fs", GL_FRAGMENT_SHADER);
	fragLinearH = loadShaderFromFile("linear_horiz.fs", GL_FRAGMENT_SHADER);
	prog[0] = createProgramFromShaders(vert, frag);
	prog[1] = createProgramFromShaders(vert, fragDiscreteV);
	prog[2] = createProgramFromShaders(vert, fragLinearV);
	prog[3] = createProgramFromShaders(vert, fragDiscreteH);
	prog[4] = createProgramFromShaders(vert, fragLinearH);

	// create full-screen quad mesh
	GLuint vbo;
	glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLfloat quad[] = { 1.f, 1.f, -1.f, 1.f, -1.f,-1.f,
					  -1.f,-1.f,  1.f,-1.f,  1.f, 1.f };
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*6, quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2, (void*)0);

	// create temporary framebuffer
	GLuint fbo[2], fbtex[2];
	createFramebuffer(fbo[0], fbtex[0]);
	createFramebuffer(fbo[1], fbtex[1]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// load texture
	TGAImage* image = new TGAImage("image.tga");
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image->width, image->height, 0, GL_BGR, GL_UNSIGNED_BYTE, image->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	delete image;

	int mode = 0, iterations=1;
	float lastFPS = Clock.GetElapsedTime();

	while (App.IsOpened())
	{
	    sf::Event Event;
	    while (App.GetEvent(Event))
	    {
			if (Event.Type == sf::Event::KeyPressed) {
				if ( Event.Key.Code == sf::Key::Space ) {
					mode = ++mode % 3;
					switch (mode) {
						case 0: cout << "No Gaussian filter" << endl; break;
						case 1: cout << "Gaussian filter with discrete sampling" << endl; break;
						case 2: cout << "Gaussian filter with linear sampling" << endl; break;
					}
				}
			}
	    	if (Event.Type == sf::Event::Closed)
		    	break;
	    }
    	if (Event.Type == sf::Event::Closed)
	    	break;
    	if (Input.IsKeyDown(sf::Key::Escape)) break;

    	if (Input.IsKeyDown(sf::Key::Num1)) iterations = 1;
    	if (Input.IsKeyDown(sf::Key::Num2)) iterations = 2;
    	if (Input.IsKeyDown(sf::Key::Num3)) iterations = 3;
    	if (Input.IsKeyDown(sf::Key::Num4)) iterations = 4;
    	if (Input.IsKeyDown(sf::Key::Num5)) iterations = 5;
    	if (Input.IsKeyDown(sf::Key::Num6)) iterations = 6;
    	if (Input.IsKeyDown(sf::Key::Num7)) iterations = 7;
    	if (Input.IsKeyDown(sf::Key::Num8)) iterations = 8;
    	if (Input.IsKeyDown(sf::Key::Num9)) iterations = 9;

		App.SetActive();

		glBindTexture(GL_TEXTURE_2D, texture);

		if ( mode != 0 ) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[0]);

			glUseProgram(prog[mode+2]);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			for (int i=1; i<iterations; i++) {
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[1]);
				glBindTexture(GL_TEXTURE_2D, fbtex[0]);

				glUseProgram(prog[mode]);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[0]);
				glBindTexture(GL_TEXTURE_2D, fbtex[1]);

				glUseProgram(prog[mode+2]);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glBindTexture(GL_TEXTURE_2D, fbtex[0]);
		}

		glUseProgram(prog[mode]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	    if (Clock.GetElapsedTime() - lastFPS > 1.0) {
	    	cout << "FPS: " << 1.0 / App.GetFrameTime() << endl;
	    	cout << endl;
	    	lastFPS = Clock.GetElapsedTime();
	    }

	    App.Display();
	}

	glDeleteTextures(1, &texture);

	for (int i=0; i<5; i++)
		glDeleteProgram(prog[i]);

	glDeleteShader(vert);
	glDeleteShader(frag);
	glDeleteShader(fragDiscreteV);
	glDeleteShader(fragLinearV);
	glDeleteShader(fragDiscreteH);
	glDeleteShader(fragLinearH);

	glDeleteTextures(2, fbtex);
	glDeleteFramebuffers(2, fbo);

	return 0;
}
