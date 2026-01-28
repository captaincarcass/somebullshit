
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.hpp"
#include "gui.hpp"

void framebuffer_size_callback(int width, int height);
GLFWwindow* initWindow();
void processCameraInputs(OrthoCamera &camera, float deltaTime);
void processCameraInputs(PerspectiveCamera &camera, float deltaTime);
void processGUIInputs();
bool isPressed(int key);

extern GLFWwindow* window;
extern uint64_t SCR_WIDTH;
extern uint64_t SCR_HEIGHT;
extern uint64_t g_windowWidth;
extern uint64_t g_windowHeight;
