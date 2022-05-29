
#include "DynamicMesh.h"

#include <cassert>

DynamicMesh::DynamicMesh()
	:
	m_VertArray(std::make_unique<Gl::VertexArray>())
{
	m_IdxBuffer = Gl::IndexBuffer::Create();
	m_VertArray->SetIndexBuffer(m_IdxBuffer);
	CreateNewVertexBuffer({
		{ Gl::ShaderDataType::Float3, "position" }
	});

	m_VertBufferParamLength = 1;
}

DynamicMesh::DynamicMesh(const Gl::BufferLayout& layout)
	:
	m_VertArray(std::make_unique<Gl::VertexArray>())
{
	m_IdxBuffer = Gl::IndexBuffer::Create();
	m_VertArray->SetIndexBuffer(m_IdxBuffer);
	CreateNewVertexBuffer(layout);
	m_VertBufferParamLength = layout.GetElements().size();
}

DynamicMesh::DynamicMesh(const DynamicMesh& o)
{
	m_VertBufferParamCount = o.m_VertBufferParamCount;
	m_VertBufferParamLength = o.m_VertBufferParamLength;
	m_DrawType = o.m_DrawType;
	m_VertCount = o.m_VertCount;
	m_ElementCount = o.m_ElementCount;

	m_VertexBuffers.clear();
	m_VertexData.clear();
}


uint32_t DynamicMesh::CreateNewVertexBuffer(const Gl::BufferLayout& layout)
{
	auto buff = Gl::VertexBuffer::Create(layout);

	m_VertexData.emplace_back();

	m_VertArray->AddVertexBuffer(buff);
	m_VertexBuffers.push_back(buff);

	return m_VertexBuffers.size() - 1;
}

void DynamicMesh::AllocateVertexBuffer(uint32_t vertIdx, size_t size)
{
	m_VertexData[vertIdx].reserve(size);
}

void DynamicMesh::AllocateIndexBuffer(size_t size)
{
	m_IndexData.reserve(size);
}

void DynamicMesh::SetDrawType(GLint drawType)
{
	m_DrawType = drawType;
}

void DynamicMesh::AddVertexData(uint32_t vertIdx, std::vector<float>& data)
{
	m_VertexData[vertIdx].insert(m_VertexData[vertIdx].end(), data.begin(), data.end());

	m_VertCount += data.size() / m_VertexBuffers[vertIdx]->GetLayout().GetLength();
}

void DynamicMesh::AddVertexData(std::vector<float>& data)
{
	AddVertexData(0, data);
}

void DynamicMesh::ConnectVertices(uint32_t idx1, uint32_t idx2, uint32_t idx3)
{
	m_IndexData.push_back(idx1);
	m_IndexData.push_back(idx2);
	m_IndexData.push_back(idx3);
	m_ElementCount += 3;
}

void DynamicMesh::FlushVertexData(uint32_t vertIdx)
{
	assert(m_VertexBuffers.size() > vertIdx);

	if (m_VertexData[vertIdx].empty())
	{
		std::cout << "Flushing a vertex buffer that's empty";
	}
	m_VertexBuffers[vertIdx]->SetData(m_VertexData[vertIdx], m_VertexData[vertIdx].size());
}

void DynamicMesh::FlushIndexData()
{
	if (m_IndexData.empty())
	{
		return; 
	}
	m_IdxBuffer->SetData(m_IndexData, m_ElementCount);
}

void DynamicMesh::Flush()
{
	FlushIndexData();

	for (auto i = 0; i < m_VertexData.size(); i++)
	{
		FlushVertexData(i);
	}
}

void DynamicMesh::DrawIndexed() const
{
	m_VertArray->Bind();
	glDrawElements(m_DrawType, m_ElementCount, GL_UNSIGNED_INT, 0);
}

void DynamicMesh::DrawArrays() const
{
	m_VertArray->Bind();
	glDrawArrays(m_DrawType, 0, m_VertCount);
}

// Overwrites the selected draw type and renders data with the draw type from parameter
void DynamicMesh::DrawArrays(GLint type) const
{
	m_VertArray->Bind();
	glDrawArrays(type, 0, m_VertCount);
}

void DynamicMesh::NewMesh()
{
	ClearBuffers();
	ClearGpuBuffers();
	m_ElementCount = 0;
	m_VertCount = 0;
}

void DynamicMesh::ClearBuffers()
{
	for (auto& vec : m_VertexData)
	{
		vec.clear();
	}
	m_IndexData.clear();
}

void DynamicMesh::ClearGpuBuffers()
{
	for (auto& buff : m_VertexBuffers)
	{
		buff->Clear();
	}
	m_IdxBuffer->Clear();
}