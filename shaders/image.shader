#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aColor;

uniform float aspect;

out vec2 TexCoord;

void main() {
    vec2 pos = aPos.xy;
    pos.y *= aspect;
    gl_Position = vec4(pos, aPos.z , 1.0);
    TexCoord = aTex;
}

#shader fragment
#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}

#shader end
