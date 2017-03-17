/// \file		UCoreEngine.h
/// \author		Daniel Huc, Henry Lefèvre
/// \date		April 2016
#pragma once

/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

/* External dependencies */
#include <GL/glew.h>
#include <SDL2/SDL.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "HConstants.h"
#include "CStaticMesh.h"
#include "CShader.h"
#include "CInputHandler.h"
#include "CCamera.h"
#include "CTimer.h"
#include "LowLevelRenderingWrapper.h"
#include "UErrorHandler.h"
#include "Display.h"
#include "CFrameBuffer.h"
#include "CPostProcessPass.h"
#include "CSkybox.h"

/* Singleton */
class UCoreEngine {
/* Members */
private:
	static UCoreEngine* game;

	pxBool m_isRunning;

	pxInt m_frame{ 0 };
	pxUInt64 m_elapsedTime{ 0 };
	CTimer m_secondTimer; // Timer reset on each second for FPS computation
	char m_windowCaption[64]; // Window caption (updated on each frame to show FPS)

	SDL_Window* m_SDLWindow; // Our window handle
	SDL_GLContext m_GLContext; // Our opengl context handle

	Utility::CInputHandler* m_InputHandler; // Retrieve the inputs of player one
	PixPhetamine::CCamera* m_Camera; // Camera for the player one

	std::vector<std::string> m_ShaderNames;
	std::vector<std::string> m_MeshNames;

	std::map<std::string, PixPhetamine::LowLevelWrapper::CShader*> m_ShaderList; // List of the shaders used in the game
	std::map<std::string, PixPhetamine::CStaticMesh*> m_MeshList;

	pxMat4f m_ModelMatrix;
	pxMat4f m_ViewProjectionMatrix;
	pxMat4f m_ModelViewProjectionMatrix;

	PixPhetamine::PostProcess::CFrameBuffer* m_GBufferMS;
	PixPhetamine::PostProcess::CFrameBuffer* m_GBufferAA;
	PixPhetamine::PostProcess::CFrameBuffer* m_DownSampled;
	PixPhetamine::PostProcess::CFrameBuffer* m_RGBSplitted;
	PixPhetamine::PostProcess::CFrameBuffer* m_BufferBlurPartial;
	PixPhetamine::PostProcess::CFrameBuffer* m_BufferBlur;

	PixPhetamine::PostProcess::CPostProcessPass* m_DownSamplingPass;
	PixPhetamine::PostProcess::CPostProcessPass* m_BlurPassPartI;
	PixPhetamine::PostProcess::CPostProcessPass* m_BlurPassPartII;
	PixPhetamine::PostProcess::CPostProcessPass* m_RGBSplitPass;
	PixPhetamine::PostProcess::CPostProcessPass* m_DeferredShadingPass;

	PixPhetamine::SceneRendering::CSkybox* m_skyBox;

/* Methods */
private:
	UCoreEngine();
	~UCoreEngine();
	void loadShaders();
	void reloadShaders();
	void loadMeshes();

public:
	static UCoreEngine& getInstance();
	static void destroyInstance();
	void runGameLoop();
};
