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

namespace sg
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

    //defaults
    float min 	{ 0.0f };
    float max 	{ 1.0f };
    float speed { 0.01f };
    bool isColor{ false };
};

struct State {
    inline static bool isShaderUniformGUI = false;
    inline static bool isShaderPickerGUI = false;
    inline static bool isShaderLoaderGUI = false;
};

class Manager {
private:
    Manager() = default;
    ~Manager() = default;
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    std::unordered_map<std::string, ShaderData> shaderDataStore;
public:
    static Manager& Instance();
    bool InitFromConfig();
    void Shutdown();
    bool Compile(const std::string& name);
    void Remove(const std::string& name);
    void SetActivity(const std::string& name, bool status);
    bool isActive(const std::string& name) const;
    bool isCompiled(const std::string& name) const;
    bool RegisterPaths(const std::string& name, const Paths& paths);
    bool RegisterName(const std::string& name);
    Paths* GetRegisteredPaths(const std::string& name);
    ShaderData* GetShaderData(const std::string& name);
    std::vector<Paths*> GetAllRegisteredPaths();
    std::vector<ShaderData*> GetActiveShaders();
    std::vector<std::string*> GetActiveShaderNames();
    std::vector<ShaderData*> GetCompiledShaders();
    std::vector<ShaderData*> GetShadersByType(ShaderType type);
    bool isNameUnique(const std::string& name) const;
    bool isPathUnique(const std::string& path) const;
    void SaveUniformData(const std::string& name, const UniformData& data);
    void SaveUniformInfo(const std::string& name, const UniformInfo& info);
    UniformType GLTypeToUniformType(GLenum glType);
    struct Paths {
	Paths(std::string combPath, 
	      std::string vertPath, 
	      std::string fragPath);
	std::string combinedPath;	
	std::string vertexPath;
	std::string fragmentPath;
    };
    struct UniformData {
	std::unordered_map<std::string, float> floatValues; 
	std::unordered_map<std::string, int> intValues; 
	std::unordered_map<std::string, bool> boolValues; 
	std::unordered_map<std::string, glm::vec2> vec2Values; 
	std::unordered_map<std::string, glm::vec3> vec3Values; 
	std::unordered_map<std::string, glm::vec4> vec4Values;
    };
    struct ShaderData {
	std::string name;
	std::unique_ptr<Shader> shader;
	ShaderType type;
	bool isCompiled;
	bool isActive = false;
	bool isCombined;
	std::vector<UniformInfo> uniformsInfo;
	Paths paths;
	UniformData uniformData;
    };
};

class Picker {
private:
public:
    static void Draw();
};

class Loader {
private:
public:
    static void Draw();
};

class Setter {
private:
    static void Introspect(const std::string& name);
    static void InitDefaults(const UniformInfo& info);
    static void RenderShaderSection(const std::string& name, ShaderType type);
    static void RenderUniformsForShader(const Manager::ShaderData* data);
public:
    static void Draw();
};
}
