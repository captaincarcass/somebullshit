
#pragma once
#include <json>
#include <fstream>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

class ConfigManager {
private:
    inline static nlohmann::json config;
    inline static std::string configPath = "config/config.json";
public:
    static nlohmann::json& GetConfig();
    static bool Load(const std::string& name);
    static bool Save(const std::string& name);
    static void Clear(const std::string& name);
};

namespace glm {
    inline void to_json(nlohmann::json& j, const vec2& v){j={v.x,v.y};}
    inline void from_json(const nlohmann::json& j, vec2& v){v.x=j[0];v.y=j[1];} 
    inline void to_json(nlohmann::json& j, const vec3& v){j={v.x,v.y,v.z};}
    inline void from_json(const nlohmann::json& j, vec3& v){v.x=j[0];v.y=j[1];v.z= j[2];}
    inline void to_json(nlohmann::json& j, const vec4& v){j ={v.x,v.y,v.z,v.w};}
    inline void from_json(const nlohmann::json& j, vec4& v){v.x=j[0];v.y=j[1];v.z=j[2];v.w= j[3];}
}
