
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <cmath>
#include "shaderdef.hpp"
#include "primitives.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "postprocess.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "gui.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "window.hpp"
#include "init.hpp"

uint64_t SCR_WIDTH = 800;
uint64_t SCR_HEIGHT = 600;

uint64_t g_windowWidth = SCR_WIDTH;
uint64_t g_windowHeight = SCR_HEIGHT;

const float PI { 3.14159265359 };

OrthoCamera camera;

float deltaTime{};
float lastFrame{};



float func(float x){
    return std::sin(x);
} 

int main() {
    auto result = Init::Init();
    if (!result) {
	switch (result.error()) {
	    case InitError::GLFW_WINDOW_FAILED:
		std::cerr << "Failed to create GLFW window\n";
		break;
	    case InitError::GLAD_LOAD_FAILED:
		std::cerr << "Failed to load GLAD\n";
		break;
	    case InitError::CONFIG_LOAD_FAILED:
		std::cerr << "Failed to load config file\n";
		break;
	    case InitError::GUI_INIT_FAILED:
		std::cerr << "Failed to initalise GUI\n";
		break;
	}
	return -1;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);


    PostProcess postProcess(fbWidth, fbHeight, false);
    Shader blurShader("shaders/gaussianblur.glsl");
    Shader bilateral("shaders/bilateralfilter.shader");
    Shader dog("shaders/dog.shader");

    Shader image("shaders/image.shader");

    Shader testShader("shaders/testshader.glsl");
    Shader blinnphong("shaders/blinnphong.glsl");
    Shader lightingShader("shaders/lightsource.glsl");

    //instantiating meshes

    std::vector<float> planeVertices{};
    std::vector<unsigned int> planeIndices{};
    genPlane(planeVertices, planeIndices, 1.8f, 1);
    Mesh quad(planeVertices, planeIndices, true, true, true);


    std::vector<float> vertices {};
    std::vector<unsigned int> indices {};
    genSphere(vertices, indices, 0.5f, 50, 50);
    Mesh sphere(vertices, indices, true, false, true);
    
    std::vector<float> vLight{};
    std::vector<unsigned int> iLight{};
    genSphere(vLight, iLight, 0.05f, 10, 10);
    Mesh light(vLight, iLight, true, false, true);

    Graph g;
    g.lineWidth = 0.1f;
    Mesh graph = g.linePlotThin(func, -100*PI,100*PI , 0.0001f);

    //texture stuff
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int texwidth, texheight, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("standard-test-images-for-Image-Processing/standard_test_images/barbara.bmp", &texwidth, &texheight, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ShaderGUI::ShaderUniformGUI uniformsGUI(&dog);
    ShaderGUI::ShaderPickerGUI pickerGUI;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    while(!glfwWindowShouldClose(window)){
	postProcess.beginScene();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLineWidth(1.0f);

	ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


	float currentFrame{(float)glfwGetTime()};
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processCameraInputs(camera, deltaTime);
	processGUIInputs();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);

	float imageAspect = (float)texwidth / (float)texheight;
	float windowAspect = (float)g_windowWidth / (float)g_windowHeight;

	float scaleX = 1.0f;
	float scaleY = 1.0f;

	if (imageAspect > windowAspect)
	    scaleY = windowAspect / imageAspect;
	else
	    scaleX = imageAspect / windowAspect;


	float aspect = (float)g_windowWidth / (float)g_windowHeight;
	image.use();
	image.setFloat("aspect", aspect);
	image.setInt("texture1", 0);
	quad.draw();
	
	pickerGUI.RenderGUI();
	uniformsGUI.RenderGUI(dog);

	postProcess.chain()
	    .add(dog, nullptr)
	    .finish();

	ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	std::cout << deltaTime * 1000 << "ms\n";

	glfwPollEvents();
    	glfwSwapBuffers(window);
    }
    

    glfwTerminate();
    return 0;
}




