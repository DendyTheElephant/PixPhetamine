#include "UCoreEngine.h"

UCoreEngine* UCoreEngine::game = nullptr;
using namespace PixPhetamine::PostProcess;
using namespace PixPhetamine;
using namespace Utility;
using namespace PixPhetamine::Display;

UCoreEngine::UCoreEngine() : m_isRunning(false) {
	STACK_TRACE;

	openWindowAndInitializeOpenGL(m_SDLWindow, &m_GLContext, PIX::WINDOW_CAPTION, PIX::WINDOW_WIDTH, PIX::WINDOW_HEIGHT);

	m_InputHandler = new Utility::CInputHandler(m_SDLWindow);
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

	STACK_MESSAGE("Checking OpenGL errors");
	Utility::UErrorHandler::checkOpenGLErrors();

	STACK_MESSAGE("Creation of FrameBuffers");
	m_GBufferMS = new CFrameBuffer(PIX::WINDOW_WIDTH, PIX::WINDOW_HEIGHT, CFrameBuffer::EType::WITH_DEPTH_MULTISAMPLED);
	m_GBufferAA = new CFrameBuffer(PIX::WINDOW_WIDTH, PIX::WINDOW_HEIGHT, CFrameBuffer::EType::WITH_DEPTH);
	m_DownSampled = new CFrameBuffer(PIX::WINDOW_WIDTH / static_cast<pxUInt>(PIX::DOWNSCALING), PIX::WINDOW_HEIGHT / static_cast<pxUInt>(PIX::DOWNSCALING), CFrameBuffer::EType::NORMAL);
	m_RGBSplitted = new CFrameBuffer(PIX::WINDOW_WIDTH, PIX::WINDOW_HEIGHT, CFrameBuffer::EType::NORMAL);
	m_BufferBlurPartial = new CFrameBuffer(PIX::WINDOW_WIDTH / static_cast<pxUInt>(PIX::DOWNSCALING), PIX::WINDOW_HEIGHT / static_cast<pxUInt>(PIX::DOWNSCALING), CFrameBuffer::EType::NORMAL);
	m_BufferBlur = new CFrameBuffer(PIX::WINDOW_WIDTH / static_cast<pxUInt>(PIX::DOWNSCALING), PIX::WINDOW_HEIGHT / static_cast<pxUInt>(PIX::DOWNSCALING), CFrameBuffer::EType::NORMAL);
	STACK_MESSAGE("Creation of FrameBuffers [COMPLETE]");

	STACK_MESSAGE("Checking OpenGL errors");
	Utility::UErrorHandler::checkOpenGLErrors();

	STACK_MESSAGE("Initialisation of FrameBuffers");
	// Texture attachment
	m_GBufferMS->addTexture("colorTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
	m_GBufferMS->addTexture("normalTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 1);
	m_GBufferMS->addTexture("typeTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 2);

	m_GBufferAA->addTexture("colorTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
	m_GBufferAA->addTexture("normalTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 1);
	m_GBufferAA->addTexture("typeTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 2);
	m_GBufferAA->addTexture("blurredColorTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 3);

	m_DownSampled->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
	m_RGBSplitted->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
	m_BufferBlurPartial->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
	m_BufferBlur->addTexture("processedTexture", PixPhetamine::LowLevelWrapper::CTexture::ETextureType::NORMAL, 0);
	STACK_MESSAGE("Initialisation of FrameBuffers [COMPLETE]");

	STACK_MESSAGE("Checking OpenGL errors");
	Utility::UErrorHandler::checkOpenGLErrors();

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
	m_DeferredShadingPass->bindTexture(m_GBufferAA->getTexture("depth_texture"), "depth_map", 4);
	m_DeferredShadingPass->bindVariableName("sun_direction");
	STACK_MESSAGE("Setup of Post Process passes [COMPLETE]");

	STACK_MESSAGE("Checking OpenGL errors");
	Utility::UErrorHandler::checkOpenGLErrors();

	std::vector<std::string> skyboxTextures;
	skyboxTextures.push_back("textures/skyboxRight.png");
	skyboxTextures.push_back("textures/skyboxLeft.png");
	skyboxTextures.push_back("textures/skyboxTop.png");
	skyboxTextures.push_back("textures/skyboxBottom.png");
	skyboxTextures.push_back("textures/skyboxFront.png");
	skyboxTextures.push_back("textures/skyboxBack.png");
	m_skyBox = new PixPhetamine::SceneRendering::CSkybox("textures/skybox");	

	STACK_MESSAGE("Checking OpenGL errors");
	Utility::UErrorHandler::checkOpenGLErrors();

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
		std::string vertexShader = PIX::SHADERS_FOLDER + it_shaderName + PIX::SHADER_VERTEX_EXTENSION;
		std::string fragmentShader = PIX::SHADERS_FOLDER + it_shaderName + PIX::SHADER_FRAGMENT_EXTENSION;
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
		std::string meshPath = PIX::MODELS_FOLDER + it_meshName + PIX::MODELS_EXTENSION;
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

		if (m_InputHandler->getKey(CInputHandler::EInput::LEFT)) {
			m_Camera->moveCameraLeft(speed);
		}
		if (m_InputHandler->getKey(CInputHandler::EInput::RIGHT)) {
			m_Camera->moveCameraRight(speed);
		}
		if (m_InputHandler->getKey(CInputHandler::EInput::UP)) {
			m_Camera->moveCameraForward(speed);
		}
		if (m_InputHandler->getKey(CInputHandler::EInput::DOWN)) {
			m_Camera->moveCameraBackward(speed);
		}
		if (m_InputHandler->getKey(CInputHandler::EInput::SHOULDER_RIGHT)) {
			m_Camera->moveCameraUp(speed);
		}
		if (m_InputHandler->getKey(CInputHandler::EInput::SHOULDER_LEFT)) {
			m_Camera->moveCameraDown(speed);
		}

		/*if (pxUInt value = m_InputHandler->getBulletTime()) {
			reloadShaders();
		}*/


		pxVec3f sunDirectionV = pxVec3f(0.5f, 0.5f, 0.0f);
		sunDirectionV = glm::normalize(sunDirectionV);
		pxFloat old_sunDirection[3] = { sunDirectionV.x, sunDirectionV.y, sunDirectionV.z };
		pxVec3f sunDirection = pxVec3f(sunDirectionV.x, sunDirectionV.y, sunDirectionV.z);
		pxFloat sunColor[3] = { 1.0f, 1.0f, 1.0f };


		STACK_MESSAGE("Scene Draw");
		STACK_MESSAGE("Checking OpenGL errors");
		Utility::UErrorHandler::checkOpenGLErrors();

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

		for (size_t i_lionhead = 0; i_lionhead < 3000; ++i_lionhead) {

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

		STACK_MESSAGE("Scene Draw");
		STACK_MESSAGE("Checking OpenGL errors");
		Utility::UErrorHandler::checkOpenGLErrors();

		STACK_MESSAGE("Anti Aliasing filtering");

		/* =========================================================================================== */
		/* ==== Anti Aliasing filtering ============================================================== */
		/* =========================================================================================== */
		//PixPhetamine::LowLevelWrapper::multiSamplingAntiAliasing(m_GBufferMultiSampled, m_GBufferWitoutAliasing, WINDOW_WIDTH, WINDOW_HEIGHT);
		STACK_TRACE;

		m_GBufferMS->blit(m_GBufferAA, { 
			CFrameBuffer::SBlit("colorTexture", "colorTexture"),
			CFrameBuffer::SBlit("normalTexture", "normalTexture"),
			CFrameBuffer::SBlit("typeTexture", "typeTexture"),
			CFrameBuffer::SBlit("depth_texture", "depth_texture")
		});

		UNSTACK_TRACE;

		STACK_MESSAGE("Anti Aliasing filtering [COMPLETE]");
		STACK_MESSAGE("Scene Draw");
		STACK_MESSAGE("Checking OpenGL errors");
		Utility::UErrorHandler::checkOpenGLErrors();
		/* =========================================================================================== */
		/* ==== Post Process ========================================================================= */
		/* =========================================================================================== */

		///* Blur ====================================================================================== */
		// Downsampling
		
		m_DownSamplingPass->sendVariable("scale", float(PIX::DOWNSCALING));
		m_DownSamplingPass->process({ "processedTexture" });

		// Horizontal blur
		m_BlurPassPartI->process({ "processedTexture" });

		// Vertical blur
		m_BlurPassPartII->process({ "processedTexture" });

		//m_GBufferAA->replaceTexture("colorTexture", m_BufferBlur, "processedTexture");
		//m_GBufferAA->replaceTexture("blurredColorTexture", m_BufferBlur, "processedTexture");
		m_BufferBlur->blit(m_GBufferAA, { CFrameBuffer::SBlit("processedTexture", "blurredColorTexture") });




		//if (pxUInt value = m_InputHandler->getShoot()) {
			/* RGB Split ================================================================================================================ */

			//pxFloat split = (pxFloat)value / 10.0f;
			//
			//m_RGBSplitPass->sendVariable("split", split);
			//m_RGBSplitPass->process({ "processedTexture" });

			////m_GBufferAA->replaceTexture("colorTexture", m_RGBSplitted, "processedTexture");
			//m_RGBSplitted->blit(m_GBufferAA, { CFrameBuffer::SBlit("processedTexture", "colorTexture") });
		//}

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
			sprintf_s(m_windowCaption, "%s    FPS: %f", PIX::WINDOW_CAPTION, m_frame / (m_elapsedTime / 1000.0));
#else
			snprintf(m_windowCaption, 64, "%s    FPS: %f", PIX::WINDOW_CAPTION, m_frame / (m_elapsedTime / 1000.0));
#endif

			SDL_SetWindowTitle(m_SDLWindow, m_windowCaption);
			/*
			std::cout << "==========================================" << std::endl;
			std::cout << "Scene rendering:        ";//< m_renderSceneTimer.getElapsedTime() << " ms" << std::endl;
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

		STACK_MESSAGE("Scene Draw");
		STACK_MESSAGE("Checking OpenGL errors");
		Utility::UErrorHandler::checkOpenGLErrors();

		UNSTACK_TRACE;

	} while (m_InputHandler->isQuit() == false);
}
