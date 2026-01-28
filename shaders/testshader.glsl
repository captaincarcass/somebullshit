#shader vertex

#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = vec3(1.0, 1.0, 1.0);
}

#shader fragment

#version 460 core

out vec4 FragColor;

in vec3 vertexColor;

void main(){
    FragColor = vec4(vertexColor, 1.0);
}

#shader end
