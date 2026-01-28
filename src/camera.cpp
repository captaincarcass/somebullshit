
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include "camera.hpp"

PerspectiveCamera::PerspectiveCamera(glm::vec3 Position, glm::vec3 Up, float Yaw, float Pitch): front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), zoom(ZOOM), mouseSensitivity(SENSITIVITY) {
    position = Position;
    worldUp = Up;
    yaw = Yaw;
    pitch = Pitch;
    updateVectors();
}
PerspectiveCamera::PerspectiveCamera(float xPos, float yPos, float zPos, float xUp, float yUp, float zUp, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), zoom(ZOOM), mouseSensitivity(SENSITIVITY) 
{
    position = glm::vec3(xPos, yPos, zPos);
    worldUp = glm::vec3(xUp, yUp, zUp);
    yaw = yaw;
    pitch = pitch;
    updateVectors();
}

glm::mat4 PerspectiveCamera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void PerspectiveCamera::updateVectors() {
    glm::vec3 Front{};
    Front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    Front.y = std::sin(glm::radians(pitch));
    Front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front = glm::normalize(Front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void PerspectiveCamera::processInput(Camera_Movement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    float rotationSpeed = mouseSensitivity * deltaTime * 400.0f;
    switch(direction) {
	case Camera_Movement::FORWARD:	position += front * velocity; break;
	case Camera_Movement::BACKWARD:	position -= front * velocity; break;
	case Camera_Movement::LEFT:	position -= right * velocity; break;
	case Camera_Movement::RIGHT:	position += right * velocity; break;
	case Camera_Movement::UP:	position += up * velocity; break;
	case Camera_Movement::DOWN:	position -= up * velocity; break;
	case Camera_Movement::PITCH_UP:	pitch += rotationSpeed; break;
	case Camera_Movement::PITCH_DOWN:pitch -= rotationSpeed; break;
	case Camera_Movement::YAW_LEFT:	yaw -= rotationSpeed; break;
	case Camera_Movement::YAW_RIGHT:yaw += rotationSpeed; break;
    }
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}

void PerspectiveCamera::processMouseMovement(float xoffset, float yoffset){
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}

OrthoCamera::OrthoCamera(glm::vec3 Position, float Width, float Height) 
    : movementSpeed(SPEED), zoom(ORTHO_ZOOM), zoomSensitivity(ZOOM_SENSITIVITY)
{
    rightplane	= Width/2;
    leftplane	= -Width/2;
    topplane	= Height/2;
    bottomplane	= -Height/2;
    frontplane	= 1000.0f;
    backplane	= -1000.0f;
    position 	= Position;
    right	= glm::vec3(1.0f, 0.0f, 0.0f);
    up		= glm::vec3(0.0f, 1.0f, 0.0f);
    front	= glm::vec3(0.0f, 0.0f, -1.0f);
    viewWidth	= Width;
    viewHeight	= Height;
    update(zoomfunc);
}

glm::mat4 OrthoCamera::getOrthoMatrix() const {
    return ortho;
}

glm::mat4 OrthoCamera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

float OrthoCamera::zoomfunc(float Zoom){
    return std::expf(-Zoom);
}

void OrthoCamera::update(float (*zoomfunc)(float)) {
    ortho = glm::ortho(leftplane*zoomfunc(zoom), rightplane*zoomfunc(zoom), topplane*zoomfunc(zoom), bottomplane*zoomfunc(zoom), backplane, frontplane);
}

void OrthoCamera::processInput(Camera_Movement direction, float deltaTime) {
    float velocity = 10 * movementSpeed * deltaTime;
    float zoom_vel = zoomSensitivity * deltaTime;
    switch(direction) {
	case Camera_Movement::RIGHT:	position += (right * velocity) / zoom; break;
	case Camera_Movement::LEFT:	position -= (right * velocity) / zoom;break;
	case Camera_Movement::UP:	position -= (up * velocity) / zoom; break;
	case Camera_Movement::DOWN:	position += (up * velocity) / zoom; break;
	case Camera_Movement::ZOOM_POS:	zoom += zoom_vel; break;
	case Camera_Movement::ZOOM_NEG: zoom -= zoom_vel; break;
    }
    update(zoomfunc);
}

