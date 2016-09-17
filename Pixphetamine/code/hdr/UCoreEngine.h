#pragma once

/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

/* External dependencies */
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "CStaticMesh.h"
#include "CShader.h"
#include "CInputHandler.h"
#include "CCamera.h"
#include "CTimer.h"
#include "LowLevelRenderingWrapper.h"
#include "UErrorHandler.h"
#include "Display.h"

#define WINDOW_WIDTH				1200
#define WINDOW_HEIGHT				800
#define WINDOW_CAPTION				"Sick Beam Phetamine"
#define SHADERS_FOLDER				"shaders/"
#define MESHES_FOLDER				"models/"
#define MESHES_EXTENSION			".lowpoly"
#define SHADER_FRAGMENT_EXTENSION	".frag"
#define SHADER_VERTEX_EXTENSION		".vert"



/* Singleton */
class UCoreEngine {
/* Members */
private:
	pxBool m_isRunning;

	pxInt m_frame{ 0 };
	pxUInt64 m_elapsedTime{ 0 };
	//CTimer m_frameTimer; // Timer reset on each frame for FPS computation
	CTimer m_secondTimer; // Timer reset on each second for FPS computation
	//CTimer m_renderSceneTimer; // Timer that would measure the scene rendering
	//CTimer m_renderAntiAliasingTimer; // Timer that would measure the anti aliasing filtering
	//CTimer m_renderPostProcessTimer; // Timer that would measure the post process rendering
	char m_windowCaption[64]; // Window caption (updated on each frame to show FPS)

	SDL_Window* m_SDLWindow; // Our window handle
	SDL_GLContext m_GLContext; // Our opengl context handle

	CInputHandler* m_InputHandler; // Retrieve the inputs of player one
	PixPhetamine::CCamera* m_Camera; // Camera for the player one

	std::vector<std::string> m_ShaderNames;
	std::vector<std::string> m_MeshNames;

	std::map<std::string, PixPhetamine::LowLevelWrapper::CShader*> m_ShaderList; // List of the shaders used in the game
	std::map<std::string, PixPhetamine::CStaticMesh*> m_MeshList;

	pxMat4f m_ModelMatrix;
	pxMat4f m_ViewProjectionMatrix;
	pxMat4f m_ModelViewProjectionMatrix;

	PixPhetamine::LowLevelWrapper::GBuffer* m_GBufferMultiSampled;
	PixPhetamine::LowLevelWrapper::GBuffer* m_GBufferWitoutAliasing;
	PixPhetamine::LowLevelWrapper::ImageBuffer* m_BufferBlurPartial;
	PixPhetamine::LowLevelWrapper::ImageBuffer* m_BufferBlur;

/* Methods */
private:
	UCoreEngine();
	~UCoreEngine();
	void loadShaders();
	void loadMeshes();

public:
	static UCoreEngine& getInstance();
	static void destroyInstance();
	void runGameLoop();
};
