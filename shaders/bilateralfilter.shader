#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aPos.xy * 0.5 + 0.5;
}

#shader fragment
#version 460 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D screenTexture;
uniform int radius;
uniform float sigmaSpacial;
uniform float sigmaIntensity;

float pi = 3.14159265;

float gaussian(in float x, in float sigma){
    return (exp(-(x*x) / (2.0 * sigma*sigma)) / (2 * pi * sigma*sigma));
}

void main(){
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    vec3 result = vec3(0.0);
    vec3 texCol = texture(screenTexture, TexCoord).rgb;

    vec3 weight = vec3(0.0);
    for (int y = -radius; y <= radius; y++){
	for(int x = -radius; x <= radius; x++){
	    vec2 offset = vec2(float(x), float(y)) * texelSize;
	    vec3 newTexCol = texture(screenTexture, TexCoord + offset).rgb;

	    float spacialDiff = sqrt(x*x + y*y); 
	    float spacialFilter = gaussian(spacialDiff, sigmaSpacial);

	    vec3 intensityDiff = abs(texCol - newTexCol);
	    vec3 intensityFilter = vec3(gaussian(intensityDiff.x, sigmaIntensity), gaussian(intensityDiff.y, sigmaIntensity), gaussian(intensityDiff.z, sigmaIntensity));

	    weight += spacialFilter * intensityFilter;
	    result += newTexCol * spacialFilter * intensityFilter;
	}
    }

    FragColor = vec4(result/weight, 1.0);
}
#shader end
