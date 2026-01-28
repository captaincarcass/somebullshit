#pragma once
#include <cmath>
#include <vector>

void genSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectors, int stacks);
void genPlane(std::vector<float>& vertices, std::vector<unsigned int>& indices, float sidelength, int resolution);
