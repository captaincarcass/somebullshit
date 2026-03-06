#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <glad/glad.h>
#include <nlohmann/json>
#include <fstream>
#include <type_traits>
#include "shaderdef.hpp"
#include "gui.hpp"
#include "config_manager.hpp"

#define PI 3.1415926535
#define TAU 6.2831853071

using json = nlohmann::json;

namespace sg	
{
Manager& Manager::Instance() {
    static Manager instance;
    return instance;
}

bool Manager::InitFromConfig() {
    auto& config = ConfigManager::GetConfig();
    if (config.empty()) return false;

    std::vector<ShaderData> tempShaderDataStore(config.size());

    int i {};
    for (auto& [name, data] : config.items()) {
	tempShaderDataStore[i].name = name;
	tempShaderDataStore[i].type = data.value("type", static_cast<ShaderType>(0));
	tempShaderDataStore[i].isActive = data.value("isActive", false);
	tempShaderDataStore[i].isCombined = data.value("isCombined", true);
	
	for (auto& info : data["uniformsInfo"]) {
	    tempShaderDataStore[i].uniformsInfo.emplace_back(
		UniformInfo{
		    .name = info.value("name", std::string{}),
		    .type = static_cast<UniformType>(info.value("type", 0)),
		    .min = info.value("min", 0.0f),
		    .max = info.value("max", 1.0f),
		    .speed = info.value("speed", 0.01f),
		    .isColor = info.value("isColor", false)
		});
	}
	tempShaderDataStore[i].paths.combinedPath = data.value("combinedPath", decltype(tempShaderDataStore[i].paths.combinedPath){});
	tempShaderDataStore[i].paths.vertexPath = data.value("vertexPath", decltype(tempShaderDataStore[i].paths.vertexPath){});
	tempShaderDataStore[i].paths.fragmentPath = data.value("fragmentPath", decltype(tempShaderDataStore[i].paths.fragmentPath){});
	tempShaderDataStore[i].uniformData.floatValues = data.value("floatValues", decltype(tempShaderDataStore[i].uniformData.floatValues){});
	tempShaderDataStore[i].uniformData.intValues = data.value("intValues", decltype(tempShaderDataStore[i].uniformData.intValues){});
	tempShaderDataStore[i].uniformData.boolValues = data.value("boolValues", decltype(tempShaderDataStore[i].uniformData.boolValues){});
	tempShaderDataStore[i].uniformData.vec2Values = data.value("vec2Values", decltype(tempShaderDataStore[i].uniformData.vec2Values){});
	tempShaderDataStore[i].uniformData.vec3Values = data.value("vec3Values", decltype(tempShaderDataStore[i].uniformData.vec3Values){});
	tempShaderDataStore[i].uniformData.vec4Values = data.value("vec4Values", decltype(tempShaderDataStore[i].uniformData.vec4Values){});
	shaderDataStore[name] = std::move(tempShaderDataStore[i]);
	++i;
    }
    return true;
}

void Manager::Shutdown() {
    auto& config = ConfigManager::GetConfig();

    std::vector<ShaderData> tempShaderDataStore {};
    tempShaderDataStore.reserve(shaderDataStore.size());

    for (auto& [name, data] : shaderDataStore) {
	tempShaderDataStore.emplace_back(data);
    }

for (auto& data : tempShaderDataStore) {
	config[data.name]["type"] = std::to_underlying(data.type);
	config[data.name]["isActive"] = data.isActive;
	config[data.name]["isCombined"] = data.isCombined;
	
	json uniformsArray = json::array();
	for (auto& uniform : data.uniformsInfo) {
	    uniformsArray.push_back({
		{"name", uniform.name},
		{"type", uniform.type},
		{"min", uniform.min},
		{"max", uniform.max},
		{"speed", uniform.speed},
		{"isColor", uniform.isColor},
	    });
	}
	config[data.name]["uniformsInfo"] = uniformsArray;
	config[data.name]["combinedPath"] = data.paths.combinedPath;
	config[data.name]["vertexPath"] = data.paths.vertexPath;
	config[data.name]["fragmentPath"] = data.paths.fragmentPath;
	config[data.name]["floatValues"] = data.uniformData.floatValues;
	config[data.name]["intValues"] = data.uniformData.intValues;
	config[data.name]["boolValues"] = data.uniformData.boolValues;
	config[data.name]["vec2Values"] = data.uniformData.vec2Values;
	config[data.name]["vec3Values"] = data.uniformData.vec3Values;
	config[data.name]["vec4Values"] = data.uniformData.vec4Values;
    }

    ConfigManager::Save();
    shaderDataStore.clear();
}

bool Manager::Compile(const std::string& name) {
    if (!shaderDataStore.contains(name)) return false;

    auto& shader = shaderDataStore[name];
    
    if(shader.isCombined) {
	shader.shader = std::make_unique<Shader>(shader.paths.combinedPath.c_str());
    } else {
	shader.shader = std::make_unique<Shader>(shader.paths.vertexPath.c_str(), shader.paths.fragmentPath.c_str());
    }
    shaderDataStore[name].isCompiled = true;
    return true;
}

bool Manager::Remove(const std::string& name) {
    if (!shaderDataStore.contains(name)) return false;
    shaderDataStore.erase(name);
    return true;
}

bool Manager::SetActivity(const std::string& name, bool status) {
    if (!shaderDataStore.contains(name)) return false;
    shaderDataStore[name].isActive = status;
    return true;
}

bool Manager::isActive(const std::string& name) {
    if (!shaderDataStore.contains(name)) return false;
    return shaderDataStore[name].isActive;
}

bool Manager::isCompiled(const std::string& name) {
    if (!shaderDataStore.contains(name)) return false;
    return shaderDataStore[name].isCompiled;
}

ShaderData* Manager::GetShaderData(const std::string& name) {
    if (!shaderDataStore.contains(name)) return nullptr;
    return &shaderDataStore[name];
}

bool Manager::RegisterName(const std::string& name) {
    if (shaderDataStore.contains(name)) return false;
    shaderDataStore[name] = name;
    return true;
}

bool Manager::RegisterPaths(const std::string& name, const Paths& paths) {
    if (!shaderDataStore.contains(name)) return false;
    shaderDataStore[name].paths = paths;
    return true;
}

Paths* Manager::GetRegisteredPaths(const std::string& name) {
    if (!shaderDataStore.contains(name)) return nullptr;
    return &shaderDataStore[name].paths;
}

std::vector<Paths*> Manager::GetAllRegisteredPaths() {
    std::vector<Paths*> paths {};
    for (auto& [name, data] : shaderDataStore) {
	paths.emplace_back(&data.paths);
    }
    return paths;
}

std::vector<ShaderData*> Manager::GetActiveShaders() {
    std::vector<ShaderData*> shaders {};
    for (auto& [name, data] : shaderDataStore) {
	if (data.isActive) {
	    shaders.emplace_back(data);
	}
    }
    return shaders;
}

std::vector<std::string*> Manager::GetActiveShaderNames() {
    std::vector<std::string*> shaders {};
    for (auto& [name, data] : shaderDataStore) {
	if (data.isActive) {
	    shaders.emplace_back(name);
	}
    }
    return shaders;
}

std::vector<ShaderData*> Manager::GetCompiledShaders() {
    std::vector<ShaderData*> shaders {};
    for (auto& [name, data] : shaderDataStore) {
	if (data.isCompiled) {
	    shaders.emplace_back(data);
	}
    }
    return shaders;
}

std::vector<ShaderData*> Manager::GetShadersByType(ShaderType type) {
    std::vector<ShaderData*> shaders {};
    for (auto& [name, data] : shaderDataStore) {
	if (data.type == type) {
	    shaders.emplace_back(data);
	}
    }
    return shaders;
}

bool Manager::isNameUnique(const std::string& name) const {
    return !shaderDataStore.contains(name);
}

bool Manager::isPathUnique(const std::string& path) const {
    bool isUnique {true};
    for (auto& [name, data] : shaderDataStore) {
	if (data.paths.combinedPath == path) isUnique = false;
	if (data.paths.vertexPath == path) isUnique = false;
	if (data.paths.fragmentPath == path) isUnique = false;
    }
    return isUnique;
}

void Manager::SaveUniformData(const std::string& name, const UniformData& data) {
    if (!shaderDataStore.contains(name)) return;
    shaderDataStore[name].uniformData = data;
    return;
}

void Manager::SaveUniformInfo(const std::string& name, const UniformInfo& info) {
    if (!shaderDataStore.contains(name)) return;
    auto& uniforms = shaderDataStore[name].uniformsInfo;

    for (u : uniforms) {
	if(u.name == info.name) {
	    u = info;
	    return;
	}
    }
    uniforms.push_back(info);
    return;
}

UniformType Manager::GLTypeToUniformType(GLenum glType) {
    switch (glType) {
	case GL_FLOAT: return UniformType::FLOAT; break;
	case GL_INT: return UniformType::INT; break;
	case GL_BOOL: return UniformType::BOOL; break;
	case GL_FLOAT_VEC2: return UniformType::VEC2; break;
	case GL_FLOAT_VEC3: return UniformType::VEC3; break;
	case GL_FLOAT_VEC4: return UniformType::VEC4; break;
	case GL_FLOAT_MAT2: return UniformType::MAT2; break;
	case GL_FLOAT_MAT3: return UniformType::MAT3; break;
	case GL_FLOAT_MAT4: return UniformType::MAT4; break;
	case GL_SAMPLER_2D: return UniformType::SAMPLER2D; break;
	default: return UniformType::UNKNOWN; break;
    }
}

void Manager::Paths::Paths(std::string combPath, 
			   std::string vertPath, 
			   std::string fragPath)
    : combinedPath(std::move(combPath)), 
      vertexPath(std::move(vertPath)), 
      fragmentPath(std::move(fragPath))
{
}

void Picker::Draw() {
    if (!State::isShaderPickerGUI) return;

    auto& manager = Manager::Instance();

    static bool isCombined { true };
    static std::string tempName;
    static std::string tempCombinedPath;
    static std::string tempVertexPath;
    static std::string tempFragmentPath;

    ImGui::Begin("Add Shader");
    ImGui::InputText("Shader Name", &tempName);
    ImGui::Checkbox("Combined Shader File?", &isCombined);

    bool canSave { !tempName.empty() };
    static bool validPath {};

    validPath = manager.isNameUnique();

    if ( isCombined ) {
	ImGui::InputText("Shader Path", &tempCombinedPath);
	canSave &= !tempCombinedPath.empty();
	validPath = manager.isPathUnique(tempCombinedPath);
	canSave &= validPath;
    } else {
	ImGui::InputText("Vertex Shader Path", &tempVertexPath);
	ImGui::InputText("Fragment Shader Path", &tempFragmentPath);
	canSave &= !tempVertexPath.empty() && !tempFragmentPath.empty();
	validPath = manager.isPathUnique(tempVertexPath) && 
		    manager.isPathUnique (tempFragmentPath,);
	canSave &= validPath;
    }

    !validPath ? ImGui::Text("This shader already exists") : true;

    ImGui::BeginDisabled(!canSave);
    bool saveClicked = ImGui::Button("Save Shader");
    ImGui::EndDisabled();

    Manager::Paths paths(tempCombinedPath, tempVertexPath, tempFragmentPath);
    if ( saveClicked && canSave ) {
	if (manager.RegisterName(tempName)) {} else std::cerr << "dont do that\n";
	if (manager.RegisterPaths(tempName, paths)) {} else std::cerr << "dont do that\n";

	tempName.clear();
	tempCombinedPath.clear();
	tempVertexPath.clear();
	tempFragmentPath.clear();
	State::isShaderPickerGUI = false;
    }
    ImGui::End();
}

void Loader::Draw() {
    if (!State::isShaderLoaderGUI) return;

    auto& manager = Manager::Instance();

    ImGui::Begin("Load/Unload Shaders")

    const char* currentShader = nullptr;

    bool isSelected {};
    bool isActive {};
    bool showConfirmationButton {};

    if(ImGui::BeginCombo("Load/Unload Shaders", currentShader)) {
	ImGui::SetItemDefaultFocus();
	for (const auto& shader : GetActiveShaderNames()) {
	    isSelected = (currentShader == shader);
	    if (ImGui::Selectable(shader, isSelected)) {
		currentShader = shader;
		

		manager.SetActivity(shader, ImGui::Checkbox("Active", &isActive));
    
		if (manager.isCompiled(shader)) {
		    if (ImGui::Button("Recompile Shader")) manager.Compile(shader);
		} else {
		    if (ImGui::Button("Compile Shader")) manager.Compile(shader);
		}

		if (ImGui::Button("Remove Shader")) {
		    showConfirmationButton = true;
		    if (showConfirmationButton) {
			if (ImGui::Button("Are you really sure?")) {
			    manager.Remove(shader);
			} else if (ImGui::Button("Get me the fuck out of here")) {
			    showConfirmationButton = false;
			}
		    }
		}

	    }
	}
	ImGui::EndCombo();
    }
}

void Setter::Introspect(const std::string& name) {
    
    auto& manager = Manager::Instance();

    GLuint shaderID = manager.GetShaderData(name)->shader->ID;
    
    GLint count {};
    glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &count);

    GLchar name[256];
    GLsizei length;
    GLint size;
    GLenum type;
    UniformInfo UniformInfo;

    for (GLint i = 0; i < count; ++i) {
	glGetActiveUniform(shaderID, i, sizeof(name), &length, &size, &type, name);

	UniformInfo.name = std::string(name);
	UniformInfo.type = manager.GLTypeToUniformType(type);
	UniformInfo.location = glGetUniformLocation(shaderID, name);

	if (UniformInfo.location == -1 ||
	    UniformInfo.type == UniformType::SAMPLER2D ||
	    UniformInfo.type == UniformType::SAMPLER3D) {
	    continue;
	}
	
	std::string lowercase = UniformInfo.name;

	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);

	//smart defaults
	if (lowercase.find("color") != std::string::npos || 
	    lowercase.find("colour") != std::string::npos) {
	    UniformInfo.isColor = true;
	    UniformInfo.min = 0.0f;
	    UniformInfo.max = 1.0f;
	} else if (lowercase.find("position") != std::string::npos || 
		   lowercase.find("pos") != std::string::npos) {
	    UniformInfo.min = -10.0f;
	    UniformInfo.max = 10.0f;
	    UniformInfo.speed = 0.1f;
	} else if (lowercase.find("scale") != std::string::npos) {
	    UniformInfo.min = 0.0f;
	    UniformInfo.max = 5.0f;
	    UniformInfo.speed = 0.01f;
	} else if (lowercase.find("angle") != std::string::npos || 
		   lowercase.find("rotation") != std::string::npos) {
	    UniformInfo.min = 0.0f;
	    UniformInfo.max = TAU;
	    UniformInfo.speed = 0.01f;
	} else if (lowercase.find("alpha") != std::string::npos || 
		   lowercase.find("opacity") != std::string::npos ||
		   lowercase.find("roughness") != std::string::npos ||
		   lowercase.find("metallic") != std::string::npos) {
	    UniformInfo.min = 0.0f;
	    UniformInfo.max = 1.0f;
	} else {
	    // Generic defaults
	    UniformInfo.min = -1.0f;
	    UniformInfo.max = 1.0f;
	    UniformInfo.speed = 0.01f;
	}
	manager.SaveUniformInfo(name, UniformInfo);
	InitDefaults(UniformInfo);
    }
}

void Setter::InitDefaults(const UniformInfo& info) {
    switch (info.type) {
	case UniformType::FLOAT:
	    if (floatValues.find(info.name) == floatValues.end()) {
		floatValues[info.name] = { 1.0f };
	    }
	    break;
	case UniformType::INT:
	    if (intValues.find(info.name) == intValues.end()) {
		intValues[info.name] = { 1 };
	    }
	    break;
	case UniformType::BOOL:
	    if (boolValues.find(info.name) == boolValues.end()) {
		boolValues[info.name] = { false };
	    }
	    break;
	case UniformType::VEC2:
	    if (vec2Values.find(info.name) == vec2Values.end()) {
		vec2Values[info.name] = { glm::vec2(0.0f) };
	    }
	    break;
	case UniformType::VEC3:
	    if (vec3Values.find(info.name) == vec3Values.end()) {
		vec3Values[info.name] = {
		    info.isColor ? glm::vec3(1.0f) : glm::vec3(0.0f)
		};
	    }
	    break;
	case UniformType::VEC4:
	    if (vec4Values.find(info.name) == vec4Values.end()) {
		vec4Values[info.name] = {
		    info.isColor ? glm::vec4(1.0f) : glm::vec4(0.0f)
		};
	    }
	    break;
    }
}

void Setter::RenderShaderSection(const std::string& headerName, ShaderType type) {

    auto& manager = Manager::Instance();

    std::vector<std::string> shadersOfThisType;

    for (auto* data : manager.GetShadersByType(type)) {
	if (data->type == type) {
	    shadersOfThisType.emplace_back(data->name);
	}
    }

    if (shadersOfThisType.empty()) return;
    
    if (ImGui::CollapsingHeader(headerName.c_str())) {
	for (auto& name : shadersOfThisType) {
	    auto* data = manager.GetShaderData(name);

	    if (ImGui::TreeNode(data->name.c_str())) {
		RenderUniformsForShader(data);
		ImGui::TreePop();
	    }
	}
    }
}

void ShaderGUI::ShaderUniformGUI::RenderUniformsForShader(const Manager::ShaderData& data) {
    data->shader->use();

    if (data->uniformsInfo.empty()) {
	ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No uniforms detected");
    }

    for (auto& UniformInfo : data->uniformsInfo) {
	bool changed = false;
	
	//NEEDS FIXING FROM HERE DOWN
	//I HAVE NO IDEA WHAT IS HAPPENING HERE
	switch (UniformInfo.type) {
	    case UniformType::FLOAT: {
		float& val = floatValues[UniformInfo.name];
		changed = ImGui::SliderFloat(UniformInfo.name.c_str(), 
					&val, UniformInfo.min, 
					UniformInfo.max, "%.3f");
		if (changed) data.shader.setFloat(UniformInfo.name, val);
		break;
	    }
	    
	    case UniformType::INT: {
		int& val = intValues[UniformInfo.name];
		changed = ImGui::SliderInt(UniformInfo.name.c_str(), 
					&val, (int)UniformInfo.min, 
					(int)UniformInfo.max);
		if (changed) data.shader.setInt(UniformInfo.name, val);
		break;
	    }
	    
	    case UniformType::BOOL: {
		bool& val = boolValues[UniformInfo.name];
		changed = ImGui::Checkbox(UniformInfo.name.c_str(), &val);
		if (changed) data.shader.setBool(UniformInfo.name, val);
		break;
	    }
	    
	    case UniformType::VEC2: {
		glm::vec2& val = vec2Values[UniformInfo.name];
		changed = ImGui::SliderFloat2(UniformInfo.name.c_str(), 
					    &val.x, UniformInfo.min, 
					    UniformInfo.max, "%.2f");
		if (changed) data.shader.setVec2(UniformInfo.name, val);
		break;
	    }
	    
	    case UniformType::VEC3: {
		glm::vec3& val = vec3Values[UniformInfo.name];
		if (UniformInfo.isColor) {
		    changed = ImGui::ColorEdit3(UniformInfo.name.c_str(), 
					    &val.x, ImGuiColorEditFlags_Float);
		} else {
		    changed = ImGui::SliderFloat3(UniformInfo.name.c_str(), 
					    &val.x, UniformInfo.min, 
					    UniformInfo.max, "%.2f");
		}
		if (changed) data.shader.setVec3(UniformInfo.name, val);
		break;
	    }
	
	    case UniformType::VEC4: {
		glm::vec4& val = vec4Values[UniformInfo.name];
		if (UniformInfo.isColor) {
		    changed = ImGui::ColorEdit4(UniformInfo.name.c_str(), 
					    &val.x, ImGuiColorEditFlags_Float);
		} else {
		    changed = ImGui::SliderFloat4(UniformInfo.name.c_str(), 
					    &val.x, UniformInfo.min, 
					    UniformInfo.max, "%.2f");
		}
		if (changed) data.shader.setVec4(UniformInfo.name, val);
		break;
	    }
	
	    case UniformType::MAT2:
	    case UniformType::MAT3:
	    case UniformType::MAT4:
		ImGui::TextDisabled("%s (matrix - not editable)", UniformInfo.name.c_str());
		break;
	    default:
		ImGui::TextDisabled("%s (unsupported type)", UniformInfo.name.c_str());
		break;
	}

	if (ImGui::BeginPopupContextItem(UniformInfo.name.c_str())) {
	    ImGui::Text("Customize: %s", UniformInfo.name.c_str());
	    ImGui::Separator();
	    ImGui::DragFloat("Min", &UniformInfo.min, 0.1f);
	    ImGui::DragFloat("Max", &UniformInfo.max, 0.1f);
	    if (UniformInfo.type == UniformType::VEC3 || 
		UniformInfo.type == UniformType::VEC4) {
		ImGui::Checkbox("Is Color", &UniformInfo.isColor);
	    }
	    ImGui::EndPopup();
	}
    }
    ImGui::Separator();
    if (ImGui::Button("Reset All")) {
	for (auto& info : uniforms) {
	    InitDefaults(info);
	}
    }
}



}
