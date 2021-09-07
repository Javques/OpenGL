/*THIS ABSTRACT CLASS IS BASED ON THE FOLLOWING OPENGL SERIES ON YOUTUBE BY THE CHERNO.
  https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&ab_channel=TheCherno
  SLIGHT MODIFICATIONS HAVE BEEN MADE. THIS INFORMATION HAS BEEN DISCLOSED TO THE PROFESSOR ALREADY.*/

#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer {
private:
public:
	void clear() const;
	void DrawTriangleArray(const VertexArray& va,  Shader& shader, unsigned int count);

	void DrawLineArray(const VertexArray& va,  Shader& shader, unsigned int count);
	void DrawTriangle(const VertexArray& va, const IndexBuffer& ib,  Shader& shader) const;
	void DrawLine(const VertexArray& va, const IndexBuffer& ib,  Shader& shader) const;

};