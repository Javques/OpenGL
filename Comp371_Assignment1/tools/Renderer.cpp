#include "Renderer.h"


void Renderer::clear() const
{
    //Add function to set clear color perhaps
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::DrawTriangleArray(const VertexArray& va, Shader& shader, unsigned int count)
{
    shader.use();
    va.Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0,count));

}

void Renderer::DrawLineArray(const VertexArray& va,  Shader& shader, unsigned int count)
{
    shader.use();
    va.Bind();
    GLCall(glDrawArrays(GL_LINES, 0, count));
}

void Renderer::DrawTriangle(const VertexArray& va, const IndexBuffer& ib,  Shader& shader) const
{
    shader.use();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount() , GL_UNSIGNED_INT, nullptr));

}

void Renderer::DrawLine(const VertexArray& va, const IndexBuffer& ib,  Shader& shader) const
{
    shader.use();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
