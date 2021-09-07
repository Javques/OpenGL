/*THIS ABSTRACT CLASS IS BASED ON THE FOLLOWING OPENGL SERIES ON YOUTUBE BY THE CHERNO.
  https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&ab_channel=TheCherno
  SLIGHT MODIFICATIONS HAVE BEEN MADE. THIS INFORMATION HAS BEEN DISCLOSED TO THE PROFESSOR ALREADY.*/
#pragma once
#include <glew/glew.h>


#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLCLearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))

void GLCLearError();
bool GLLogCall(const char* function, const char* file, int line);


