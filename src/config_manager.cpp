
#include <json>
#include <fstream>
#include <unordered_map>
#include <string>
#include "config_manager.hpp"

using json = nlohmann::json;

json& ConfigManager::GetConfig() {
    return config;
}

bool ConfigManager::Load() {
    std::ifstream file(configPath);
    if (!file.isOpen()) {
	std::cerr << "Failed to read" << configPath << std::endl;
	return false;
    }

    try {
	file >> config;
	return true;
    } catch (const nlohmann::json::exception& e) {
	std::cerr << "Failed to parse JSON" << e.what() << std::endl;
	return false;
    }
}

bool ConfigManager::Save() {
    std::ofstream file(configPath);
    if (!file.isOpen()) {
	std::cerr << "Failed to open " << configPath << " for writing" << std::endl;
	return false;
    }

    file << config.dump(4);
    return true;
}

void ConfigManager::Clear() {
    config.clear();
}



