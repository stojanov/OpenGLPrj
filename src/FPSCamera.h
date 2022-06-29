#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(float fov, int width, int height, float clipNear, float clipFar);

	void SetPos(const glm::vec3& pos);
	void OffsetPosition(const glm::vec3& pos);
	const glm::vec3& GetPos()
	{
		return m_Pos;
	}
	void MoveX(float speed);
	void MoveZ(float speed);
	void MoveY(float speed);

	void Rotate(const glm::vec2& rot);

	const glm::mat4& GetView()
	{
		return m_View;
	}

	const glm::mat4& GetProj()
	{
		return m_Proj;
	}

	void Update(float dt);
private:
	glm::mat4 m_View;
	glm::mat4 m_Proj;

	glm::vec3 m_Up { 0.f, 1.f, 0.f };
	glm::vec3 m_Front { 0.f, 0.f, 1.f };
	glm::vec3 m_LookAt{ 0.f, 0.f, 1.f };

	glm::vec3 m_Pos{ 0.f };
	glm::vec3 m_DPos{ 0.f };
	glm::vec2 m_Rot{ 0.f };
	glm::vec2 m_DRot{ 0.f };
};