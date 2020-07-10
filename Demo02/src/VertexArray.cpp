#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RenderID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RenderID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		const auto& e = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, e.count, e.type, e.normalized, \
			layout.GetStride(), (const void*)offset));
		offset += e.count * VertexBufferElement::GetSizeOfType(e.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RenderID));
}

void VertexArray::UnBind() const
{
	GLCall(glBindVertexArray(0));
}

