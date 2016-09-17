#include "UCoreEngine.h"

UCoreEngine* UCoreEngine::game = nullptr;

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
	STACK_MESSAGE("Loading Meshes");
	loadMeshes();
	STACK_MESSAGE("Loading Meshes [COMPLETE]");

	STACK_MESSAGE("Creation of FrameBuffers");
	m_GBufferMultiSampled = new PixPhetamine::LowLevelWrapper::GBuffer();
	m_GBufferWitoutAliasing = new PixPhetamine::LowLevelWrapper::GBuffer();
	m_BufferBlurPartial = new PixPhetamine::LowLevelWrapper::ImageBuffer();
	m_BufferBlur = new PixPhetamine::LowLevelWrapper::ImageBuffer();
	STACK_MESSAGE("Creation of FrameBuffers [COMPLETE]");

	STACK_MESSAGE("Initialisation of FrameBuffers");
	m_GBufferMultiSampled->initialize(WINDOW_WIDTH, WINDOW_HEIGHT, GL_TEXTURE_2D_MULTISAMPLE);
	m_GBufferWitoutAliasing->initialize(WINDOW_WIDTH, WINDOW_HEIGHT, GL_TEXTURE_2D);
	m_BufferBlurPartial->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_BufferBlur->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
	STACK_MESSAGE("Initialisation of FrameBuffers [COMPLETE]");

	UNSTACK_TRACE;
}

UCoreEngine::~UCoreEngine() {
	PixPhetamine::Display::shutdownSDL_GL(m_SDLWindow, m_GLContext);

	m_GBufferMultiSampled->free();
	m_GBufferWitoutAliasing->free();
	m_BufferBlurPartial->free();
	m_BufferBlur->free();

	for (auto const &it_shaderName : m_ShaderNames) {
		delete m_ShaderList[it_shaderName];
	}

	for (auto const &it_meshName : m_MeshNames) {
		delete m_MeshList[it_meshName];
	}
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
		m_ShaderList[it_shaderName] = new PixPhetamine::LowLevelWrapper::CShader();
		std::string vertexShader = SHADERS_FOLDER + it_shaderName + SHADER_VERTEX_EXTENSION;
		std::string fragmentShader = SHADERS_FOLDER + it_shaderName + SHADER_FRAGMENT_EXTENSION;
		m_ShaderList[it_shaderName]->load(vertexShader.c_str(), fragmentShader.c_str());
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


		pxVec3f sunDirectionV = pxVec3f(0.5f, 0.5f, 0.0f);
		sunDirectionV = glm::normalize(sunDirectionV);
		pxFloat sunDirection[3] = { sunDirectionV.x, sunDirectionV.y, sunDirectionV.z };
		pxFloat sunColor[3] = { 1.0f, 1.0f, 1.0f };


		STACK_MESSAGE("Scene Draw");
		/* =========================================================================================== */
		/* ==== Draw the Scene ======================================================================= */
		/* =========================================================================================== */
		m_ViewProjectionMatrix = m_Camera->getViewProjectionMatrix();
		GLenum gBufferTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_DEPTH_ATTACHMENT };
		PixPhetamine::LowLevelWrapper::initialiseDrawIntoBuffer(m_ShaderList["basic"]->id(), m_GBufferMultiSampled->id, gBufferTargets, 3);

		/* Draw LionHeads */
		pxFloat type_fox[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		glBindVertexArray(m_MeshList["lionhead"]->getVBO());

		for (size_t i_lionhead = 0; i_lionhead < 3000; ++i_lionhead) {

			m_ModelMatrix = pxMat4f();
			pxVec3f rotateY(0.0f, 1.0f, 0.0f);
			m_ModelMatrix = glm::translate(m_ModelMatrix, pxVec3f(-(i_lionhead % 33 * 3.0f), 0.0f, -(i_lionhead / 33 * 3.0f)));
			//M = glm::rotate(M, 90.0f, rotateY);
			m_ModelMatrix = glm::scale(m_ModelMatrix, pxVec3f(0.5f, 0.5f, 0.5f));

			m_ModelViewProjectionMatrix = m_ViewProjectionMatrix * m_ModelMatrix;

			glUniformMatrix4fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "MVP"), 1, GL_FALSE, glm::value_ptr(m_ModelViewProjectionMatrix));
			glUniform3fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "sun_direction"), 1, sunDirection);
			glUniform3fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "sun_color"), 1, sunColor);
			glUniform4fv(glGetUniformLocation(m_ShaderList["basic"]->id(), "object_type"), 1, type_fox);

			glDrawElements(GL_TRIANGLES, 3 * m_MeshList["lionhead"]->getNumberOfFaces(), GL_UNSIGNED_SHORT, (void *)0);
		}

		glBindVertexArray(0);
		
		// disable shader 
		glUseProgram(0);

		STACK_MESSAGE("Scene Draw [COMPLETE]");
		STACK_MESSAGE("Anti Aliasing filtering");

		/* =========================================================================================== */
		/* ==== Anti Aliasing filtering ============================================================== */
		/* =========================================================================================== */
		PixPhetamine::LowLevelWrapper::multiSamplingAntiAliasing(m_GBufferMultiSampled, m_GBufferWitoutAliasing, WINDOW_WIDTH, WINDOW_HEIGHT);
		
		/* =========================================================================================== */
		/* ==== Post Process ========================================================================= */
		/* =========================================================================================== */


		///* Blur ====================================================================================== */
		if (pxUInt value = m_InputHandler->getBulletTime()) {
			for (pxInt i = 0; i < 4; ++i) {
				GLenum blurPassTargets[] = { GL_COLOR_ATTACHMENT0 };
				PixPhetamine::LowLevelWrapper::initialiseDrawIntoBuffer(m_ShaderList["blurH"]->id(), m_BufferBlurPartial->id, blurPassTargets, 1);

				// send the textures
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->colorTexture); // Activate the texture to send
				glUniform1i(glGetUniformLocation(m_ShaderList["blurH"]->id(), "image"), 0); // Send it to the shader

				// Send quad and draw
				glBindVertexArray(m_BufferBlurPartial->VAO_id);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);



				PixPhetamine::LowLevelWrapper::initialiseDrawIntoBuffer(m_ShaderList["blurV"]->id(), m_BufferBlur->id, blurPassTargets, 1);

				// send the textures
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_BufferBlurPartial->texture); // Activate the texture to send
				glUniform1i(glGetUniformLocation(m_ShaderList["blurV"]->id(), "image"), 0); // Send it to the shader

				// Send quad and draw
				glBindVertexArray(m_BufferBlur->VAO_id);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);



				/* Store the blurred texture to the G Buffer */
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferWitoutAliasing->id); // Result is going in the non aliased GBuffer
				glBindFramebuffer(GL_READ_FRAMEBUFFER, m_BufferBlur->id); // From the multi sampled aliased GBuffer
				glClearColor(0.5, 0.5, 0.5, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);

				// Resolve color multisampling
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
				glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			}
		}
		



		if (pxUInt value = m_InputHandler->getShoot()) {
			/* RGB Split ================================================================================================================ */
			GLenum rgbPassTarget[] = { GL_COLOR_ATTACHMENT0 };
			PixPhetamine::LowLevelWrapper::initialiseDrawIntoBuffer(m_ShaderList["rgbsplit"]->id(), m_BufferBlur->id, rgbPassTarget, 1);

			// send the textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->colorTexture); // Activate the texture to send
			glUniform1i(glGetUniformLocation(m_ShaderList["rgbsplit"]->id(), "image"), 0); // Send it to the shader

			pxFloat split = (pxFloat)value / 10.0f;
			m_ShaderList["rgbsplit"]->bindVariableName("split");
			m_ShaderList["rgbsplit"]->sendVariable("split", split);
			//glUniform1f(glGetUniformLocation(m_ShaderList["rgbsplit"]->id(), "split"), split);

			// Send quad and draw
			glBindVertexArray(m_BufferBlur->VAO_id);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);



			/* Store the blurred texture to the G Buffer */
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferWitoutAliasing->id); // Result is going in the non aliased GBuffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_BufferBlur->id); // From the multi sampled aliased GBuffer
			glClearColor(0.5, 0.5, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			// Resolve color multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		





		/* =========================================================================================== */
		// Reset frame buffer to read/write to the default window
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(m_ShaderList["postprocess"]->id());

		// send the light
		glUniform3fv(glGetUniformLocation(m_ShaderList["postprocess"]->id(), "sun_direction"), 1, sunDirection);
		//glUniform3fv(glGetUniformLocation(m_ShaderList["ssao"]->id(), "view_direction"), 1, fview);

		// send the textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->colorTexture);
		glUniform1i(glGetUniformLocation(m_ShaderList["postprocess"]->id(), "color_map"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->normalTexture);
		glUniform1i(glGetUniformLocation(m_ShaderList["postprocess"]->id(), "normal_map"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->typeTexture);
		glUniform1i(glGetUniformLocation(m_ShaderList["postprocess"]->id(), "type_map"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->depthTexture);
		glUniform1i(glGetUniformLocation(m_ShaderList["postprocess"]->id(), "depth_map"), 3);


		// Send quad and draw
		glBindVertexArray(m_GBufferWitoutAliasing->VAO_id);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		








		/* Swap our back buffer to the front */
		SDL_GL_SwapWindow(m_SDLWindow);


		const Uint32 endFrameTime = SDL_GetTicks();

		if (m_secondTimer.getElapsedTime() > 1000.0) {
			
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
		if (m_elapsedTime > 10000) {
			m_elapsedTime = 0;
			m_frame = 0;
		}
		
		UNSTACK_TRACE;

	} while (m_InputHandler->isNotQuit());
}
