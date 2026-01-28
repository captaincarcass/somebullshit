#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.hpp"
#include "window.hpp"
#include "gui.hpp"

GLFWwindow* window = nullptr;

extern struct ShaderGUI::ShaderGUIState shaderGUIState;

void processCameraInputs(PerspectiveCamera &camera, float deltaTime) {
    if(isPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
    if(isPressed(GLFW_KEY_W)) camera.processInput(Camera_Movement::FORWARD, deltaTime);
    if(isPressed(GLFW_KEY_S)) camera.processInput(Camera_Movement::BACKWARD, deltaTime);
    if(isPressed(GLFW_KEY_A)) camera.processInput(Camera_Movement::LEFT, deltaTime);
    if(isPressed(GLFW_KEY_D)) camera.processInput(Camera_Movement::RIGHT, deltaTime);
    if(isPressed(GLFW_KEY_R)) camera.processInput(Camera_Movement::PITCH_UP, deltaTime);
    if(isPressed(GLFW_KEY_F)) camera.processInput(Camera_Movement::PITCH_DOWN, deltaTime);
    if(isPressed(GLFW_KEY_Q)) camera.processInput(Camera_Movement::YAW_LEFT, deltaTime);
    if(isPressed(GLFW_KEY_E)) camera.processInput(Camera_Movement::YAW_RIGHT, deltaTime);
    if(isPressed(GLFW_KEY_SPACE)) camera.processInput(Camera_Movement::UP, deltaTime);
    if(isPressed(GLFW_KEY_LEFT_SHIFT)) camera.processInput(Camera_Movement::DOWN, deltaTime);
}

void processCameraInputs(OrthoCamera &camera, float deltaTime) {
    if(isPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
    if(isPressed(GLFW_KEY_W)) camera.processInput(Camera_Movement::FORWARD, deltaTime);
    if(isPressed(GLFW_KEY_S)) camera.processInput(Camera_Movement::BACKWARD, deltaTime);
    if(isPressed(GLFW_KEY_A)) camera.processInput(Camera_Movement::LEFT, deltaTime);
    if(isPressed(GLFW_KEY_D)) camera.processInput(Camera_Movement::RIGHT, deltaTime);
    if(isPressed(GLFW_KEY_Z)) camera.processInput(Camera_Movement::ZOOM_POS, deltaTime);
    if(isPressed(GLFW_KEY_X)) camera.processInput(Camera_Movement::ZOOM_NEG, deltaTime);
}

void processGUIInputs() {
    static bool pickerPicked { false };
    static bool showKeyWasPressed { false };

    bool showKeyPressed = isPressed(GLFW_KEY_LEFT_CONTROL) && isPressed(GLFW_KEY_S);

    if (showKeyPressed && !showKeyWasPressed) {
	pickerPicked = !pickerPicked;
	ShaderGUI::shaderGUIState.isShaderPickerGUI = pickerPicked;
    }

    showKeyWasPressed = showKeyPressed;
}

void framebuffer_size_callback(GLFWwindow* window,int width, int height) {
    g_windowWidth = width;
    g_windowHeight = height;
    glViewport(0, 0, width, height);
}

GLFWwindow* initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GL_MULTISAMPLE, 4);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "test", NULL, NULL);
    if (window == NULL) {
	std::cout << "Failed to Create GLFW window" << std::endl;
	glfwTerminate();
	return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);

    glfwSwapInterval(0);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	std::cout << "Failed to initialize GLAD" << std::endl;
	return nullptr;
    }
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<unsigned int>::max());
    return window;
}

bool isPressed(int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

