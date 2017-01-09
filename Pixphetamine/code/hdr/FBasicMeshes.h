/// \file		FBasicMeshes.h
///	\author		Daniel Huc
/// \date		January 2017
#pragma once

/* External dependencies */
#include <GL/glew.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		/// \brief		Basic Geometry
		/// \details	Allocate the geometry in VRAM
		class VBasicMesh {
		/* Members */
		protected:
			GLvramLocation m_VAOid{ 0 };				///< ID of the VAO, used to reference the active VBO in the rendering pipeline
			GLvramLocation m_VBOVertices{ 0 };			///< Buffer containing the vertices
		/* Methods */
		protected:
			void sendToVRAM(std::vector<pxFloat> vertices);
			void deleteFromVRAM();
		public:
			VBasicMesh() {};
			virtual ~VBasicMesh() = 0;
			GLvramLocation getVAO() { return m_VAOid; }
		};

		class CBasicMeshQuad: public VBasicMesh {
		/* Methods */
		public:
			CBasicMeshQuad();
			~CBasicMeshQuad() { deleteFromVRAM(); }
		};

		class CBasicMeshCube : public VBasicMesh {
			/* Methods */
		public:
			CBasicMeshCube();
			~CBasicMeshCube() { deleteFromVRAM(); }
		};
	}
}