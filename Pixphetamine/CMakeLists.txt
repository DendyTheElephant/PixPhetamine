CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (Pixphetamine)
SET (EXE_NAME Pixphetamine)
ADD_EXECUTABLE (EXE_NAME)

# We want to use c++11 ! 
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Debug mode
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#SET (CMAKE_BUILD_TYPE Release)
SET (CMAKE_BUILD_TYPE Debug)

# Is that really necessary since we include glew? (part of GL thats missing from standard openGL)
FIND_PACKAGE (OpenGL REQUIRED)


# Paths definitions
SET (PROJECT_DIR .)
SET (PROJECT_SRC_DIR ${PROJECT_DIR}/code/src)
SET (PROJECT_HDR_DIR ${PROJECT_DIR}/code/hdr)
# For the moment libs are dynamics (and no CMake compilation (maybe better to think about crossPlotform libs))
if (WIN32) # May be better check if it is MSVC! (might be MinGW)
	SET (PROJECT_LIB_DIR ${PROJECT_DIR}/libs/VC)
endif()
if (UNIX) # FIXME
	SET (PROJECT_LIB_DIR ${PROJECT_DIR}/libs/UNIX)
endif()
if (MAC) # FIXME
	SET (PROJECT_LIB_DIR ${PROJECT_DIR}/libs/MAC)
endif()
	

# Include the headers (and library headers)
INCLUDE_DIRECTORIES (${PROJECT_HDR_DIR})
INCLUDE_DIRECTORIES (${PROJECT_LIB_DIR}/glm)
INCLUDE_DIRECTORIES (${PROJECT_LIB_DIR}/SDL/include)
INCLUDE_DIRECTORIES (${PROJECT_LIB_DIR}/glew)
INCLUDE_DIRECTORIES (${PROJECT_LIB_DIR}/SDL_image/include)

# Link the .lib (Windows only???)
if (WIN32) # May be better check if it is MSVC! (might be MinGW)
	LINK_DIRECTORIES (${PROJECT_LIB_DIR}/SDL/lib)
	LINK_DIRECTORIES (${PROJECT_LIB_DIR}/glew/lib)
	LINK_DIRECTORIES (${PROJECT_LIB_DIR}/SDL_image/lib)
endif()

# ??? must be libs (static?) on MSVC but on UNIX looks like -Dcimg_use_png or ${SYSTEM_LIBS}
TARGET_LINK_LIBRARIES (EXE_NAME opengl32.lib)
opengl32.lib;
glu32.lib;
glew32.lib;
SDL2main.lib;
SDL2.lib;
SDL2_image.lib;
# Is it really necessary since we provide .dll with the exe (.so libs? on UNIX) Shared libs


