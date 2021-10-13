#include <stdio.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define numVAOs 1
#define numVBOs 2

// function declaration
void setupVertices(void);
void printShaderLog(GLuint shader);
void printProgramLog(int prog);
int checkOpenGLError();
GLuint createShaderProgram(char * vertShaderSrc, char * fragShaderSrc);
void init(GLFWwindow * window);
void display(GLFWwindow * window, double currentTime);

float xCamera, yCamera, zCamera;
float xCubeLoc, yCubeLoc, zCubeLoc;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// allocate variables used in display() function, so that they won't need to be
// allocated during rendering
GLuint mvLoc, projLoc;
int width, height;
float aspect;
mat4 pMat, vMat, mMat, mvMat;

int main() {
    if(!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow * window = glfwCreateWindow(600, 600, "Cube", NULL, NULL);
    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK) {
        exit(EXIT_FAILURE);
    }

    glfwSwapInterval(1);
    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void setupVertices(void) {
    float vertexPositions[108] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
         1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
    };
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), 
            vertexPositions, GL_STATIC_DRAW);
}

//OpenGL log when GLSL compilation failed
void printShaderLog(GLuint shader){
	int len = 0;
    int chWrittn = 0;
	char *log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if(len > 0){
		log = (char *)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		printf("Shader Info Log: %s\n", log);
		free(log);
	}
}


//OpenGL log when GLSL linking failed
void printProgramLog(int prog){
	int len = 0;
	int chWrittn = 0;
	char *log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if(len > 0){
		log = (char *) malloc(len);
        glGetProgramInfoLog(prog, len, &chWrittn, log);
		printf("Program Info Log: %s\n", log);
		free(log);
	}
}

//checks OpenGL error flag for OpenGL error
int checkOpenGLError(){
	int foundError = 0;
	int glErr = glGetError();
	while(glErr != GL_NO_ERROR){
		printf("glError: %d\n", glErr);
		foundError = 1;
		glErr = glGetError();
	}
	return foundError;
}

char* readShaderSource(char* filePath){
	char* buffer = NULL;
	int stringSize, readSize;
	FILE* file = fopen(filePath, "r");

	if(file){
		fseek(file, 0, SEEK_END); //Seek last byte of File
		stringSize = ftell(file); //Offset from first to last byte(filesize)
		rewind(file); //bakc to start of file
		buffer = (char*) malloc(sizeof(char)* (stringSize + 1)); //Allocate a string with size of file

		readSize = fread(buffer, sizeof(char), stringSize, file); //read all of file
		buffer[stringSize] = '\0';

		//Error checking
		if(stringSize != readSize){
			free(buffer);
			buffer = NULL;
		}
		fclose(file);
		return buffer;
	}
    return NULL;
}

GLuint createShaderProgram(char * vertShaderSrc, char * fragShaderSrc){
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	//Shader Sources as strings from file
	const char* vertShaderStr = readShaderSource(vertShaderSrc);
	const char* fragShaderStr = readShaderSource(fragShaderSrc);

	//Create Shader
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Loads GLSL code into Shader Obj
	glShaderSource(vShader, 1, &vertShaderStr, NULL);
	glShaderSource(fShader, 1, &fragShaderStr, NULL);

	//Create Vertex Shader and Error check
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if(vertCompiled != 1){
		printf("vertex compilation failed!\n");
		printShaderLog(vShader);
	}

	//Create Fragment Shader and error check
	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if(fragCompiled != 1){
		printf("fragment compilation failed\n");
		printShaderLog(fShader);
	}

	//Create Program containig compiled Shaders and link them
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if(linked != 1){
		printf("linking failed\n");
		printProgramLog(vfProgram);
	}

	return vfProgram;
}

void init(GLFWwindow * window) {
    renderingProgram = createShaderProgram("Shaders/cubeVert.glsl", 
            "Shaders/cubeFrag.glsl");
    xCamera = 0.0f; yCamera = 0.0f; zCamera = 8.0f;
    xCubeLoc = 0.0f; yCubeLoc = -2.0f; zCubeLoc = 0.0f;
    setupVertices();
}

void display(GLFWwindow * window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);

    // get uniform variables for MV and projection matrices
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float) width / (float) height;
    glm_perspective(1.0472f, aspect, 0.1f, 1000.0f, pMat);
    
    // use current time to compute different translations in x, y, z
    mat4 tMat, rMat;
    glm_mat4_identity(tMat);
    glm_mat4_identity(rMat);
    glm_translate(tMat,(vec3) {sin(0.35f * currentTime) * 2.0f, cos(0.52f * currentTime) * 2.0f, sin(0.7f * currentTime) * 2.0f});
    glm_rotate(rMat, 1.75f * (float)currentTime, (vec3) {0.0f, 1.0f, 0.0f});
    glm_rotate(rMat, 1.75f * (float)currentTime, (vec3) {1.0f, 0.0f, 0.0f});
    glm_rotate(rMat, 1.75f * (float)currentTime, (vec3) {0.0f, 0.0f, 1.0f});
    
    // build view matrix, model matrix and model-view matrix
    glm_mat4_identity(vMat);
    glm_mat4_mul(tMat, rMat, mMat);
    glm_translate(vMat, (vec3 ){-xCamera, -yCamera, -zCamera});
    glm_translate(mMat, (vec3 ){xCubeLoc, yCubeLoc, zCubeLoc});
    glm_mat4_mul(vMat, mMat, mvMat);

    // copy perspective and MV matrices to corresponding uniform variables
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (const float *) mvMat);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (const float *) pMat);

    // associate VBO with the corresponding vertex attribute in the vertex shader 
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // adjust OpenGL settings and draw model
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
