/// \file		CSkybox.h
///	\author		Daniel Huc
/// \date		January 2017
#pragma once

/* Standard library includes */

/* External dependencies */
#include <GL/glew.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"
#include "FBasicMeshes.h"
#include "CSkyboxTexture.h"
#include "CShader.h"

#define TEXTURE_SUFFIX_RIGHT	"Right.png"
#define TEXTURE_SUFFIX_LEFT		"Left.png"
#define TEXTURE_SUFFIX_TOP		"Top.png"
#define TEXTURE_SUFFIX_BOTTOM	"Bottom.png"
#define TEXTURE_SUFFIX_BACK		"Back.png"
#define TEXTURE_SUFFIX_FRONT	"Front.png"

namespace PixPhetamine {

	namespace SceneRendering {

		class CSkybox {
		/* Members */
		private:
			LowLevelWrapper::CBasicMeshCube		m_cube;
			LowLevelWrapper::CSkyboxTexture*	m_texture{ nullptr };
		public:
			LowLevelWrapper::CShader*			m_shader{ nullptr };

		/* Methods */
		public:
			CSkybox(std::string skyboxTexturePathName);
			~CSkybox();

			void draw(pxMat4f modelViewMatrix, pxVec3f cameraPosition);
		};
	}
}