#include "SCoreEngine.h"

SCoreEngine::SCoreEngine() : m_isRunning(false) {
	m_SDLWindow = render::createSDLWindow(WINDOW_CAPTION, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_GLContext = render::initRenderContext(m_SDLWindow);

	m_InputHandler = new CInputHandler(m_SDLWindow);
	m_Camera = new Render::CCamera(m_SDLWindow);

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

	loadShaders();
	loadMeshes();

	m_GBufferMultiSampled = new render::GBuffer();
	m_GBufferWitoutAliasing = new render::GBuffer();
	m_BufferBlurPartial = new render::ImageBuffer();
	m_BufferBlur = new render::ImageBuffer();
	m_GBufferMultiSampled->initialize(WINDOW_WIDTH, WINDOW_HEIGHT, GL_TEXTURE_2D_MULTISAMPLE);
	m_GBufferWitoutAliasing->initialize(WINDOW_WIDTH, WINDOW_HEIGHT, GL_TEXTURE_2D);
	m_BufferBlurPartial->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_BufferBlur->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

SCoreEngine::~SCoreEngine() {
	render::shutdownSDL_GL(m_SDLWindow, m_GLContext);

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

SCoreEngine& SCoreEngine::getInstance() {
	static SCoreEngine* game = nullptr;
	if (game == nullptr) {
		game = new SCoreEngine();
	}

	return *game;
}

void SCoreEngine::destroyInstance() {
	static SCoreEngine* game = &getInstance();
	if (game != nullptr) {
		delete game;
	}
}

void SCoreEngine::loadShaders() {
	for (auto const &it_shaderName : m_ShaderNames) {
		m_ShaderList[it_shaderName] = new Render::CShader();
		std::string vertexShader = SHADERS_FOLDER + it_shaderName + SHADER_VERTEX_EXTENSION;
		std::string fragmentShader = SHADERS_FOLDER + it_shaderName + SHADER_FRAGMENT_EXTENSION;
		m_ShaderList[it_shaderName]->load(vertexShader.c_str(), fragmentShader.c_str());
	}
}

void SCoreEngine::loadMeshes() {
	for (auto const &it_meshName : m_MeshNames) {
		std::string meshPath = MESHES_FOLDER + it_meshName + MESHES_EXTENSION;
		m_MeshList[it_meshName] = new Render::CStaticMesh(meshPath.c_str());
	}
}






void SCoreEngine::runGameLoop() {
	m_isRunning = true;

	m_secondTimer.start();

	do {
		
		const Uint32 startFrameTime = SDL_GetTicks();

		m_InputHandler->update();
		
		m_Camera->moveView((float)m_InputHandler->getMouseMotionX(), (float)m_InputHandler->getMouseMotionY());

		float speed = 0.4f;

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


		glm::vec3 sunDirectionV = glm::vec3(0.5f, 0.5f, 0.0f);
		sunDirectionV = glm::normalize(sunDirectionV);
		float sunDirection[3] = { sunDirectionV.x, sunDirectionV.y, sunDirectionV.z };
		float sunColor[3] = { 1.0f, 1.0f, 1.0f };



		/* =========================================================================================== */
		/* ==== Draw the Scene ======================================================================= */
		/* =========================================================================================== */
		m_ViewProjectionMatrix = m_Camera->getViewProjectionMatrix();
		GLenum gBufferTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_DEPTH_ATTACHMENT };
		render::initialiseDrawIntoBuffer(m_ShaderList["basic"]->id(), m_GBufferMultiSampled->id, gBufferTargets, 3);

		/* Draw LionHeads */
		float type_fox[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		glBindVertexArray(m_MeshList["lionhead"]->getVBO());

		for (size_t i_lionhead = 0; i_lionhead < 100; ++i_lionhead) {

			m_ModelMatrix = glm::mat4();
			glm::vec3 rotateY(0.0f, 1.0f, 0.0f);
			m_ModelMatrix = glm::translate(m_ModelMatrix, glm::vec3(-(i_lionhead % 10 * 3.0f), 0.0f, -(i_lionhead / 10 * 3.0f)));
			//M = glm::rotate(M, 90.0f, rotateY);
			m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

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

		
		/* =========================================================================================== */
		/* ==== Anti Aliasing filtering ============================================================== */
		/* =========================================================================================== */
		render::multiSamplingAntiAliasing(m_GBufferMultiSampled, m_GBufferWitoutAliasing, WINDOW_WIDTH, WINDOW_HEIGHT);

		/* =========================================================================================== */
		/* ==== Post Process ========================================================================= */
		/* =========================================================================================== */


		///* Blur ====================================================================================== */

		for (int i = 0; i < 0; ++i) {
			GLenum blurPassTargets[] = { GL_COLOR_ATTACHMENT0 };
			render::initialiseDrawIntoBuffer(m_ShaderList["blurH"]->id(), m_BufferBlurPartial->id, blurPassTargets, 1);

			// send the textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->colorTexture); // Activate the texture to send
			glUniform1i(glGetUniformLocation(m_ShaderList["blurH"]->id(), "image"), 0); // Send it to the shader

			// Send quad and draw
			glBindVertexArray(m_BufferBlurPartial->VAO_id);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);



			render::initialiseDrawIntoBuffer(m_ShaderList["blurV"]->id(), m_BufferBlur->id, blurPassTargets, 1);

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
			glDisable(GL_DEPTH);

			// Resolve color multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		



		if (unsigned int value = m_InputHandler->getShoot()) {
			/* RGB Split ================================================================================================================ */
			GLenum rgbPassTarget[] = { GL_COLOR_ATTACHMENT0 };
			render::initialiseDrawIntoBuffer(m_ShaderList["rgbsplit"]->id(), m_BufferBlur->id, rgbPassTarget, 1);

			// send the textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_GBufferWitoutAliasing->colorTexture); // Activate the texture to send
			glUniform1i(glGetUniformLocation(m_ShaderList["rgbsplit"]->id(), "image"), 0); // Send it to the shader

			GLfloat split = (float)value / 10.0;
			glUniform1f(glGetUniformLocation(m_ShaderList["rgbsplit"]->id(), "split"), split);

			// Send quad and draw
			glBindVertexArray(m_BufferBlur->VAO_id);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);



			/* Store the blurred texture to the G Buffer */
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferWitoutAliasing->id); // Result is going in the non aliased GBuffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_BufferBlur->id); // From the multi sampled aliased GBuffer
			glClearColor(0.5, 0.5, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH);

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
		glDisable(GL_DEPTH);

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

			sprintf_s(m_windowCaption, "%s    FPS: %f", WINDOW_CAPTION, m_frame / (m_elapsedTime / 1000.0));
			SDL_SetWindowTitle(m_SDLWindow, m_windowCaption);

			/*std::cout << "==========================================" << std::endl;
			std::cout << "Scene rendering:        " << m_renderSceneTimer.getElapsedTime() << " ms" << std::endl;
			std::cout << "MSAAfiltering:          " << m_renderAntiAliasingTimer.getElapsedTime() << " ms" << std::endl;
			std::cout << "Post-process rendering: " << m_renderPostProcessTimer.getElapsedTime() << " ms" << std::endl;
			std::cout << "==========================================" << std::endl;*/

			m_secondTimer.start();
		}

		// Every 10 sec 
		if (m_elapsedTime > 10000) {
			m_elapsedTime = 0;
			m_frame = 0;
		}

	} while (m_InputHandler->isNotQuit());
}