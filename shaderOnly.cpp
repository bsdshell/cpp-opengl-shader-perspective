// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

#include <OpenGL/gl3.h> //(My code compiles without this line)
#define GLFW_INCLUDE_GLCOREARB
#include "GLFW/glfw3.h"

// #include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>


#ifndef AronCLibNew
#define AronCLibNew
#include "AronCLibNew.h"
#endif

#ifndef AronOpenGLLib
#define AronOpenGLLib
#include "AronOpenGLLib.h"
#endif

#include "AronLib.h"

using namespace std;
using namespace MatrixVector;

// DATE: Friday, 21 April 2023 12:12 PDT
// KEY: Draw one rotated x-axis triangle only
// COMPILE: opengl_compile.sh cpp-opengl-shader-rotate-triangle.cpp
//

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/*
Ref: https://learnopengl.com/Getting-started/Shaders
Data: 18-08-2020
KEY: opengl shader, vertex shader fragment shader, shader tutorial, shader uniform
Compile: opengl_compile.sh
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;


std::string vertexShaderStr = R"(
#version 330 core
    layout (location = 0) in vec2 aPos;
    // layout (location = 1) in vec3 aColor;
    // out vec3 ourColor;
    
    void main()
    {
        // gl_Position = vec4(aPos, 1.0);
        // ourColor = aPos; 

        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
)";

std::string fragStr = R"(
#version 330 core

out vec4 FragColor;

// uniform vec2 resolution; // Screen resolution
// uniform vec2 center;     // Center of the circle
// uniform float radius;    // Radius of the circle

void main()
{
    float radius = 0.4;
    vec2 center = vec2(0, 0);
    vec2 res = vec2(1000, 1000);
    // Calculate the normalized device coordinates of the fragment
    // vec2 ndc = (2.0 * gl_FragCoord.xy - resolution) / min(resolution.x, resolution.y);
    vec2 ndc = (2.0 * gl_FragCoord.xy - res) / min(res.x, res.y);
    
    // Calculate the distance from the fragment to the center of the circle
    float distanceToCenter = length(ndc - center / res);
    
    // If the distance is less than the radius, the fragment is inside the circle
    if (distanceToCenter < radius) {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Red color
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black color
        // discard; // Discard fragments outside the circle
    }
}
)";

const char *vertexShaderSource = vertexShaderStr.c_str();

const char *fragmentShaderSource = fragStr.c_str(); 

int main(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    /*
       if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
       {
       std::cout << "Failed to initialize GLAD" << std::endl;
       return -1;
       }
       */

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader); // vertexShader - vertex shader code
    glAttachShader(shaderProgram, fragmentShader); // fragmentShader - fragment shader code
    glLinkProgram(shaderProgram);                 // link shader
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        exit(1);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    while (!glfwWindowShouldClose(window)) {
        
        processInput(window);
        /*
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// KEY: GL_DEPTH_BUFFER_BIT, depth test
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    */	
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// URL: https://www.glfw.org/docs/3.3/group__keys.html
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
	glfwSetWindowShouldClose(window, true);
  }if (key == GLFW_KEY_A && action == GLFW_PRESS){
	log("pres A =>");
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


