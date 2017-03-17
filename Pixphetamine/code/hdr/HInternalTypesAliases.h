////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////====================================================================================================================================////
/// | \file		DInternalTypesAliases
///	| \author	Daniel Huc
/// | \date		May 2016
/// |----------------------------------------------------------------------------------------------------------------------------------
/// | \brief	Aliases for types
/// | \details	Using standard types may cause confusion in memory representation (depending on system) \n
///	|			Plus if some mathematical functions will be changed in the future, we want to avoid multiple modifications!
////====================================================================================================================================////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

/* Standard library includes */
#include <vector>
#include <memory>
#include <map>

/* External dependencies */
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

/* Substitution Aliases */
using pxBool = GLboolean;			/// 1b boolean GL_TRUE or GL_FALSE
using pxInt16 = int_fast16_t;		/// Signed 16b integer [-32 768,  32 767]
using pxUInt16 = uint_fast16_t;		/// Unsigned 16b integer [0,  65 535]
using pxInt = int_fast32_t;			/// Signed 32b integer [-2 147 483 648,  2 147 483 647]
using pxUInt = uint_fast32_t;		/// Unsigned 32b integer [0,  4 294 967 295]
using pxInt64 = int_fast64_t;		/// Signed 64b integer [-9 223 372 036 854 775 808,  9 223 372 036 854 775 807]
using pxUInt64 = uint_fast64_t;		/// Unsigned 64b integer [0,  18 446 744 073 709 551 615]
using pxFloat = GLfloat;			/// Signed 32b floating-point
using pxFloat64 = GLdouble;			/// Signed 64b floating-point
using pxVec2i = glm::i32vec2;		/// Signed 32*2b integer vector
using pxVec2f = glm::f32vec2;		/// Signed 32*2b floating-point vector
using pxVec3f = glm::f32vec3;		/// Signed 32*3b floating-point vector
using pxVec4f = glm::f32vec4;		/// Signed 32*4b floating-point vector
using pxVec3f64 = glm::f64vec3;		/// Signed 64*3b floating-point vector
using pxMat4f = glm::f32mat4;		/// Signed 32*4*4b floating-point matrix

using pxString = std::string;
template<class T, class U>
using pxMap = std::map<T, U>;
template<class T>
using pxVector = std::vector<T>;
template<class T>
using pxUniquePtr = std::unique_ptr<T>;
template<class T>
using pxSharedPtr = std::shared_ptr<T>;

using pxFloatArray = std::vector<pxFloat>;
using pxUInt16Array = std::vector<pxUInt16>;

namespace MathPhetamine {
	// Functions redirection, used for abstraction (= simple modularity!)
	// Will be defined in each .obj that includes this header, but it's only a function pointer so ...
	static pxVec3f(*normalize)(pxVec3f const& vector) = &glm::normalize;
	static pxVec3f(*cross)(pxVec3f const& vector1, pxVec3f const& vector2) = &glm::cross;
	static pxFloat(*mod)(pxFloat value, pxFloat module) = &std::fmod;
	static pxFloat(*toRadians)(pxFloat const& value) = &glm::radians;
	static pxFloat(*clamp)(pxFloat const& value, pxFloat const& min, pxFloat const& max) = &glm::clamp;
}

/* Polymorph Aliases : (human sense) */
namespace PixPhetamine {
	using GLvramLocation = GLuint;		/// Used for OpenGL bindings
}


// Namespaces comments:

/// Rendering engine
namespace PixPhetamine {
	/// Set of abstractions of the OpenGL3.3/SDL2 API
	namespace LowLevelWrapper {}
	/// Handles the display in a window on screen (SDL2)
	namespace Display {}
	/// Post processing module
	namespace PostProcess {}
	/// All that we need to render the scenes
	namespace SceneRendering {}
}
/// Math utilities
namespace MatPhetamine {}
/// Game Engine
namespace GemPhetamine {}
/// Utilities
namespace Utility {}