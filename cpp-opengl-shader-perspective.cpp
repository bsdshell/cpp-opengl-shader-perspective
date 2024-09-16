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
// Fri  1 Mar 01:25:11 2024 
// BUG: fixed There are duplicated include lib
//
class Frustum;

void shaderSetMatrix(GLuint shaderHandle, string uniformVarStr, float arr[16]);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void frustum(float left, float right, float bottom, float top, float near, float far, float mat[16]);

void createFrustumMat(Frustum& fm);

class Cube{
public:
  float r;
  float* arr;
  int sizeOf;
public:
  Cube(float r_){
	r = r_;
  }
  void init(){
	float box[] = {
	  -r, -r, -r,  0.5f, 0.0f, 0.0f, // 4
      r,  -r, -r,  0.5f, 0.3f, 0.0f, // 3
      -r, -r, r,   0.2f, 0.0f, 0.3f, // 7
      r,  -r, r,   0.5f, 0.0f, 0.4f, // 8

	  r,  r,  r,  0.5f, 0.0f, 0.0f, // 5
	  r,  -r, -r, 0.4f, 0.5f, 0.0f, // 3
	  r,  r,  -r, 0.5f, 0.0f, 0.0f, // 1
	  -r, -r, -r, 0.5f, 0.2f, 0.1f, // 4

	  -r, r,  -r, 0.5f, 0.4f, 0.0f, // 2
	  -r, -r, r,  0.5f, 0.0f, 0.3f, // 7
	  -r, r,  r,  0.5f, 0.2f, 0.0f, // 6
	  r,  r,  r,  0.5f, 0.4f, 0.0f, // 5

	  -r, r, -r,  0.5f, 0.3f, 0.0f, // 2
	  r,  r, -r,  0.5f, 0.5f, 0.0f  // 1
    };
	sizeOf = sizeof(box);
	arr = (float*) new float[sizeof(box)];
	for(int i = 0; i < sizeof(box); i++){
	  arr[i] = box[i];
	}
  }
  int getSize(){
	return sizeOf;
  }
  void setR(float r_){
	r = r_;
  }
  ~Cube(){
	delete[] arr;
  }
};

class Frustum{
public: 
  float left;
  float right;
  float top;
  float bottom;
  float near;
  float far;
  float frustumMat[16];
public:
  Frustum(){
	left = -1;
	right = 1;
	top = 1;
	bottom = -1;
	near = 1;
	far = 3;
	for(int i = 0; i < 16; i++){
	  frustumMat[i] = 0;
	}
  }
  Frustum(float _left, float _right, float _top, float _bottom, float _near, float _far){
	left = _left;
	right = _right;
	top = _top;
	bottom = _bottom;
	near = _near;
	far = _far;
	for(int i = 0; i < 16; i++){
	  frustumMat[i] = 0;
	}
  }
  string toStr(){
	char arr[200];
	snprintf(arr, sizeof(arr), "left=%f right=%f top=%f bottom=%f near=%f far=%f\n", left, right, top, bottom, near, far);
	string s = charArrToString(arr);
	return s;
  }
};


void createFrustumMat(Frustum& fm){
  // void frustum(float left, float right, float bottom, float top, float near, float far, float mat[16]){
  frustum(fm.left, fm.right, fm.bottom, fm.top, fm.near, fm.far, fm.frustumMat);
}

class Draw{
    public:
        float mat[16];
        int numFloat;
        int numVertex;
        int numOfComponentAttri;
        float *pt;
        unsigned int VBO;
        unsigned int VAO;
    public: 
        Draw(int numFloat, float* arr, int numVertex, int numOfComponentAttri){
            this -> pt = (float*)malloc(numFloat*sizeof(float));
            this -> numFloat = numFloat;
			this -> numVertex = numVertex;
			this -> numOfComponentAttri = numOfComponentAttri;
            for(int i = 0; i < this -> numFloat; i++){
                pt[i] = arr[i];
            }
        }

        void setMatrix(GLuint shaderHandle, string uniformVarStr, float arr[16]){
            shaderSetMatrix(shaderHandle, uniformVarStr, arr); 
        }
        unsigned int setupVBOVAO(){
            glGenVertexArrays(1, &VAO);
            //                |
            //                + -> the number of vertex array object names to generate
            //
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numFloat, pt, GL_STATIC_DRAW);

            //                            + -> specify number of components per generic vertex attribute
            //                            ↓ 
			  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
            //                                                 ↑ 
            //                                                 + next vertex shift 6 floats
            //
			// vertex
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, numOfComponentAttri*sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // color attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, numOfComponentAttri*sizeof(float), (void*)(3*sizeof(float)));
            //                                                 ↑
            //                                                 + -> next color shift 6 floats
            //
            glEnableVertexAttribArray(1);
            // END_Draw_triangle
        }
        void draw(GLenum mode){
            // glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
            glBindVertexArray(VAO);
            glDrawArrays(mode, 0, numVertex);
        }
        ~Draw(){
            free(pt);
        }
};

/*
Ref: https://learnopengl.com/Getting-started/Shaders
Data: 18-08-2020
KEY: opengl shader, vertex shader fragment shader, shader tutorial, shader uniform
Compile: opengl_compile.sh

UPDATE: Sat 18 Dec 23:33:42 2021 
1. Use raw string in shader string
2. Add translation matrix to translate the curve
3. Fixed bug: exit the code if there is shader error

*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Frustum& frustum);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


std::string vertexShaderStr = R"(
#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    // uniform mat4 mymat;
    out vec3 ourColor;
    uniform mat4 rotYMat;
    uniform mat4 rotXMat;
    uniform mat4 rotZMat;
    uniform mat4 viewMat;

    // translation matrix
    uniform mat4 trMat;  
    uniform mat4 projFrum;
    void main()
    {
        mat4 translate;
        translate = mat4( 1.0, 0.0, 0.0,  0.0,
                          0.0, 1.0, 0.0,  0.0,
                          0.0, 0.0, 1.0,  0.0,  
                          -0.5, -0.5, 0.0, 1.0);

        mat4 tmat;
        tmat = mat4(1.2, 0.0, 0.0, 0.0,
                    0.0, 1.2, 0.0, 0.0,
                    0.0, 0.0, 1.2, 0.0,  
                    0.0, 0.0, 0.0, 1.0);

        mat4 mm = projFrum;
        vec4 vv = projFrum * viewMat * vec4(aPos, 1.0);
        gl_Position = projFrum * viewMat * trMat * rotYMat * rotXMat * rotZMat* vec4(aPos, 1.0);
        // gl_Position = rotYMat * vec4(aPos, 1.0);
        // gl_Position = vec4(aPos, 1.0);
        // ourColor = aPos; 
        ourColor = aColor; 

        // gl_Position = tmat * vec4(aPos, 1.0);
    }
)";

std::string fragStr = R"(
#version 330 core
    out vec4 FragColor;
    in  vec3 ourColor;
    // uniform vec4 ourColor;
    void main()
    {
       FragColor = vec4(ourColor, 1.0f);
       // FragColor = vec4(0.5, 0.5, 0.5, 1.0f);
    }
)";

const char *vertexShaderSource = vertexShaderStr.c_str();

const char *fragmentShaderSource = fragStr.c_str(); 

/**
 *   1/2
 *   1 x 1/2,     2 x 1/2
 *    (1/2)(1/2)    1 x 1
 *  
 *
 *  
 *   |               |             |
 *   offset+0     offset+1     offset+2
 */ 
const int xyzVertexes = 3*3;
void fillArray(float arr[xyzVertexes]){
    int nVert = (int)(xyzVertexes/3); 
    float delta = (float)1/nVert;
    for(int i = 0; i < nVert; i++){
        float val = delta*(i + 1);
        int offset = 3*i ;
        arr[offset+0] = val;
        arr[offset+1] = val*val;
        arr[offset+2] = 0.0f;
    }
}

int shaderSetVec4(GLuint shaderHandle, string uniformVarStr, float vec4[4]){
    glUseProgram(shaderHandle);
    int vec4Id = glGetUniformLocation(shaderHandle, uniformVarStr.c_str());
    printf("vec4Id=%d\n", vec4Id);
    glUniform4f(vec4Id, vec4[0], vec4[1], vec4[2], vec4[3]);
}

void shaderSetMatrix(GLuint shaderHandle, string uniformVarStr, float arr[16]){
    glUseProgram(shaderHandle);
    int matrixId = glGetUniformLocation(shaderHandle, uniformVarStr.c_str());
    if (matrixId != -1){
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, arr);
    }else{
        printf("ERROR: glGetUniformLocation(..)");
        exit(1);
    }
    // printf("matrix id=%d\n", matrixId);
}

// +X ⨂ +Y = +Z
void rotateZ(float outmat[16], float alpha){
    float mat[] = {
        cos (alpha), -sin (alpha), 0, 0,
        sin (alpha), cos (alpha),  0, 0,
        0          , 0          ,  1, 0,
        0          , 0          ,  0, 1
    };

	for(int i = 0; i < 16; i++)
	  outmat[i] = mat[i];

	// transpose, change to column-major matrix
    tranmat4(outmat);
}

// +X ⨂ +Z = -Y
void rotateY(float outmat[16], float alpha){
    float mat[] = {
	    cos(alpha),   0,  sin(alpha), 0, 
		0          ,  1,  0,          0, 
        -sin(alpha),  0,  cos(alpha), 0,
        0,            0,  0,          1,
    };

	for(int i = 0; i < 16; i++)
	  outmat[i] = mat[i];

	// transpose, change to column-major matrix
    tranmat4(outmat);
}

// +Y ⨂ +Z = +X
// outmat[16] => Column-major matrix
void rotateX(float outmat[16], float alpha){
    float mat[] = {
	    1, 0,           0,            0,
	    0, cos (alpha), -sin (alpha), 0,
        0, sin (alpha), cos (alpha),  0,
        0, 0,  0,                     1
    };
	for(int i = 0; i < 16; i++)
	  outmat[i] = mat[i];

	// transpose, change to Column-major matrix
    tranmat4(outmat);
}

void translate(float outmat[16], float x, float y, float z){
    float mat[] = {
        1,  0, 0, x,
        0,  1, 0, y,
        0,  0, 1, z,
        0,  0, 0, 1
    };

	for(int i = 0; i < 16; i++)
	  outmat[i] = mat[i];

	// transpose, change to column-major matrix
    tranmat4(outmat);
}


// URL: http://localhost/html/indexUnderstandOpenGL.html
void frustum(float left, float right, float bottom, float top, float near, float far, float mat[16]){
  float l = left;
  float r = right;
  float b = bottom;
  float t = top;
  float n = near;
  float f = far;
  
  // Row-major
  float m[] = {
	2 * n / (r - l),    0,           (r + l) / (r - l), 0,
	0,                  2*n/(t - b), (t + b) / (t - b), 0,
	0,                  0,           (n + f) / (n - f), 2*n*f/(n - f),
	0,                  0,           -1,                0
  };
  
  for(int i = 0; i < 16; i++){
	mat[i] = m[i];
  }

  // Convert Row-major to Column-major
  tranmat4(mat);
}

using namespace MatrixVector;


/**
   return a Column-major 4 x 4 view matrix
   
   URL: http://localhost/html/indexUnderstandOpenGL.html#perspective_matrix

   vector<float> eye = {0, 2, 3};
   vector<float> at  = {0, 0, 0};
   vector<float> up  = {0, 1, 0};
   
 */
void lookAtMatrix(vector<float> eye, vector<float> at, vector<float> up, float viewMat[]){
  	const float sq = sqrt(13);

	vector<float> lookDir = subtract(at, eye);
	print(lookDir);
	
	// right or size => s
	vector<float> s = crossProduct<float>(lookDir, up);
	print(s);

	vector<float> u = crossProduct(s, lookDir);
	print(u);

	vector<float> s_n = normalize(s);
	vector<float> u_n = normalize(u);
	vector<float> lookDir_n = normalize(lookDir);
	
	vector<vector<float>> vv = {s_n, u_n, negateVec(lookDir_n)};
	mat m1(vv);
	vec v(3);
	fw("insertVecNext 1");
	mat m2 = m1.insertVecNext(2, v);
	m2.print();
	row r(4);
	mat m3 = m2.addRow(r);
	m3.arr[3][3] = 1;
	fw("m3 Row-major");
	m3.print();
	
	fw("m4 Row-major R^{-1}  ∵ R is orthonormal matrix");
	mat m4 = m3.transpose();

	m4.print();
	float mymat[16];
	matToArr(m4, mymat, 16);
	fw("matToArr => Column-major");
	printArr(mymat, 16);
	
	// Create Row-major translation inverse matrix
	mat tr(4, 4);
	mat tr1 = tr.id();
	vector<float> tv = add(negateVec(eye), float(1));
	tr1.setVec(3, tv);

	// Row-major vm = R^{-1} * T^{-1}
	mat vm = tr1 * m4;
	fw("Row-major veiw matrix");
	vm.print();

	// Change vm to Column-major
	mat colViewMat = vm.transpose();

	// Convert Column-major mat to float array[16]
	matToArr(colViewMat, mymat, 16);
	
	
	// z => lookDir
	// x => s
	// y => u
	/*
	float mat[] = {
		1, 0,     0,    0,
		0, 3/sq, -2/sq, 0,
		0, 2/sq, 3/sq,  0,
		0, 0,    0,     1
	  };
	*/
	for(int i = 0; i < 16; i++){
	  viewMat[i] = mymat[i];
	}
	/*
	float viewMat[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	  };
	*/
	
	// Column-major
	// tranmat4(viewMat);
	
}

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

	/*
	float rx = 0.5;
    float box[] = {
	    0,   0, 0, 0.5f, 0.0f, 0.0f,
        rx,  0, 0, 0.5f, 0.0f, 0.0f,
        0,   rx, 0, 0.5f, 0.0f, 0.0f,
        rx,  rx, 0, 0.5f, 0.0f, 0.0f,

        0,  rx, -rx, 0.5f, 0.0f, 0.0f,
        rx, rx, -0.5, 0.5f, 0.0f, 0.0f,
        0,  0, -rx,  0.5f, 0.0f, 0.0f,
        rx, 0, -rx, 0.5f, 0.0f, 0.0f,

        rx, rx, 0, 0.5f, 0.0f, 0.0f,
        rx, 0, 0, 0.5f, 0.0f, 0.0f,
        0, 0, -rx, 0.5f, 0.0f, 0.0f,
        0, 0, 0, 0.5f, 0.0f, 0.0f,

        0, rx, -rx, 0.5f, 0.0f, 0.0f,
        0, rx,  0, 0.5f, 0.0f, 0.0f
    };
	*/

    /**
	SEE: pdf file
	http://localhost/pdf/triangle_strips_for_fast_rendering.pdf
	https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
	
	    NOTE: Need to enable face culling in order to see the cube properly
	    Enable face culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull the back faces
	*/
	float r = 0.5;
    float box[] = {
	  -r, -r, -r,  0.5f, 0.0f, 0.0f, // 4
      r,  -r, -r,  0.5f, 0.3f, 0.0f, // 3
      -r, -r, r,   0.2f, 0.0f, 0.3f, // 7
      r,  -r, r,   0.5f, 0.0f, 0.4f, // 8

	  r,  r,  r,  0.5f, 0.0f, 0.0f, // 5
	  r,  -r, -r, 0.4f, 0.5f, 0.0f, // 3
	  r,  r,  -r, 0.5f, 0.0f, 0.0f, // 1
	  -r, -r, -r, 0.5f, 0.2f, 0.1f, // 4

	  -r, r,  -r, 0.5f, 0.4f, 0.0f, // 2
	  -r, -r, r,  0.5f, 0.0f, 0.3f, // 7
	  -r, r,  r,  0.5f, 0.2f, 0.0f, // 6
	  r,  r,  r,  0.5f, 0.4f, 0.0f, // 5

	  -r, r, -r,  0.5f, 0.3f, 0.0f, // 2
	  r,  r, -r,  0.5f, 0.5f, 0.0f  // 1
    };
	
    float tri[] = {    
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, 
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 
        0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
                          //  R     G     B
    };
	int numVertex = 3;
	// 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f = 6 components
	int numOfComponentAttri = 6;
    Draw* triPt = new Draw(sizeof(tri), tri, numVertex, numOfComponentAttri);
	
	float rectangle[] = {
	  -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
	  0.5f,  -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,
	  -0.5f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
	  0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f
	  //                      R     G     B
    };
	
	int rect_numVert = 4;
	int rect_numComp = 6;
	Draw* rectPt = new Draw(sizeof(rectangle), rectangle, rect_numVert, rect_numComp);

	int box_numVert = 14;
	int box_numComp = 6;
	Draw* boxPt = new Draw(sizeof(box), box, box_numVert, box_numComp);
	
	float r1 = 0.5f;


	/*
    float tetrahedron[] = {
	  1/sqrtf(3.0)*r1,       0,           0,                0.5f, 0.0f, 0.9f,	// 1
	  -1/(2 * sqrtf(3.0f))*r1,  0,           -(1/2.0f)*r1,  0.5f, 0.3f, 0.0f,	// 2
	  0.0f,      sqrtf(2/3.0f)*r1, 0,              0.2f, 0.0f, 0.3f,	// 3           
	  -1/(2*sqrt(3.0f))*r1,  0,               1/2.0f*r1,   0.2f, 0.2f, 0.0f,	// 4
	  1/sqrtf(3.0)*r1,       0,           0,                0.5f, 0.0f, 0.9f,	// 5
	  -1/(2.0f * sqrtf(3.0f))*r1,  0,           -(1/2.0f)*r1,  0.5f, 0.3f, 0.0f	// 2
    };
	*/
	
   // http://localhost/html/indexTetrahedron.html
	/**
	  Check whether all the edges have the same length
      a1 ← 1, ¯1, 1
      a2 ← ¯1, 1, 1
      a3 ← ¯1, ¯1, ¯1
      a4 ← 1, 1, ¯1
      a5 ← 1, ¯1, 1
      a6 ← ¯1, 1, 1
      dist ← {x ← ⍺ - ⍵ ⋄ 0.5*⍨ +/ x*2}
      e12 ← a1 dist a2
      e23 ← a2 dist a3
      e34 ← a3 dist a4
      e45 ← a4 dist a5
      e56 ← a5 dist a6
      eq ← e12 e23 e34 e45 e56
	  ∧/ 2=/eq
      1

	 1∘=¨ 2=/eq
	 ┌→──────┐
	 │1 1 1 1│
	 └~──────┘
	 */
	float tetrahedron[] = {
	  r1, -r1, r1,                1.0f, 0.0f, 0.0f,	// A a1
	  -r1, r1,  r1,               1.0f, 0.0f, 0.0f,	// B a2        
	  -r1,-r1,  -r1,              0.0f, 1.0f, 0.0f,	// C a3
	  r1, r1,  -r1,               0.0f, 1.0f, 0.0f,	// D a4
	  r1, -r1, r1,                0.0f, 0.0f, 1.0f,	// A a5
	  -r1, r1,  r1,               0.0f, 0.0f, 1.0f,	// B a6        
    };
	
    
	fw("tetrahedron");
	// printArr(tetrahedron, 6*6, 6);
	
	/*
    float tetrahedron[] = {
	  -r, -r, -r,  0.5f, 0.0f, 0.0f, // 4
      r,  -r, -r,  0.5f, 0.3f, 0.0f, // 3
      -r, -r, r,   0.2f, 0.0f, 0.3f, // 7
      r,  -r, r,   0.5f, 0.0f, 0.4f, // 8

	  r,  r,  r,  0.5f, 0.0f, 0.0f, // 5
	  r,  -r, -r, 0.4f, 0.5f, 0.0f, // 3
	};
	*/

	int tetrahedron_numVert = 6;
	int tetrahedron_numComp = 6;
	Draw* tetraPt = new Draw(sizeof(tetrahedron), tetrahedron, tetrahedron_numVert, tetrahedron_numComp);
	
	const float sq = sqrt(13);

	float viewMat[16];
	
	// vector<float> eye = {0, 2, 3};
	vector<float> eye = {0, 0, 1.5};
	pp("eye=");
	pp(eye);
	vector<float> at  = {0, 0, 0};
	vector<float> up  = {0, 1, 0};
	
	lookAtMatrix(eye, at, up, viewMat);
 
	// tranmat4(testMat);
	fw("testMat");
	printArr(viewMat, 16, 4);
	
    GLuint viewMatId = glGetUniformLocation(shaderProgram, "viewMat");
    if (viewMatId == -1){
	  printf("ERROR: viewMatId=%d\n", viewMatId);
	  exit(1);
    }else{
    }
	
	GLuint projFrumId = glGetUniformLocation(shaderProgram, "projFrum");
    if (projFrumId == -1){
	  printf("ERROR: projFrumId=%d\n", projFrumId);
	  exit(1);
    }else{
    }

    GLuint rotYMatId = glGetUniformLocation(shaderProgram, "rotYMat");
    if (rotYMatId == -1){
        printf("ERROR: rotYMatId=%d\n", rotYMatId);
        exit(1);
    }else{
    }
	
	GLuint rotXMatId = glGetUniformLocation(shaderProgram, "rotXMat");
    if (rotXMatId == -1){
        printf("ERROR: rotXMatId=%d\n", rotXMatId);
        exit(1);
    }else{
    }
	
	GLuint rotZMatId = glGetUniformLocation(shaderProgram, "rotZMat");
    if (rotZMatId == -1){
        printf("ERROR: rotZMatId=%d\n", rotZMatId);
        exit(1);
    }else{
    }
	
	GLuint trMatId = glGetUniformLocation(shaderProgram, "trMat");
    if (trMatId == -1){
        printf("ERROR: trMatId=%d\n", trMatId);
        exit(1);
    }else{
    }

	float projFrum[16];
	float left = -1;
	float right = 1;
	float bottom = -1;
	float top = 1;
	// near and far must be positive in glFrumstum()
	float near = 1;
	float far = 20;
	fw("near=");
	pp(near);
	fw("far=");
	pp(far);
	
	// Return Column-major matrix
	frustum(left, right, bottom, top, near, far, projFrum);

	float trMat[16];
	float trX = 0.0f;
	float trY = 0.0f;
	float trZ = 0.0f;

	// trMat is identity matrix
	translate(trMat, trX, trY, trZ);
	
	Frustum fmInfo;
    unsigned int VBO4;
    unsigned int VAO4;
    // rectPt -> setupVBOVAO();
	boxPt -> setupVBOVAO();
	tetraPt -> setupVBOVAO();
	float rotYMat[16];
	float rotXMat[16];
	float rotZMat[16];
	float rotXVal = 0.0f;
	float rotYVal = 3.1415926/6.0f;
	float rotZVal = 0.0f;
	bool enableWireFrame = false;
	bool drawBox = false;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
	  
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		  rotYVal += 0.1;
		}else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS){
		  rotYVal -= 0.1;
		} else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
		  rotXVal += 0.1;
		}else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
		  rotXVal -= 0.1;
		}else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS){
		  rotZVal += 0.1;
		}else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
		  rotZVal -= 0.1;
		}else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		  trX -= 0.1;
		}else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		  trX += 0.1;
		}else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		  trY -= 0.05;
		}else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		  trY += 0.05;
		}else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		  enableWireFrame = !enableWireFrame;
		}else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS){
		  drawBox = !drawBox;
		}else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
		  rotXVal = 0.0f;
		  rotYVal = 3.1415926/6.0f;
		  rotZVal = 0.0f;
		  trX = 0.0f;
		  trY = 0.0f;
		  trZ = 0.0f;
		}
		
	    processInput(window, fmInfo);
		createFrustumMat(fmInfo);

		for(int i = 0; i < 16; i++){
		  projFrum[i] = fmInfo.frustumMat[i];
		}

		// writeFileAppend("/tmp/debug.x", fmInfo.toStr());

		/*
		sleep(1);
		printArr(viewMat, 16, 4);
		*/
		// glfwSetKeyCallback(window, key_callback);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// KEY: GL_DEPTH_BUFFER_BIT, depth test
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Enable face culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull the back faces
		
		// Enable wireframe
		if(enableWireFrame){
		  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}else{
		  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
        // be sure to activate the shader before any calls to glUniform
        // glUseProgram(shaderProgram);

        // update shader uniform
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        float redValue   = cos(timeValue) / 2.0f + 0.5f;

		float beta = sin(timeValue);
		rotateX(rotXMat, rotXVal);
		rotateY(rotYMat, rotYVal);
		rotateZ(rotZMat, rotZVal);

		translate(trMat, trX, trY, trZ);
		
	    // lookAtMatrix(eye, at, up, viewMat);
		
		/*
        triPt -> setMatrix(shaderProgram, "rotYMat", rotYMat);
		triPt -> setMatrix(shaderProgram, "viewMat", viewMat);
		triPt -> setMatrix(shaderProgram, "projFrum", projFrum);
        triPt -> draw(GL_TRIANGLE_STRIP);
		*/
		
		/*
		rectPt -> setMatrix(shaderProgram, "rotYMat", rotYMat);
		rectPt -> setMatrix(shaderProgram, "viewMat", viewMat);
		rectPt -> setMatrix(shaderProgram, "projFrum", projFrum);
        rectPt -> draw(GL_TRIANGLE_STRIP);
		*/
		
		
		boxPt -> setMatrix(shaderProgram, "rotXMat", rotXMat);		
		boxPt -> setMatrix(shaderProgram, "rotYMat", rotYMat);
		boxPt -> setMatrix(shaderProgram, "rotZMat", rotZMat);
		boxPt -> setMatrix(shaderProgram, "viewMat", viewMat);
		boxPt -> setMatrix(shaderProgram, "projFrum", projFrum);
		boxPt -> setMatrix(shaderProgram, "trMat", trMat);
		
		if(drawBox){
		  boxPt -> draw(GL_TRIANGLE_STRIP);
		}
		
		
		tetraPt -> setMatrix(shaderProgram, "rotXMat", rotXMat);		
	    tetraPt -> setMatrix(shaderProgram, "rotYMat", rotYMat);
	    tetraPt -> setMatrix(shaderProgram, "rotZMat", rotZMat);
		tetraPt -> setMatrix(shaderProgram, "viewMat", viewMat);
	    tetraPt -> setMatrix(shaderProgram, "projFrum", projFrum);
		tetraPt -> setMatrix(shaderProgram, "trMat",    trMat);
        tetraPt -> draw(GL_TRIANGLE_STRIP);
		

		
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // glDeleteVertexArrays(0, &VAO);
    // glDeleteBuffers(0, &VBO);


    glDeleteProgram(shaderProgram);
    delete triPt;
	delete rectPt;
	delete boxPt;
	delete tetraPt;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// URL: https://www.glfw.org/docs/3.3/group__keys.html
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Frustum& fm) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
  }else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
	fm.near -= 0.01;
	log("press A");
  }else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS){
	fm.near += 0.01;
	log("pres B");
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


