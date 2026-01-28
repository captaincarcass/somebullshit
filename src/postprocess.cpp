#include <glad/glad.h>
#include "postprocess.hpp"
#include "shaderdef.hpp"

PostProcess::PostProcess(int w, int h, bool hasDepthStencilTex) : width(w), height(h) {
    setupFramebuffers(hasDepthStencilTex);
    setupQuad();
}

PostProcess::~PostProcess() {
    glDeleteFramebuffers(1, &sceneFBO);
    glDeleteFramebuffers(1, &tempFBO);
    glDeleteTextures(1, &sceneTexture);
    glDeleteTextures(1, &tempTexture);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1,&quadVBO);
}

void PostProcess::beginScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcess::applyBlur(Shader& blurShader, float radius) {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    blurShader.use();
    blurShader.setFloat("blurRadius", radius);
    
    glBindFramebuffer(GL_FRAMEBUFFER, tempFBO);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    blurShader.setInt("horizontal", 1);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    blurShader.setInt("horizontal", 0);
    glBindTexture(GL_TEXTURE_2D, tempTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}


void PostProcess::setupQuad() {
    float quadVertices[] = {
	-1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}


void PostProcess::setupFramebuffers(bool hasDepthStencilTex) {
    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);

    if (!hasDepthStencilTex) {
	glGenRenderbuffers(1, &sceneRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);
    }
    if (hasDepthStencilTex) {
	glGenTextures(1, &depthStencilTexture);
	glBindTexture(GL_TEXTURE_2D, depthStencilTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture, 0);
    }

    glGenTextures(1, &tempTexture);
    glBindTexture(GL_TEXTURE_2D, tempTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenFramebuffers(1, &tempFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, tempFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempTexture, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::debugDrawSceneTexture() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);  // BLUE clear color
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

PostProcess::Chain::Chain(PostProcess& postProcess)
    :pp(postProcess), currentInput(postProcess.sceneTexture), useSceneAsInput(true)
{}

PostProcess::Chain& PostProcess::Chain::add(Shader& shader, void (*configure)(Shader&)) {
    glDisable(GL_DEPTH_TEST);
    shader.use();

    if (configure) configure(shader);

    GLuint outputFBO = useSceneAsInput ? pp.tempTexture : pp.sceneTexture;

    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, pp.width, pp.height);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, currentInput);
    glBindVertexArray(pp.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    currentInput = useSceneAsInput ? pp.tempTexture : pp.sceneTexture;
    useSceneAsInput = !useSceneAsInput;

    glEnable(GL_DEPTH_TEST);
    return *this;
}

void PostProcess::Chain::finish(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, pp.width, pp.height);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, currentInput);
    glBindVertexArray(pp.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

PostProcess::Chain PostProcess::chain(){
    return Chain(*this);
}
