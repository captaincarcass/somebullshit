
#pragma once
#include "window.hpp"
#include "config_manager.hpp"
#include "gui.hpp"
#include <expected>

enum class InitError {
    GLFW_WINDOW_FAILED
    GLAD_LOAD_FAILED
    CONFIG_LOAD_FAILED
    GUI_INIT_FAILED
};

class Init {
public:
    static std::expected<void, InitError>  Init() {
	window = initWIndow();
	if (!window) {
	    return std::unexpected(InitError::GLFW_WINDOW_FAILED);
	}

	if (!ConfigManager::Load()) {
	    return std::unexpected(InitError::CONFIG_LOAD_FAILED);
	}

	if (!ShaderGUI::ShaderPickerGUI::Init()) {
	    return std::unexpected(InitError::GUI_INIT_FAILED);
	}

	return {};
    }
};
