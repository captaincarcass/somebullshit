#pragma once
#include "shaderdef.hpp"
class PostProcess {
public:
    PostProcess(int w, int h, bool hasDepthStencilTex = false);
    ~PostProcess();
    void beginScene();
    void applyBlur(Shader& blurShader, float radius=1.0f);
    void debugDrawSceneTexture();
    class Chain {
    public:
	Chain(PostProcess& postProcess);
	Chain& add(Shader& shader, void (*configure)(Shader&) = nullptr);
	void finish();
    private:
	PostProcess& pp;
	GLuint currentInput;
	bool useSceneAsInput;
    };
    Chain chain();
private:
    GLuint sceneFBO, sceneTexture;
    GLuint tempFBO, tempTexture;
    GLuint quadVAO, quadVBO;
    int width, height;
    GLuint sceneRBO;
    GLuint depthStencilTexture;
    void setupQuad();
    void setupFramebuffers(bool hasDepthStencilTex = false);
};
