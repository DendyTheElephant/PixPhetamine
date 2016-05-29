/// \file		HInternalTypesAliases.h
///	\author		Daniel Huc
/// \date		May 2016
/// \brief		Aliases for types
/// \details	Using standart types may cause confusion in memory representation (depending on system) \n
///				Plus if some mathematical functions will be changed in the future, we want to avoid multiple modifications!
#pragma once

/* Standard library includes */
#include <vector>

/* External dependencies */
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


/* Substitution Aliases */
using pxBool		= GLboolean;		/// 1b boolean GL_TRUE or GL_FALSE
using pxInt16		= GLshort;			/// Signed 16b integer [-32 768,  32 767]
using pxUInt16		= GLushort;			/// Unsigned 16b integer [0,  65 535]
using pxInt			= GLint;			/// Signed 32b integer [-2 147 483 648,  2 147 483 647]
using pxUInt		= GLuint;			/// Unsigned 32b integer [0,  4 294 967 295]
using pxInt64		= GLint64;			/// Signed 64b integer [-9 223 372 036 854 775 808,  9 223 372 036 854 775 807]
using pxUInt64		= GLuint64;			/// Unsigned 64b integer [0,  18 446 744 073 709 551 615]
using pxFloat		= GLfloat;			/// Signed 32b floating-point
using pxFloat64		= GLdouble;			/// Signed 64b floating-point
using pxVec2i		= glm::i32vec2;		/// Signed 32*2b integer vector
using pxVec2f		= glm::f32vec2;		/// Signed 32*2b floating-point vector
using pxVec3f		= glm::f32vec3;		/// Signed 32*3b floating-point vector
using pxVec3f64		= glm::f64vec3;		/// Signed 64*3b floating-point vector
using pxMat4f		= glm::f32mat4;		/// Signed 32*4*4b floating-point matrix

using pxFloatArray	= std::vector<pxFloat>;
using pxUInt16Array = std::vector<pxUInt16>;

namespace MathPhetamine {
	// Function alias for (overloaded) functions
	const auto& normalize	= static_cast<pxVec3f(*)(pxVec3f const &vec)>							(glm::normalize);
	const auto& cross		= static_cast<pxVec3f(*)(pxVec3f const &vec1, pxVec3f const &vec2)>		(glm::cross);
	const auto& mod			=																		(std::fmodf);
	const auto& toRadians	= static_cast<pxFloat(*)(pxFloat const &value)>							(glm::radians);
	const auto& clamp		= static_cast<pxFloat(*)(pxFloat const &value, pxFloat const &min,
								pxFloat const &max)>												(glm::clamp);
}


/* Polymorph Aliases : same as referenced but another name (human sense) */

namespace PixPhetamine {
	using GLvramLocation = GLuint;		/// Used for OpenGL bindings
}




