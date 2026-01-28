
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "primitives.hpp"


float PI {3.14159265f};

void genSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectors, int stacks){
    vertices.clear();
    indices.clear();

    for(int i{0}; i<=stacks; ++i){
	float v { (float)i / stacks };    
	float theta { v * PI };
	for(int j{0}; j<=sectors; ++j){
	    float u { (float)j / sectors };
	    float phi { u * 2.0f * PI };

	    float x { radius * std::sin(theta) * std::cos(phi) };
	    float y { radius * std::cos(theta) };
	    float z { radius * std::sin(theta) * std::sin(phi) };
	    
	    glm::vec3 norm = glm::normalize(glm::vec3(x,y,z));
	    glm::vec3 dhat = -norm;

	    vertices.push_back(x);
	    vertices.push_back(y);
	    vertices.push_back(z);

	    vertices.push_back(norm.x);
	    vertices.push_back(norm.y);
	    vertices.push_back(norm.z);

	    vertices.push_back((x/radius + 1.0f)/2.0f); // r
	    vertices.push_back((y/radius + 1.0f)/2.0f); // g
	    vertices.push_back((z/radius + 1.0f)/2.0f); // b
	}	
    }

    for(int i = 0; i < stacks; ++i) {
	for(int j = 0; j < sectors; ++j) {
	    int first  = i * (sectors + 1) + j;
	    int second = first + (sectors + 1);

	    indices.push_back(first);
	    indices.push_back(second);
	    indices.push_back(first + 1);

	    indices.push_back(first + 1);
	    indices.push_back(second);
	    indices.push_back(second + 1);
	}
    }
}

void genPlane(std::vector<float>& vertices, std::vector<unsigned int>& indices, float sidelength, int resolution){
    vertices.clear();
    indices.clear();

    for(int i{}; i<=resolution; ++i){
	for(int j{}; j<=resolution; ++j){
	    float x { ( (j / (float)resolution ) - 0.5f) * sidelength };
	    float y { ( (i / (float)resolution ) - 0.5f) * sidelength };
	    float z { 0 };

	    vertices.push_back(x);
	    vertices.push_back(y);
	    vertices.push_back(z);

	    vertices.push_back(0.0f);
	    vertices.push_back(0.0f);
	    vertices.push_back(1.0f);

	    float u { j / (float)resolution };
	    float v { i / (float)resolution };
	    vertices.push_back(u);
	    vertices.push_back(v);

	    float r { j / (float)resolution };
	    float g { i / (float)resolution };
	    float b { 0.5f };

	    vertices.push_back(r);
	    vertices.push_back(g);
	    vertices.push_back(b);
	}
    }

    for(int i = 0; i < resolution; ++i) {
	for(int j = 0; j < resolution; ++j) {
	    int first  = i * (resolution + 1) + j;
	    int second = first + (resolution + 1);

	    indices.push_back(first);
	    indices.push_back(second);
	    indices.push_back(first + 1);

	    indices.push_back(first + 1);
	    indices.push_back(second);
	    indices.push_back(second + 1);
	}
    }

}

