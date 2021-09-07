/*THIS ABSTRACT CLASS IS BASED ON THE FOLLOWING OPENGL SERIES ON YOUTUBE BY THE CHERNO.
  https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&ab_channel=TheCherno
  SLIGHT MODIFICATIONS HAVE BEEN MADE. THIS INFORMATION HAS BEEN DISCLOSED TO THE PROFESSOR ALREADY.*/

#pragma once

class VertexBuffer {
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};
