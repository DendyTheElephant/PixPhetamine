#include "CSkybox.h"

namespace PixPhetamine {

	namespace SceneRendering {

		CSkybox::CSkybox(std::string a_skyboxTexturePathName) {
			std::vector<std::string> skyboxTexturePaths;
			skyboxTexturePaths.push_back(a_skyboxTexturePathName + TEXTURE_SUFFIX_RIGHT);
			skyboxTexturePaths.push_back(a_skyboxTexturePathName + TEXTURE_SUFFIX_LEFT);
			skyboxTexturePaths.push_back(a_skyboxTexturePathName + TEXTURE_SUFFIX_TOP);
			skyboxTexturePaths.push_back(a_skyboxTexturePathName + TEXTURE_SUFFIX_BOTTOM);
			skyboxTexturePaths.push_back(a_skyboxTexturePathName + TEXTURE_SUFFIX_BACK);
			skyboxTexturePaths.push_back(a_skyboxTexturePathName + TEXTURE_SUFFIX_FRONT);
			m_texture = new LowLevelWrapper::CSkyboxTexture(skyboxTexturePaths);

			static std::string vertex =
				"#version 330\n"
				"layout(location = 0) in vec3 in_vertex_position;\n"
				"out vec3 skybox_texture_coords;\n"
				"uniform mat4 VP;\n"
				"uniform vec3 camera_position;"
				"void main() {\n"
				"	gl_Position = VP * vec4(camera_position - in_vertex_position, 1.0);\n"
				"	skybox_texture_coords = in_vertex_position ;\n"
				"	skybox_texture_coords.y = -skybox_texture_coords.y;\n"
				"}";

			static std::string fragment =
				"#version 330\n"
				"in vec3 skybox_texture_coords;"
				"layout(location = 0) out vec4 outColor;"
				"uniform samplerCube skybox;"
				"void main() {"
				"	outColor = texture(skybox, skybox_texture_coords);"
				"}";

			m_shader = new LowLevelWrapper::CShader("skyboxShader.vert", vertex, "skyboxShader.frag", fragment);
			m_shader->bindVariableName("skybox");
			m_shader->bindVariableName("VP");
			m_shader->bindVariableName("camera_position");
		}

		CSkybox::~CSkybox() {
			delete m_texture;
			delete m_shader;
		}

		void CSkybox::draw(pxMat4f a_modelViewMatrix, pxVec3f a_cameraPosition) {
			glDepthMask(GL_FALSE);
			glUseProgram(m_shader->id());
			glActiveTexture(GL_TEXTURE0);

			m_shader->sendVariable("VP", a_modelViewMatrix);
			m_shader->sendVariable("camera_position", a_cameraPosition);
			
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture->getID());
			glBindVertexArray(m_cube.getVAO());

			glDrawArrays(GL_TRIANGLES, 0, 36);

			glBindVertexArray(0);
			glUseProgram(0);
			glDepthMask(GL_TRUE);
		}

	}
}
