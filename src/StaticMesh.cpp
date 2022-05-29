#include "StaticMesh.h"

StaticMesh::StaticMesh()
	:
	m_VertexArray(std::make_unique<Gl::VertexArray>()),
	m_IndexBuffer(Gl::IndexBuffer::Create())
{
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);
}

StaticMesh::StaticMesh(const Gl::BufferLayout& layout)
	:
	m_VertexArray(std::make_unique<Gl::VertexArray>()),
	m_IndexBuffer(Gl::IndexBuffer::Create())
{
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	CreateNewVertexBuffer(layout);
}

void StaticMesh::SetDrawType(GLint drawType)
{
	m_DrawType = drawType;
}

uint32_t StaticMesh::CreateNewVertexBuffer(const Gl::BufferLayout& layout)
{
	auto buff = Gl::VertexBuffer::Create(layout);

	m_VertexArray->AddVertexBuffer(buff);
	
	if (m_VertexBuffers.size() == 0)
	{
		m_VertBufferParamLength = layout.GetLength();
	}

	m_VertexBuffers.push_back(buff);

	return m_VertexBuffers.size() - 1;
}

void StaticMesh::UploadIndexData(std::vector<uint32_t>& vec)
{
	m_IndexCount = vec.size();
	m_IndexBuffer->SetData(vec, m_IndexCount);
}

void StaticMesh::UploadIndexData(uint32_t* data, size_t count)
{
	m_IndexCount = count;
	m_IndexBuffer->SetData(data, count);
}

void StaticMesh::DrawArrays()
{
	m_VertexArray->Bind();
	glDrawArrays(m_DrawType, 0, m_VertexCount);
}

void StaticMesh::DrawIndexed()
{
	m_VertexArray->Bind();
	glDrawElements(m_DrawType, m_IndexCount, GL_UNSIGNED_INT, 0);
}