#include "VertexArray.h"

#include <assert.h>

namespace Gl
{
	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_ID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_ID);
	}

	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		return std::shared_ptr<VertexArray>();
	}

	void VertexArray::Bind() const
	{
		if (m_IndexBuffer)
		{
			m_IndexBuffer->Bind();
		}
		if (m_VertexBuffer)
		{
			m_VertexBuffer->Bind();
		}
		glBindVertexArray(m_ID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer)
	{
		const auto& layout = buffer->GetLayout();
		assert(layout.GetElements().size());

		Bind();
		buffer->Bind();

		for (const auto& element : layout.GetElements())
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glVertexAttribPointer(m_BufferIndex,
					element.GetComponentCount(),
					OpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.Offset);
				glEnableVertexAttribArray(m_BufferIndex);
				m_BufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glVertexAttribPointer(m_BufferIndex,
						count,
						OpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(sizeof(float) * count * i));
					glEnableVertexAttribArray(m_BufferIndex);
					glVertexAttribDivisor(m_BufferIndex, 1);
					m_BufferIndex++;
				}
				break;
			}
			default:
				std::cerr << "Unsupported data type\n";
				break;
			}
		}
	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer)
	{
		Bind();
		buffer->Bind();
		m_IndexBuffer = buffer;
	}
}