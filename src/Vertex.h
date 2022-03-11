#pragma once

#include <glm/glm.hpp>

using Vertex1f = float;
using Vertex2f = glm::vec2;
using Vertex3f = glm::vec3;
using Vertex4f = glm::vec4;
using Vertex1i = int;
using Vertex2i = struct { int x; int y; };
using Vertex3i = struct { int x; int y; int z; };
using Vertex4i = struct { int x; int y; int z; int w; };
using VertexM4 = glm::mat4;
using VertexM3 = glm::mat3;
using Vertex1b = bool;

static constexpr int VertexTypeCount = 11;

// matrices not supported as of now
template<typename T>
constexpr int GetVertexSize()
{
	if constexpr (
		std::is_same_v<T, Vertex2f> ||
		std::is_same_v<T, Vertex2i>
		)
	{
		return 2;
	}
	else if constexpr (
		std::is_same_v<T, Vertex3f> ||
		std::is_same_v<T, Vertex3i>
		)
	{
		return 3;
	}
	else if constexpr (
		std::is_same_v<T, Vertex4f> ||
		std::is_same_v<T, Vertex4i>
		)
	{
		return 4;
	}

	return 1; // matrices not supported as of now
}

template<typename T>
constexpr bool IsVertexFloat()
{
	return
		std::is_same_v<T, Vertex1f> ||
		std::is_same_v<T, Vertex2f> ||
		std::is_same_v<T, Vertex3f> ||
		std::is_same_v<T, Vertex4f>;
}

template<typename T>
constexpr bool IsVertexInt()
{
	return
		std::is_same_v<T, Vertex1i> ||
		std::is_same_v<T, Vertex2i> ||
		std::is_same_v<T, Vertex3i> ||
		std::is_same_v<T, Vertex4i>;
}