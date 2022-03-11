#pragma once

#include <memory>

#include <glad/glad.h>
#include "Buffer.h"

namespace Gl
{
	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer();
		IndexBuffer(uint32_t* indices, uint32_t count);
		IndexBuffer(std::vector<uint32_t>& indices);

		virtual ~IndexBuffer();

		static std::shared_ptr<IndexBuffer> Create(uint32_t* indices, uint32_t count);
		static std::shared_ptr<IndexBuffer> Create();

		void SetData(uint32_t* indices, uint32_t count) const;
		void SetData(std::vector<uint32_t>& indices, uint32_t count) const;

		void Bind() const override;
		void Unbind() const override;
		void Clear() const;
	private:
		void CreateBuffer()
		{
			glCreateBuffers(1, &m_BufferID);
			// Used GL_ARRAY_BUFFER so i can load data without an active VAO
			glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		}

		unsigned m_BufferID;
		uint32_t m_Count;
	};
}
