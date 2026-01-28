
#pragma once 

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    PITCH_UP,
    PITCH_DOWN,
    YAW_LEFT,
    YAW_RIGHT,
    ZOOM_POS,
    ZOOM_NEG,
};

const float YAW 	{-90.0f};
const float PITCH	{0.0f};
const float SPEED	{2.5f};
const float SENSITIVITY	{0.1f};
const float ZOOM	{45.0f};
const float ZOOM_SENSITIVITY {10};

const float ORTHO_ZOOM		{3.0f};
const float ORTHO_WIDTH 	{800.0f};
const float ORTHO_HEIGHT	{600.0f};

class PerspectiveCamera{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    PerspectiveCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f), 
	   glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
	   float yaw = YAW,
	   float pitch = PITCH);

    PerspectiveCamera(float xPos, float yPos, float zPos,
	   float xUp, float yUp, float zUp,
	   float yaw = YAW,
	   float pitch = PITCH);


    glm::mat4 getViewMatrix() const;

    void processInput(Camera_Movement direction, float deltaTime);    
    void processMouseMovement(float xoffset, float yoffset);
private:
    void updateVectors();
};

class OrthoCamera {
public:
    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 front;

    glm::mat4 ortho;
    
    float ratio;
    float viewWidth;
    float viewHeight;

    float movementSpeed;
    float zoom;
    float zoomSensitivity;

    OrthoCamera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 1.0f), float Width = ORTHO_WIDTH, float Height = ORTHO_HEIGHT);

    glm::mat4 getOrthoMatrix() const;
    glm::mat4 getViewMatrix() const;
    void processInput(Camera_Movement direction, float deltaTime);
private:
    float rightplane;
    float leftplane;
    float topplane;
    float bottomplane;
    float frontplane;
    float backplane;
    static float velfunc(float zoom);
    static float zoomfunc(float Zoom);
    void update(float (*f)(float));
};

void processCameraInputs(GLFWwindow* window, OrthoCamera &camera, float deltaTime);
void processCameraInputs(GLFWwindow* window, PerspectiveCamera &camera, float deltaTime);



