#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;

void main(){
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aPos.xy * 0.5 + 0.5;
}

#shader fragment
#version 460 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D screenTexture;
uniform float blurRadius;  // Default: 1.0

const float kernel[81] = float[](
	0.0000,	0.0000,	0.0000,	0.0001,	0.0001,	0.0001,	0.0000,	0.0000,	0.0000,
	0.0000,	0.0000,	0.0004,	0.0014,	0.0023,	0.0014,	0.0004,	0.0000,	0.0000,
	0.0000,	0.0004,	0.0037,	0.0146,	0.0232,	0.0146,	0.0037,	0.0004,	0.0000,
	0.0001,	0.0014,	0.0146,	0.0584,	0.0926,	0.0584,	0.0146,	0.0014,	0.0001,
	0.0001,	0.0023,	0.0232,	0.0926,	0.1466,	0.0926,	0.0232,	0.0023,	0.0001,
	0.0001,	0.0014,	0.0146,	0.0584,	0.0926,	0.0584,	0.0146,	0.0014,	0.0001,
	0.0000,	0.0004,	0.0037,	0.0146,	0.0232,	0.0146,	0.0037,	0.0004,	0.0000,
	0.0000,	0.0000,	0.0004,	0.0014,	0.0023,	0.0014,	0.0004,	0.0000,	0.0000,
	0.0000,	0.0000,	0.0000,	0.0001,	0.0001,	0.0001,	0.0000,	0.0000,	0.0000
);

void main() {
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    vec3 result = vec3(0.0);
    
    int radius = 3;
    int index = 0;
    
    for(int y = -radius; y <= radius; y++) {
        for(int x = -radius; x <= radius; x++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize * blurRadius;
            result += texture(screenTexture, TexCoord + offset).rgb * kernel[index];
            index++;
        }
    }
    
    FragColor = vec4(result, 1.0);
}

#shader end
