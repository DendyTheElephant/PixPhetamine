/// \file		HConstants.h
///	\author		Daniel Huc
/// \date		January 2017
#pragma once

/* Standard library includes */

/* External dependencies */

/* Internal headers includes */
#include "HInternalTypesAliases.h"

namespace PixPhetamine {

	namespace PIX {
		constexpr pxUInt16	MSAA_X							{ 8 };
		constexpr pxUInt16	WINDOW_WIDTH					{ 1200 };
		constexpr pxUInt16	WINDOW_HEIGHT					{ 800 };
		constexpr char		WINDOW_CAPTION[]				{ "Sick Beam Phetamine" };
		constexpr char		SHADERS_FOLDER[]				{ "shaders/" };
		constexpr char		MODELS_FOLDER[]					{ "models/" };
		constexpr char		MODELS_EXTENSION[]				{ ".lowpoly" };
		constexpr char		SHADER_FRAGMENT_EXTENSION[]		{ ".frag" };
		constexpr char		SHADER_VERTEX_EXTENSION[]		{ ".vert" };
		constexpr pxFloat	DOWNSCALING						{ 4.0 };

		constexpr char		SKYBOX_SUFFIX_RIGHT[]			{ "Right.png" };
		constexpr char		SKYBOX_SUFFIX_LEFT[]			{ "Left.png" };
		constexpr char		SKYBOX_SUFFIX_TOP[]				{ "Top.png" };
		constexpr char		SKYBOX_SUFFIX_BOTTOM[]			{ "Bottom.png" };
		constexpr char		SKYBOX_SUFFIX_BACK[]			{ "Back.png" };
		constexpr char		SKYBOX_SUFFIX_FRONT[]			{ "Front.png" };
	}
}