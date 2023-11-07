#ifndef GRAPHICS_DEBUG_H
#define GRAPHICS_DEBUG_H

#include <glad/glad.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)
#define glFunction(func, ...) func(__VA_ARGS__); CHECK_GL_ERROR()

#endif //GRAPHICS_DEBUG_H
