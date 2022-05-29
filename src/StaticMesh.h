#pragma once 

#include <assert.h>
#include <glad/glad.h>

#include "VertexBuffer.h"
#include "VertexArray.h"

class StaticMesh
{
public:
	StaticMesh();
	StaticMesh(const Gl::BufferLayout& layout);

	uint32_t CreateNewVertexBuffer(const Gl::BufferLayout& layout);
	void SetDrawType(GLint drawType);
	void DrawIndexed();
	void DrawArrays();

	// Every upload function will overwrite previous data 
	template<typename T>
	void UploadVertexData(uint32_t bufIdx, std::vector<T>& vec)
	{
		assert(bufIdx >= 0 && bufIdx < m_VertexBuffers.size());

		if (bufIdx == 0)
		{
			m_VertexCount = vec.size() / m_VertBufferParamLength;
		}

		m_VertexBuffers[bufIdx]->SetData(vec, vec.size());
	}

	template<typename T>
	void UploadVertexData(std::vector<T>& vec)
	{
		UploadVertexData(0, vec);
	}

	void UploadIndexData(std::vector<uint32_t>& vec);
	void UploadIndexData(uint32_t* data, size_t size);
private:
	int m_VertBufferParamLength{ 0 };
	int m_IndexCount{ 0 };
	int m_VertexCount{ 0 };
	std::vector<std::shared_ptr<Gl::VertexBuffer>> m_VertexBuffers;
	std::unique_ptr<Gl::VertexArray> m_VertexArray;
	std::shared_ptr<Gl::IndexBuffer> m_IndexBuffer;
	GLint m_DrawType{ GL_TRIANGLES };
};