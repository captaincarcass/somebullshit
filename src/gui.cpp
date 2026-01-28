
#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <glad/glad.h>
#include <json>
#include <fstream>
#include <type_traits>
#include "shaderdef.hpp"
#include "gui.hpp"
#include "config_manager.hpp"

#define PI 3.1415926535
#define TAU 6.2831853071

using json = nlohmann::json;

struct ShaderGUI::ShaderGUIState shaderGUIState = {};

void ShaderGUI::ShaderUniformGUI::Introspect(const Shader& shader, 
					     ShaderType type, 
					     const std::string& name) {
    uint32_t id = shader.ID;
    
    ShaderData& data = shaderData[id];
    data.name = name;
    data.type = type;
    data.shader = const_cast<Shader*>(&shader);
    data.uniforms.clear();
    
    GLint count {};
    glGetProgramiv(shader.ID, GL_ACTIVE_UNIFORMS, &count);

    GLchar name[256];
    GLsizei length;
    GLint size;
    GLenum type;
    UniformInfo UniformInfo;

    for (GLint i = 0; i < count; ++i) {
	glGetActiveUniform(shader.ID, i, sizeof(name), &length, &size, &type, name);

	UniformInfo.name = std::string(name);
	UniformInfo.type = GLTypeToUniformType(type);
	UniformInfo.location = glGetUniformLocation(shader.ID, name);

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
	data.uniforms.push_back(UniformInfo);
	InitDefaults(UniformInfo);
    }
}

void ShaderGUI::ShaderUniformGUI::RenderGUI(Shader& shader) {
    if ( !shaderGUIState.isShaderUniformGUI ) return;

    ImGui::Begin("Shader Uniforms");

    RenderShaderSection("Standard Shaders", ShaderType::STANDARD);
    RenderShaderSection("Post-Processing Shaders", ShaderType::POST_PROCESS);

    if (ImGui::Button("Save All Uniforms")) SaveToConfig();

    ImGui::End();
}

void ShaderGUI::ShaderUniformGUI::RenderShaderSection(const std::string& name, ShaderType type) {
    std::vector<uint32_t> shadersOfThisType;
    for (const auto& [id, data] : shaderData) {
	if (data.type == type) {
	    shadersOfThisType.push_back(id);
	}
    }

    if (shadersOfThisType.empty()) return;
    
    if (ImGui::CollapsingHeader(name.c_str())) {
	for (uint32_t id : shadersOfThisType) {
	    auto& data = shaderData[id];

	    if (ImGui::TreeNode(data.name.c_str())) {
		RenderUniformsForShader(id, data);
		ImGui::TreePop();
	    }
	}
    }
}

void ShaderGUI::ShaderUniformGUI::RenderUniformsForShader(uint32_t shaderID, ShaderData& data) {
    data.shader -> use();

    if (data.uniforms.empty()) {
	ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No uniforms detected");
    }

    for (auto& UniformInfo : data.uniforms) {
	bool changed = false;
	
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

//unordered maps are so weird why didnt they call it hash_map
ShaderGUI::UniformType ShaderGUI::ShaderUniformGUI::GLTypeToUniformType(GLenum glType) {
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

void ShaderGUI::ShaderUniformGUI::InitDefaults(const UniformInfo& info) {
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

void ShaderGUI::ShaderUniformGUI::SaveToConfig() {
    auto& config = ConfigManager::GetConfig();
    auto& key = config[KEY];

    for (const auto& [id, data] : shaderData) {
	key[data.name]["type"] = std::to_underlying(ShaderData.type);
	key[data.name]["floatValues"] = ShaderData.floatValues;
	key[data.name]["intValues"] = ShaderData.intValues;
	key[data.name]["boolValues"] = ShaderData.boolValues;
	key[data.name]["vec2Values"] = ShaderData.vec2Values;
	key[data.name]["vec3Values"] = ShaderData.vec3Values;
	key[data.name]["vec4Values"] = ShaderData.vec4Values;

	json uniformsArray = json::array();
	for ( const auto& uniform : data.uniforms) {
	    uniformsArray.push_back({
		{"name", uniform.name},
		{"type", uniform.type},
		{"min", uniform.min},
		{"max", uniform.max},
		{"speed", uniform.speed},
		{"isColor", uniform.isColor},
	    });
	}
	key[data.name]["uniforms"] = uniformsArray;
    }
}

void ShaderGUI::ShaderUniformGUI::Init() {
    auto& config = ConfigManager::GetConfig();
    if (!config.contains(KEY)) return;
    auto& key = config[KEY];

    for (const auto& [shaderName, shaderInfo] : key.items()) {
	ShaderData data;
	data.name = shaderName;
	data.type = shaderName.value("type", decltype<ShaderGUI::ShaderType>{});


	
    }

}

void ShaderGUI::ShaderPickerGUI::RenderGUI() {
    if ( !ShaderGUI::shaderGUIState.isShaderPickerGUI ) return;
    
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

    if ( isCombined ) {
	ImGui::InputText("Shader Path", &tempCombinedPath);
	canSave &= !tempCombinedPath.empty();
	validPath = isPathUnique(tempCombinedPath);
	canSave &= validPath;
    } else {
	ImGui::InputText("Vertex Shader Path", &tempVertexPath);
	ImGui::InputText("Fragment Shader Path", &tempFragmentPath);
	canSave &= !tempVertexPath.empty() && !tempFragmentPath.empty();
	validPath = isPathUnique(tempVertexPath, VERTEX) && 
		    isPathUnique (tempFragmentPath, FRAGMENT);
	canSave &= validPath;
    }
    
    !validPath ? ImGui::Text("This shader already exists") : true;


    ImGui::BeginDisabled(!canSave);
    bool saveClicked = ImGui::Button("Save Shader");
    ImGui::EndDisabled();

    if ( saveClicked && canSave ) {
	if ( isCombined ) {
	    names[counter] = tempName;
	    combinedPaths[names[counter]] = tempCombinedPath;
	    counter++;

	    tempName.clear();
	    tempCombinedPath.clear();
	    ShaderGUI::shaderGUIState.isShaderPickerGUI = false;
	} else {
	    names[counter] = tempName;
	    vertexPaths[names[counter]] = tempVertexPath;
	    fragmentPaths[names[counter]] = tempFragmentPath;
	    counter++;

	    tempName.clear();
	    tempVertexPath.clear();
	    tempFragmentPath.clear();
	    ShaderGUI::shaderGUIState.isShaderPickerGUI = false;
	}
    }
    ImGui::End();
}

bool ShaderGUI::ShaderPickerGUI::isPathUnique(std::string currentPath, Type type) {
    switch (type) {
	case Type::VERTEX:
	    for (const auto& [Name, Path] : vertexPaths) {
		if (currentPath == Path) return false;
	    }
	    break;
	case Type::FRAGMENT:
	    for (const auto& [Name, Path] : fragmentPaths) {
		if (currentPath == Path) return false;
	    }
	    break;
	case Type::COMBINED:
	    for (const auto& [Name, Path] : combinedPaths) {
		if (currentPath == Path) return false;
	    }
	    break;
    }
    return true;
}

bool ShaderGUI::ShaderPickerGUI::isPathUnique(std::string currentPath) {
    for (const auto& [Name, Path] : combinedPaths) {
	if (currentPath == Path) return false;
    }
    return true;
}

bool ShaderGUI::ShaderPickerGUI::InitShaderPickerGUI(){
    auto& config = ConfigManager::GetConfig();

    if (config.contains(KEY)) {
	auto& shaders = config[KEY];
	counter = shaders.value("counter", 0);
	names = shaders.value("names", decltype(names){});
	combinedPaths = shaders.value("combinedPaths", decltype(combinedPaths){});
	vertexPaths = shaders.value("vertexPaths", decltype(vertexPaths){});
	fragmentPaths = shaders.value("fragmentPaths", decltype(fragmentPaths){});
    }
    return true;
}

void ShaderGUI::ShaderPickerGUI::SaveToConfig() {
    auto& config = ConfigManager::GetConfig();
    auto& key = config[KEY];

    key["counter"] = counter;
    key["names"] = names;
    key["combinedPaths"] = combinedPaths;
    key["vertexPaths"] = vertexPaths;
    key["fragmentPaths"] = fragmentPaths;

    ConfigManager::Save();
}
