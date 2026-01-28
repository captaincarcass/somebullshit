#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 3) in vec3 aColor;

out vec3 vertexColor;
out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 lightPos;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = vec3(0.99, 0.99, 0.99);
    Normal = normalMatrix * aNorm;
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    LightPos =  vec3(view * vec4(lightPos, 1));
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 vertexColor;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform vec3 lightColor;
uniform vec3 cameraPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 viewDir = normalize( -FragPos);
    vec3 reflection = reflect(-lightDir, norm);

    float phong = 128.0f;
    float reflectionAngle = max(dot(viewDir, reflection),0);
    float spec = pow(reflectionAngle, phong);
    float diffuse = max(dot(norm, lightDir),0);
    float ambience = 1.0;

    vec3 lambertian = lightColor * diffuse;
    vec3 ambient = lightColor * ambience;
    vec3 specular = lightColor * spec;
    vec3 resultColor = (ambient) * vertexColor;
    FragColor = vec4(resultColor , 1);
}
#shader end
