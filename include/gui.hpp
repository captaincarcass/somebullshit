
#pragma once
#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include "shaderdef.hpp"
#include <unordered_map>

namespace ShaderGUI
{
enum class UniformType : int {
    UNKNOWN = 0,
    FLOAT,
    INT,
    BOOL,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4,
    SAMPLER2D,
    SAMPLER3D,
};

enum class ShaderType : int {
    UNKOWN = 0,
    STANDARD,
    POST_PROCESS,
};

struct UniformInfo {
    std::string name;
    std::string lowername;
    UniformType type;
    GLint location;

    //defaults i guess subject to change
    float min 	{ 0.0f };
    float max 	{ 1.0f };
    float speed { 0.01f };
    bool isColor{ false };
};

struct ShaderGUIState {
    inline static bool isShaderUniformGUI = true;
    inline static bool isShaderPickerGUI = false;
    inline static bool isShaderLoaderGUI = false;
};

extern struct ShaderGUIState shaderGUIState;

class ShaderUniformGUI {
public:
    static void Introspect(const Shader& shader. ShaderType type, const std::string& name);
    static void RenderGUI(Shader& shader);

private:
    struct ShaderData {
	std::string name;
	ShaderType type;
	Shader* shader;
	std::vector<UniformInfo> uniforms;
	std::unordered_map<std::string, float> floatValues; 
	std::unordered_map<std::string, int> intValues; 
	std::unordered_map<std::string, bool> boolValues; 
	std::unordered_map<std::string, glm::vec2> vec2Values; 
	std::unordered_map<std::string, glm::vec3> vec3Values; 
	std::unordered_map<std::string, glm::vec4> vec4Values; 
    };

    inline static std::unordered_map<uint32_t, ShaderData> shaderData {};
    inline static constexpr const char* KEY = "shaderInfo"
    static void RenderShaderSection(const std::string& name, ShaderType type);
    static void RenderUniformsForShader(uint32_t shaderID, ShaderData& data);
    static UniformType GLTypeToUniformType(GLenum glType);
    static void InitDefaults(const UniformInfo& info);
    static void SaveToConfig();
    static void Init();
};

class ShaderPickerGUI {
public:
    static void RenderGUI();

    inline static std::unordered_map<uint64_t, std::string> names;
private:
    inline static constexpr const char* KEY = "shaders";
    inline static std::unordered_map<std::string, std::string> vertexPaths;
    inline static std::unordered_map<std::string, std::string> fragmentPaths;
    inline static std::unordered_map<std::string, std::string> combinedPaths;
    inline static uint64_t counter {} ;
    static bool isPathUnique(std::string path, Type type);
    static bool isPathUnique(std::string path);
    static bool isNameUnique(std::string name);
    static void SaveToConfig();
    static bool Init();
    enum class Type { VERTEX, FRAGMENT, COMBINED };
};

class ShaderLoaderGUI {
public:
    void RenderGUI();

};
}
