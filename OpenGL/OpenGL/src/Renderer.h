#pragma once

#include <glad/glad.h>

#ifdef _DEBUG
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
		x;\
		ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCALL(x) x
#endif



void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
