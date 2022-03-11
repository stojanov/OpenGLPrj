#pragma once

#include <glad/glad.h>

#include "Buffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

namespace Gl
{
	static GLenum OpenGLBaseType(ShaderDataType type)
	{
		static constexpr GLenum glDataType[] = {
			0,			//  None = 0,
			GL_INT,		//	Int,
			GL_INT,		//	Int2,
			GL_INT,		//	Int3,
			GL_INT,		//	Int4,
			GL_FLOAT,	//	Float,
			GL_FLOAT,	//	Float2,
			GL_FLOAT,	//	Float3,
			GL_FLOAT,	//	Float4,
			GL_FLOAT,	//	Mat3, 3 * float3
			GL_FLOAT,	//	Mat4, 4 * float4
			GL_BOOL,	//	Bool
		};

		if (type == ShaderDataType::None)
		{
			std::cerr << "Invalid shader data type :" << static_cast<int>(type) << "\n";
			return glDataType[1];
		}

		return glDataType[static_cast<int>(type)];
	}

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		static std::shared_ptr<VertexArray> Create();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buf);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer);
	private:
		uint32_t m_ID;
		uint32_t m_BufferIndex{ 0 };
		uint32_t m_Stride;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}
