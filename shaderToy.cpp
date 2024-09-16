// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <glm/glm.hpp>

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

#ifndef GLM 
#define GLM 
// #include <glm/glm.hpp>
#endif

#include "AronLib.h"

const char *vertexShaderSource = 
R"(#version 330 core
layout (location = 0) in vec2 position;            
layout (location = 1) in vec2 inTexCoord;

out vec2 texCoord;
void main(){
    texCoord = inTexCoord;
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
})";

const char *fragmentShaderSource = 
R"(#version 330 core
in vec2 texCoord;
uniform vec2 iResolution;
uniform float iTime;
out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;

float myCircle(vec2 v0, float r){
   vec2 cen = vec2(0, 0);
   vec2 dv = v0 - cen;
   float d = sqrt(dot(dv, dv));
   return d - r;
}

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;
    uv = uv * 2.0 - 1.0;//transform from [0,1] to [-1,1]
    uv.x *= iResolution.x / iResolution.y; //aspect fix
    
    /*                                       //
    vec3 cameraPos = vec3(sin(iTime * 0.47) * 4.0,sin(iTime * 0.7)*8.0+6.0,-25.0);//camera pos animation
    vec3 cameraFocusPoint = vec3(0,0.0 + sin(iTime),0);//camera look target point animation
    vec3 cameraDir = normalize(cameraFocusPoint - cameraPos);
    
    //TEMPCODE: fov & all ray init dir, it is wrong!!!!
    //----------------------------------------------------
    float fovTempMul = 0.2 + sin(iTime * 0.4) * 0.05;//fov animation
    vec3 rayDir = normalize(cameraDir + vec3(uv,0) * fovTempMul);
    //----------------------------------------------------

    vec4 finalColor = vec4(0);
    for(int i = 1; i <= AVERAGECOUNT; i++)
    {
        finalColor+= calculateFinalColor(cameraPos,rayDir, float(i));
    }
    finalColor = finalColor/float(AVERAGECOUNT);//brute force AA & denoise
    finalColor.rgb = pow(finalColor.rgb,vec3(1.0/2.2));//gamma correction
    
    //only for CineShader, to show depth
    float z = finalColor.w; //z is linear world space distance from camera to surface
    float cineShaderZ; //expect 0~1
    cineShaderZ = pow(clamp(1.0 - max(0.0,z-21.0) * (1.0/6.0),0.0,1.0),2.0);
    
    //result
    // fragColor = vec4(finalColor.rgb,cineShaderZ);
     */    
    if(myCircle(uv, sin(iTime)) < 0.001){
        fragColor = vec4(0, 1, 0, 1.0);
    }else{
        fragColor = vec4(0.5, 0.5, 0.5, 1.0);
    }
})";

int main()
{
    int width = 800;
    int height = 600;

    glm::vec2 screen(width, height);
    // float resolution[2] = {1000, 1000};

    float deltaTime = 0.0f; 
    float lastFrame = 0.0f; 

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "OpenglContext", nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to create window" << std::endl;
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    /*
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "failed to initialize glad with processes " << std::endl;
        exit(-1);
    }
    */

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    int samples = 4;
    float quadVerts[] = {
        -1.0, -1.0,     0.0, 0.0,
        -1.0, 1.0,      0.0, 1.0,
        1.0, -1.0,      1.0, 0.0,

        1.0, -1.0,      1.0, 0.0,
        -1.0, 1.0,      0.0, 1.0,
        1.0, 1.0,       1.0, 1.0
    };

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); 

    GLuint texColor;
    glGenTextures(1, &texColor);
    glBindTexture(GL_TEXTURE_2D, texColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    // glUniform2fv(glGetUniformLocation(shaderProgram, "iResolution"), 1, &resolution[0]);
    glUniform2fv(glGetUniformLocation(shaderProgram, "iResolution"), 1, &screen[0]);


    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(shaderProgram);
        // glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), (int)currentFrame % 20); 
        // glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), currentFrame); 
        glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), (int)currentFrame % 60); 
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwTerminate();
    // cleanup
}
