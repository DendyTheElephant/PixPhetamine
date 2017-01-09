﻿#include "UCoreEngine.h"

UCoreEngine* UCoreEngine::game = nullptr;
using namespace PixPhetamine::PostProcess;

UCoreEngine::UCoreEngine() : m_isRunning(false) {
	STACK_TRACE;

	PixPhetamine::Display::openWindowAndInitializeOpenGL(m_SDLWindow, &m_GLContext, WINDOW_CAPTION, WINDOW_WIDTH, WINDOW_HEIGHT);

	m_InputHandler = new CInputHandler(m_SDLWindow);
	m_Camera = new PixPhetamine::CCamera(m_SDLWindow);

	/* ============================================== */
	/* Insert names of shaders to load in ShaderNames */
	/* ============================================== */
	m_ShaderNames.push_back("basic");
	m_ShaderNames.push_back("postprocess");
	m_ShaderNames.push_back("downsampling");
	m_ShaderNames.push_back("blurH");
	m_ShaderNames.push_back("blurV");
	m_ShaderNames.push_back("rgbsplit");
	/* ============================================== */

	/* =========================================== */
	/* Insert names of meshes to load in MeshNames */
	/* =========================================== */
	m_MeshNames.push_back("lionhead");
	m_MeshNames.push_back("cube");
	m_MeshNames.push_back("sphere");
	/* =========================================== */

	STACK_MESSAGE("Loading Shaders");
	loadShaders();
	STACK_MESSAGE("Loading Shaders [COMPLETE]");

	STACK_MESSAGE("Checking OpenGL errors");
	Utility::UErrorHandler::checkOpenGLErrors();

	STACK_MESSAGE("Loading Meshes");
	loadMeshes();
	STACK_MESSAGE("Loading Meshes [COMPLETE]");

	STACK_MESSAGE("Creation of FrameBuffers");
	m_GBufferMS = new CFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, true);
	m_GBufferAA = new CFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, false);
	m_DownSampled = new CFrameBuffer(WINDOW_WIDTH / DOWNSAMPLING, WINDOW_HEIGHT / DOWNSAMPLING, false);
	m_RGBSplitted = new CFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, false);
	m_BufferBlurPartial = new CFrameBuffer(WINDOW_WIDTH / DOWNSAMPLING, WINDOW_HEIGHT / DOWNSAMPLING, false);
	m_BufferBlur = new CFrameBuffer(WINDOW_WIDTH / DOWNSAMPLING, WINDOW_HEIGHT / DOWNSAMPLING, false);
	STACK_MESSAGE("Creation of FrameBuffers [COMPLETE]");

	STACK_MESSAGE("Initialisation of FrameBuffers");
	// Texture attachment
	m_GBufferMS->addTexture("colorTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_GBufferMS->addTexture("normalTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_GBufferMS->addTexture("typeTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_GBufferMS->addTexture("depthTexture", PixPhetamine::LowLevelWrapper::DEPTH);
	m_GBufferAA->addTexture("colorTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_GBufferAA->addTexture("normalTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_GBufferAA->addTexture("typeTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_GBufferAA->addTexture("blurredColorTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_GBufferAA->addTexture("depthTexture", PixPhetamine::LowLevelWrapper::DEPTH);
	m_DownSampled->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_RGBSplitted->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_BufferBlurPartial->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	m_BufferBlur->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::NORMAL);
	STACK_MESSAGE("Initialisation of FrameBuffers [COMPLETE]");

	STACK_MESSAGE("Setup of Post Process passes");
	// Creation
	m_DownSamplingPass = new CPostProcessPass(m_ShaderList["downsampling"], m_DownSampled);
	m_BlurPassPartI = new CPostProcessPass(m_ShaderList["blurH"], m_BufferBlurPartial);
	m_BlurPassPartII = new CPostProcessPass(m_ShaderList["blurV"], m_BufferBlur);
	m_RGBSplitPass = new CPostProcessPass(m_ShaderList["rgbsplit"], m_RGBSplitted);
	m_DeferredShadingPass = new CPostProcessPass(m_ShaderList["postprocess"]);
	// Downsampling pass set-up
	m_DownSamplingPass->bindTexture(m_GBufferAA->getTexture("colorTexture"), "image", 0);
	m_DownSamplingPass->bindVariableName("scale");
	// Blur pass I pass set-up
	m_BlurPassPartI->bindTexture(m_DownSampled->getTexture("processedTexture"), "image", 0);
	// Blur pass II pass set-up
	m_BlurPassPartII->bindTexture(m_BufferBlurPartial->getTexture("processedTexture"), "image", 0);
	// RGB Split pass set-up
	m_RGBSplitPass->bindTexture(m_GBufferAA->getTexture("colorTexture"), "image", 0);
	m_RGBSplitPass->bindVariableName("split");
	// Deferred shading pass set-up
	m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("colorTexture"), "color_map", 0);
	m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("normalTexture"), "normal_map", 1);
	m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("typeTexture"), "type_map", 2);
	m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("blurredColorTexture"), "blurred_color_map", 3);
	m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("depthTexture"), "depth_map", 4);
	m_DeferredShadingPass->bindVariableName("sun_direction");
	STACK_MESSAGE("Setup of Post Process passes [COMPLETE]");

	std::vector<std::string> skyboxTextures;
	skyboxTextures.push_back("textures/skyboxRight.png");
	skyboxTextures.push_back("textures/skyboxLeft.png");
	skyboxTextures.push_back("textures/skyboxTop.png");
	skyboxTextures.push_back("textures/skyboxBottom.png");
	skyboxTextures.push_back("textures/skyboxFront.png");
	skyboxTextures.push_back("textures/skyboxBack.png");
	m_skyBox = new PixPhetamine::SceneRendering::CSkybox("textures/skybox");	

	UNSTACK_TRACE;
}

UCoreEngine::~UCoreEngine() {
	STACK_TRACE;
	PixPhetamine::Display::shutdownSDL_GL(m_SDLWindow, m_GLContext);

	for (auto const &it_shaderName : m_ShaderNames) {
		delete m_ShaderList[it_shaderName];
	}

	for (auto const &it_meshName : m_MeshNames) {
		delete m_MeshList[it_meshName];
	}
	UNSTACK_TRACE;
}

UCoreEngine& UCoreEngine::getInstance() {
    if (UCoreEngine::game == nullptr) {
        UCoreEngine::game = new UCoreEngine();
    }

    return *game;
}

void UCoreEngine::destroyInstance() {
    if (UCoreEngine::game != nullptr) {
        delete UCoreEngine::game;
        UCoreEngine::game = nullptr;
    }
}

void UCoreEngine::loadShaders() {
	STACK_TRACE;
	for (auto const &it_shaderName : m_ShaderNames) {
		std::string vertexShader = SHADERS_FOLDER + it_shaderName + SHADER_VERTEX_EXTENSION;
		std::string fragmentShader = SHADERS_FOLDER + it_shaderName + SHADER_FRAGMENT_EXTENSION;
		m_ShaderList[it_shaderName] = new PixPhetamine::LowLevelWrapper::CShader(vertexShader.c_str(), fragmentShader.c_str());
	}
	UNSTACK_TRACE;
}

void UCoreEngine::reloadShaders() {
	STACK_TRACE;
	for (auto const &it_shaderName : m_ShaderNames) {
		m_ShaderList[it_shaderName]->reload();
	}
	UNSTACK_TRACE;
}

void UCoreEngine::loadMeshes() {
	STACK_TRACE;
	for (auto const &it_meshName : m_MeshNames) {
		std::string meshPath = MESHES_FOLDER + it_meshName + MESHES_EXTENSION;
		m_MeshList[it_meshName] = new PixPhetamine::CStaticMesh(meshPath.c_str());
	}
	UNSTACK_TRACE;
}






void UCoreEngine::runGameLoop() {

	m_isRunning = true;

	m_secondTimer.start();

	do {
		STACK_TRACE;

		const pxUInt startFrameTime = SDL_GetTicks();

		m_InputHandler->update();

		m_Camera->moveView((pxFloat)m_InputHandler->getMouseMotionX(), (pxFloat)m_InputHandler->getMouseMotionY());

		pxFloat speed = 0.4f;

		if (m_InputHandler->getMoveLeft()) {
			m_Camera->moveCameraLeft(speed);
		}
		if (m_InputHandler->getMoveRight()) {
			m_Camera->moveCameraRight(speed);
		}
		if (m_InputHandler->getMoveForward()) {
			m_Camera->moveCameraForward(speed);
		}
		if (m_InputHandler->getMoveBackward()) {
			m_Camera->moveCameraBackward(speed);
		}
		if (m_InputHandler->getMoveUp()) {
			m_Camera->moveCameraUp(speed);
		}
		if (m_InputHandler->getMoveDown()) {
			m_Camera->moveCameraDown(speed);
		}

		if (pxUInt value = m_InputHandler->getBulletTime()) {
			reloadShaders();
		}


		pxVec3f sunDirectionV = pxVec3f(0.5f, 0.5f, 0.0f);
		sunDirectionV = glm::normalize(sunDirectionV);
		pxFloat old_sunDirection[3] = { sunDirectionV.x, sunDirectionV.y, sunDirectionV.z };
		pxVec3f sunDirection = pxVec3f(sunDirectionV.x, sunDirectionV.y, sunDirectionV.z);
		pxFloat sunColor[3] = { 1.0f, 1.0f, 1.0f };


		STACK_MESSAGE("Scene Draw");
		/* =========================================================================================== */
		/* ==== Draw the Scene ======================================================================= */
		/* =========================================================================================== */
		m_ViewProjectionMatrix = m_Camera->getViewProjectionMatrix();
		GLenum gBufferTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_DEPTH_ATTACHMENT };
		PixPhetamine::LowLevelWrapper::initialiseDrawIntoBuffer(m_ShaderList["basic"]->id(), m_GBufferMS->getId(), gBufferTargets, 3);

		/* Draw LionHeads */
		pxFloat type_fox[4] = { 1.0f, 0.0f, 0.0f, 1.0f };


		// PRE SKYBOX OPENGL DIRECTIVES:
		m_skyBox->draw(m_ViewProjectionMatrix, m_Camera->getPosition());
		// POST SKYBOX OPENGL DIRECTIVES:
		glEnable(GL_DEPTH_TEST);
		glUseProgram(m_ShaderList["basic"]->id());

		glBindVertexArray(m_MeshList["lionhead"]->getVBO());

		for (size_t i_lionhead = 0; i_lionhead < 300; ++i_lionhead) {

			m_ModelMatrix = pxMat4f();
			pxVec3f rotateY(0.0f, 1.0f, 0.0f);
			m_ModelMatrix = glm::translate(m_ModelMatrix, pxVec3f(-(i_lionhead % 33 * 3.0f), 0.0f, -(i_lionhead / 33 * 3.0f)));
			//M = glm::rotate(M, 90.0f, rotateY);
			m_ModelMatrix = glm::scale(m_ModelMatrix, pxVec3f(0.5f, 0.5f, 0.5f));

			m_ModelViewProjectionMatrix = m_ViewProjectionMatrix * m_ModelMatrix;

			glUniformMatrix4fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "MVP"), 1, GL_FALSE, glm::value_ptr(m_ModelViewProjectionMatrix));
			glUniform3fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "sun_direction"), 1, old_sunDirection);
			glUniform3fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "sun_color"), 1, sunColor);
			glUniform4fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "object_type"), 1, type_fox);

			glDrawElements(GL_TRIANGLES, m_MeshList["lionhead"]->getNumberOfFaces(), GL_UNSIGNED_SHORT, (void *)0);
		}
		glBindVertexArray(0);

		// disable shader
		glUseProgram(0);

		STACK_MESSAGE("Scene Draw [COMPLETE]");



		STACK_MESSAGE("Anti Aliasing filtering");

		/* =========================================================================================== */
		/* ==== Anti Aliasing filtering ============================================================== */
		/* =========================================================================================== */
		//PixPhetamine::LowLevelWrapper::multiSamplingAntiAliasing(m_GBufferMultiSampled, m_GBufferWitoutAliasing, WINDOW_WIDTH, WINDOW_HEIGHT);
		STACK_TRACE;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBufferMS->getId()); // From the multi sampled aliased GBuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferAA->getId()); // Result is going in the non aliased GBuffer
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		// Resolve color multisampling
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// Resolve normal multisampling
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// Resolve type multisampling
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glDrawBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glReadBuffer(GL_COLOR_ATTACHMENT3);
		glDrawBuffer(GL_COLOR_ATTACHMENT3);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		UNSTACK_TRACE;

		/* =========================================================================================== */
		/* ==== Post Process ========================================================================= */
		/* =========================================================================================== */

		///* Blur ====================================================================================== */
		// Downsampling
		
		m_DownSamplingPass->sendVariable("scale", float(DOWNSAMPLING));
		m_DownSamplingPass->process({ "processedTexture" });

		// Horizontal blur
		m_BlurPassPartI->process({ "processedTexture" });

		// Vertical blur
		m_BlurPassPartII->process({ "processedTexture" });

		//m_GBufferAA->replaceTexture("colorTexture", m_BufferBlur, "processedTexture");
		m_GBufferAA->replaceTexture("blurredColorTexture", m_BufferBlur, "processedTexture");




		if (pxUInt value = m_InputHandler->getShoot()) {
			/* RGB Split ================================================================================================================ */

			pxFloat split = (pxFloat)value / 10.0f;
			
			m_RGBSplitPass->sendVariable("split", split);
			m_RGBSplitPass->process({ "processedTexture" });

			m_GBufferAA->replaceTexture("colorTexture", m_RGBSplitted, "processedTexture");
		}

		/* =========================================================================================== */
		/* ============ Deferred Shading, final pass ================================================= */
		/* =========================================================================================== */
		m_DeferredShadingPass->sendVariable("sun_direction", sunDirection);
		m_DeferredShadingPass->process();









		/* Swap our back buffer to the front */
		SDL_GL_SwapWindow(m_SDLWindow);


		const Uint32 endFrameTime = SDL_GetTicks();

		if (m_secondTimer.getElapsedTime() > 500) {

			m_elapsedTime += endFrameTime - startFrameTime;
			++m_frame;

#ifdef _MSC_VER
			sprintf_s(m_windowCaption, "%s    FPS: %f", WINDOW_CAPTION, m_frame / (m_elapsedTime / 1000.0));
#else
			snprintf(m_windowCaption, 64, "%s    FPS: %f", WINDOW_CAPTION, m_frame / (m_elapsedTime / 1000.0));
#endif

			SDL_SetWindowTitle(m_SDLWindow, m_windowCaption);
			/*
			std::cout << "==========================================" << std::endl;
			std::cout << "Scene rendering:        ";//<< m_renderSceneTimer.getElapsedTime() << " ms" << std::endl;
			std::cout << "MSAAfiltering:          ";//<< m_renderAntiAliasingTimer.getElapsedTime() << " ms" << std::endl;
			std::cout << "Post-process rendering: ";//<< m_renderPostProcessTimer.getElapsedTime() << " ms" << std::endl;
			std::cout << "==========================================" << std::endl;
			*/
			m_secondTimer.start();
		}

		// Every 10 sec
		if (m_elapsedTime > 1000) {
			m_elapsedTime = 0;
			m_frame = 0;
		}

		UNSTACK_TRACE;

	} while (m_InputHandler->isNotQuit());
}
