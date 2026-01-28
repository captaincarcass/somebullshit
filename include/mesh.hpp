#pragma once

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>


class Mesh {
public:
    GLuint VAO{}, VBO{}, EBO{};
    GLsizei indexCount{};
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, bool hasNormals = false, bool hasTexCoords = false, bool hasColor = false);
    void draw() const;
    void drawLine() const;
private:
    void setupMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, bool hasNormals, bool hasTexCoords, bool hasColor);
};

class Graph {
public:
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float lineWidth{0.005f};
    float Step{0.0001f};

    Mesh linePlotThick(float (*f)(float), float xmin, float xmax, float step = 0.0001f);
    Mesh linePlotThin(float (*f)(float), float xmin, float xmax, float step = 0.0001f);
    bool isCont(float (*f)(float), float x, float step = 0.0001f, float threshold = 100.0f);
    float derivative(float (*f)(float), float x, float step = 0.0001f);
private:
};


