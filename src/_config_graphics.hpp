#pragma once

// --- START CONFIG ---
// Select exactly one Graphics API (by uncommenting api you want to use)

// #define USE_GAPI_OPENGL
// #define USE_GAPI_VULKAN

// --- END CONFIG ---

#if defined(USE_GAPI_OPENGL) && defined(USE_GAPI_VULKAN)
#error "Only one graphics API can be selected"
#endif

#if !defined(USE_GAPI_OPENGL) && !defined(USE_GAPI_VULKAN)
#pragma message("No graphics API defined, defaulting to OpenGL")
#define USE_GAPI_OPENGL
#endif
