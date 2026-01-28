
#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "mesh.hpp"

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, bool hasNormals, bool hasTexCoords, bool hasColor){
    setupMesh(vertices, indices, hasNormals, hasTexCoords, hasColor);
}

void Mesh::setupMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, bool hasNormals, bool hasTexCoords, bool hasColor){
    indexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    int stride {3};
    if(hasNormals) stride += 3;
    if(hasTexCoords) stride += 2;
    if(hasColor) stride += 3;

    int offset {0};
    
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
    glEnableVertexAttribArray(0);
    offset += 3;

    if(hasNormals){
	glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(1);
	offset += 3;
    }
    if(hasTexCoords){
	glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(2);
	offset += 2;
    }
    if(hasColor){
	glVertexAttribPointer(3,3,GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(3);
    }


    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::drawLine() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_LINE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh Graph::linePlotThick(float (*f)(float), float xmin, float xmax, float step) {
    std::vector<float> vertices{};
    std::vector<unsigned int> indices{};

    for (float x = xmin; x <= xmax; x += step) {
        glm::vec3 linePos0 = {x, f(x), 0.0f};
        glm::vec3 linePos1 = {x + step, f(x + step), 0.0f};

        glm::vec3 lineDir = linePos1 - linePos0;
        glm::vec3 lineNormal = glm::normalize(glm::vec3(-lineDir.y, lineDir.x, 0.0f));
        glm::vec3 offset = 0.5f * lineWidth * lineNormal;

        glm::vec3 firstVertex = linePos0 + offset;
        glm::vec3 secondVertex  = linePos0 - offset;

        // Push vertices (2 per sample)
        vertices.push_back(firstVertex.x);
        vertices.push_back(firstVertex.y);
        vertices.push_back(firstVertex.z);

        vertices.push_back(secondVertex.x);
        vertices.push_back(secondVertex.y);
        vertices.push_back(secondVertex.z);
    }

    int N = (int)((xmax - xmin) / step); // number of segments
    for (int i = 0; i < N-1; i++) {
        int v0 = i * 2;
        int v1 = v0 + 1;
        int v2 = v0 + 2;
        int v3 = v0 + 3;

        // Triangles
        indices.push_back(v0);
        indices.push_back(v1);
        indices.push_back(v2);

        indices.push_back(v2);
        indices.push_back(v1);
        indices.push_back(v3);
    }

    return Mesh(vertices, indices);
}

Mesh Graph::linePlotThin(float (*f)(float), float xmin, float xmax, float step){
    std::vector<float> vertices{};
    std::vector<unsigned int> indices{};
    vertices.reserve( static_cast<int>(3 * ((xmax - xmin) / step)) );
    indices.reserve( static_cast<int>(((xmax-xmin) / step)) );

    int i{};
    for(float x = xmin; x <= xmax; x += step){
	vertices.push_back(x);
	vertices.push_back(f(x));
	vertices.push_back(0.0f);

	if (i > 0 && !isCont(f, x, step, 1000.0f)) indices.push_back(std::numeric_limits<unsigned int>::max());
	indices.push_back(i);
	++i;
    }
    std::cout << vertices.size() << std::endl;
    std::cout << indices.size() << std::endl;
    return Mesh(vertices, indices);
}

bool Graph::isCont(float (*f)(float), float x, float step, float threshold){
    return std::abs(derivative(f, x, step) - derivative(f, x + step, step)) < threshold;
}

float Graph::derivative(float (*f)(float), float x, float step){
    return (f(x+step) - f(x-step)) / (2 * step);
}

