/*THIS ABSTRACT CLASS IS BASED ON THE FOLLOWING OPENGL SERIES ON YOUTUBE BY THE CHERNO.
  // https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&ab_channel=TheCherno
  SLIGHT MODIFICATIONS HAVE BEEN MADE. THIS INFORMATION HAS BEEN DISCLOSED TO THE PROFESSOR ALREADY.*/

#pragma once
class IndexBuffer {
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};