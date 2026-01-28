
#include <memory>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include "shaderdef.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    std::string vertexCode {loadFile(vertexPath)};
    std::string fragmentCode {loadFile(fragmentPath)};
	
    const char* vertexShader {vertexCode.c_str()};
    const char* fragmentShader {fragmentCode.c_str()};

    GLuint vertex {compile(GL_VERTEX_SHADER, vertexShader)};
    GLuint fragment {compile(GL_FRAGMENT_SHADER, fragmentShader)};

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

Shader::Shader(const char* path){
    ID = glCreateProgram();
    const std::string combinedCode	{loadFile(path)};
    findShaders(combinedCode);

    GLuint vertex;
    GLuint fragment;
    
    if (vertexExists){
	std::string vertexCode	{extractShader(GL_VERTEX_SHADER, combinedCode)};
	const char* vertexShader{vertexCode.c_str()};
	GLuint vertex 		= compile(GL_VERTEX_SHADER, vertexShader);
	glAttachShader(ID, vertex);
    }

    if (fragmentExists){
        std::string fragmentCode	{extractShader(GL_FRAGMENT_SHADER, combinedCode)};
	const char* fragmentShader	{fragmentCode.c_str()}; 
	 GLuint fragment		= compile(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(ID, fragment);
    }

    glLinkProgram(ID);
    checkErrors(ID, "PROGRAM");

    if (vertexExists) glDeleteShader(vertex);
    if (fragmentExists) glDeleteShader(fragment);
}

Shader::~Shader(){
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}
void Shader::setBool(const std::string& name, bool value) const {         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string& name, int value) const { 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setFloat(const std::string& name, float value) const { 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
} 
void Shader::setVec2(const std::string& name, const glm::vec2& value) const { 
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec2(const std::string& name, float x, float y) const { 
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const { 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const { 
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
}
void Shader::setVec4(const std::string& name, const glm::vec4& value) const { 
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{ 
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
}
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkErrors(GLuint object, const std::string& type){
    int success{};
    char infoLog[1024];
    bool isProgram = (type == "PROGRAM");
    if (isProgram) {
	glGetProgramiv(object, GL_LINK_STATUS, &success);
    } else {
	glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    }
    if (success) {
	return;
    }
    if (isProgram) {
	glGetProgramInfoLog(object, 1024, nullptr, infoLog);
	throw std::runtime_error("PROGRAM LINK ERROR:\n" + std::string(infoLog));
    } else {
        glGetShaderInfoLog(object, 1024, nullptr, infoLog);
	throw std::runtime_error("SHADER COMPILE ERROR:\n" + std::string(infoLog));
    }
}
std::string Shader::loadFile(const char* path){
    try{
	std::ifstream file(path);
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream buffer{};
        buffer << file.rdbuf();
	file.close();
        return buffer.str();
    } catch(const std::ifstream::failure& e) {
	std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
        return {};
    }
}

void Shader::findShaders(const std::string &src){
    vertexExists = src.find("#shader vertex") != std::string::npos;
    fragmentExists = src.find("#shader fragment") != std::string::npos;
}

std::string Shader::extractShader(GLenum type, const std::string &src){
    size_t start;
    size_t end;
    // std::cout << src << std::endl;
    switch(type){
	case GL_VERTEX_SHADER: {
	    const std::string token {"#shader vertex"};
	    start = src.find(token); 
	    start += token.length();
	    end = src.find("#shader", start + 1);
	    // std::cout << src.substr(start, end - start) << std::endl;
	    return src.substr(start, end - start);
	}
	case GL_FRAGMENT_SHADER: {
	    const std::string token {"#shader fragment"};
	    start = src.find(token); 
	    start += token.length();
	    end = src.find("#shader", start + 1);
	    // std::cout << src.substr(start, end - start) << std::endl;
	    return src.substr(start, end - start);
	}
    }
    return std::string("invalid shader type");
}

GLuint Shader::compile(GLenum type, const char* source){
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    checkErrors(shader, "SHADER");
    return(shader);
}



