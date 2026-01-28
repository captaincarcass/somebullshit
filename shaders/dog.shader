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
uniform float sigmaBig;
uniform float sigmaSmall;
uniform float tau;
uniform float phi;
uniform float threshold;
#define pi 3.14159265

float gaussian(in float x, in float sigma){
    return (exp(-(x*x) / (2.0 * sigma*sigma)) / (2 * pi * sigma*sigma));
}

void main(){
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    vec3 result = vec3(0.);
    vec3 convBig = vec3(0.);
    vec3 convSmall = vec3(0.);

    vec3 weight = vec3(0.0);
    for (int y = -radius; y <= radius; y++){
	for(int x = -radius; x <= radius; x++){
	    vec2 offset = vec2(float(x), float(y)) * texelSize;
	    vec3 texCol = texture(screenTexture, TexCoord + offset).rgb;

	    float texLum = dot(texCol, vec3(0.2126, 0.7152, 0.0722));

	    float spacialDiff = sqrt(x*x + y*y); 
	    float bigBlur = gaussian(spacialDiff, sigmaBig);
	    float smallBlur = gaussian(spacialDiff, sigmaSmall);

	    convBig += bigBlur * texCol;
	    convSmall += smallBlur * texCol;
	}
    }
    vec3 diff = (1+tau)*convSmall - tau*convBig;
    (length(diff) >= threshold) ? diff *= vec3(0.9) : diff *= vec3(0.9 + tanh(phi*(diff - threshold)));
    // diff >= threshold ? diff = 1.0 : diff = 0.0;

    result = diff;
    
    FragColor = vec4(vec3(result), 1.0f);
}

#shader end

