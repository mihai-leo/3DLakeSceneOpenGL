#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp" //core glm functionality
#include "glm/gtc/matrix_transform.hpp" //glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp" //glm extension for computing inverse matrices
#include "glm/gtc/type_ptr.hpp" //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include "SkyBox.hpp"

#include <iostream>
#include <fstream>

//Done by me 


bool wireframeMode = false;
bool pointMode = false;
bool fogEnable = false;
bool light1Enable = true;
bool light2Enable = true;
bool movementCroc = true;


int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;
bool animate = false;
bool mousefirtposition = false;
double lastx, lasty;
float yaw = -90.0f;
float pitch = 0.0f;
int z = 0;
glm::vec3 pointLightPosition = glm::vec3(0.81008f, 0.127388f, -0.05721f); // Example position

//glm::vec3 pointLightPosition = glm::vec3(-1.00328f, 0.226083f, 1.24274f); // Example position
glm::vec3 pointLightColor = glm::vec3(3.0f, 0.7f, 0.0f);     // White light
float constant = 1.0f;
float linear = 1.9f;
float quadratic = 3.032f;
glm::mat4 lightRotation;
//Shadows 
// Shadow mapping resources
const unsigned int SHADOW_WIDTH =  4*2048;
const unsigned int SHADOW_HEIGHT =  4*2048;


GLuint shadowMapFBO;
GLuint depthMapTexture;
GLfloat lightAngle=0.0f;

float crocAngle = 0;

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 1.0f, 2.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.05f;

GLboolean pressedKeys[1024];

gps::SkyBox mySkyBox;
// models
gps::Model3D lake;
gps::Model3D lakeWater;
gps::Model3D crocodile;
gps::Model3D screenQuad;
gps::Model3D lightCube;
GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;
bool showDepthMap;


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        animate = !animate;
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        light1Enable = !light1Enable;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        light2Enable = !light2Enable;
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        wireframeMode = !wireframeMode;
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        pointMode = !pointMode;
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
        fogEnable = !fogEnable;

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {


    if (!mousefirtposition)
    {
        lastx = xpos;
        lasty = ypos;
        mousefirtposition = true;
        return;
    }
    float xoffset = xpos - lastx;
    float yoffset = lasty - ypos;
    lastx = xpos;
    lasty = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = std::max(-89.0f, std::min(pitch, 89.0f)); // Clamp pitch

    // Update the camera's orientation
    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for lake
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

}
void procesAnimations()
{
    crocAngle -= 1.0f;
    if (animate)
    {
        lightAngle += 0.5f;


        myCamera.newLocation();
    }
    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 1.0f;

    }
    if (fogEnable)
    {
        myBasicShader.useShaderProgram();
        GLuint fogEnableLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogEnable");
        glUniform1i(fogEnableLoc, fogEnable ? 1 : 0);
    }
    else
    {
        myBasicShader.useShaderProgram();
        GLuint fogEnableLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogEnable");
        glUniform1i(fogEnableLoc, fogEnable ? 1 : 0);
    }
    if(light1Enable)
    { 
        myBasicShader.useShaderProgram();
        //set light color
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        // send light color to shader
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }
    else
    {
        myBasicShader.useShaderProgram();
        lightColor = glm::vec3(0.0f, 0.0f, 0.0f); //white light
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        // send light color to shader
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }
    if (light2Enable)
    {
        myBasicShader.useShaderProgram();
        //set light color
        pointLightColor = glm::vec3(2.0f, 0.2f, 0.2f);
         GLint pointLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.color");
   
         glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));
    }
    else
    {
        myBasicShader.useShaderProgram();
        //set light color
        pointLightColor = glm::vec3(0.0f, 0.0f, 0.0f);
        GLint pointLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.color");

        glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));
    }
}
void processMovement() {



	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));

	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));

	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}
    if (pressedKeys[GLFW_KEY_R]) {
        myCamera.move(gps::MOVE_UP, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_F]) {
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for lake
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for lake
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for lake
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));

    }
}
bool initOpenGLWindow()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(glWindow);

    glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    //for RETINA display
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    return true;
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);

    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

    glEnable(GL_FRAMEBUFFER_SRGB);
}

void initModels() {
    lake.LoadModel("models/lake2/lake.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    crocodile.LoadModel("models/crocodile/crocodile.obj");
    lakeWater.LoadModel("models/lake/water.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    myBasicShader.useShaderProgram();

    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depth.vert", "shaders/depth.frag");
    depthMapShader.useShaderProgram();
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();

}
void initSkybox()
{
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");
    mySkyBox.Load(faces);
}
void initUniforms() {
	myBasicShader.useShaderProgram();
  //  depthShader.useShaderProgram();
    // create model matrix for lake
   // model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	//modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for lake
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	// create projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    


    GLint pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.position");
    GLint pointLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.color");
    GLint pointLightConstantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.constant");
    GLint pointLightLinearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.linear");
    GLint pointLightQuadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.quadratic");

    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPosition));
    glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));
    glUniform1f(pointLightConstantLoc, constant);
    glUniform1f(pointLightLinearLoc, linear);
    glUniform1f(pointLightQuadraticLoc, quadratic); 

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


}
void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);
    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::mat4 computeLightSpaceTrMatrix() {
    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 10.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    return(lightProjection * lightView);
}

void viewMode()
{
    if (pointMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    else {
        if (wireframeMode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        }
    }
}
void drawObjects(gps::Shader shader, bool depthPass) {



    viewMode();
    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
      
    }


    glUniform1f(glGetUniformLocation(shader.shaderProgram, "transparency"), 1.0f);
    lake.Draw(shader);



    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-0.2f, 0.02f, 0.82f ));
    model = glm::rotate(model, glm::radians(crocAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(cos(crocAngle * 3.14159265358979323846f / 180.0) / 3.7f, -0.087f+cos(crocAngle * 5.14159265358979323846f / 180.0) / 30.0f, 0.37f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    crocodile.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    }
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "transparency"), 0.6f);
    lakeWater.Draw(shader);


   
}

void renderlake(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send lake model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send lake normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw lake
    lake.Draw(shader);
}

void renderScene() {
    depthMapShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceMatrix"), 1,GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    drawObjects(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap) {
        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(5.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        drawObjects(myBasicShader, false);

        //draw a white cube around the light
        
        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        model = lightRotation;
       model = glm::translate(model, 5.0f * lightDir);

        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        lightCube.Draw(lightShader);
        skyboxShader.useShaderProgram();
        mySkyBox.Draw(skyboxShader, view, projection);
        
    }
}

void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    glfwDestroyWindow(glWindow);
    //close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char * argv[]) {

    if (!initOpenGLWindow()) {
        glfwTerminate();
        return 1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initFBO();
   initSkybox();
    glCheckError();

    while (!glfwWindowShouldClose(glWindow)) {
        processMovement();
        procesAnimations();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }

    cleanup();

    return 0;
}
