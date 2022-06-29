#include "FPSCamera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(float fov, int width, int height, float clipNear, float clipFar)
{
	m_Proj = glm::perspective(glm::radians(fov), (width * 1.f / height), clipNear, clipFar);
}

void Camera::SetPos(const glm::vec3& pos)
{
	m_Pos = pos;
}

void Camera::OffsetPosition(const glm::vec3& pos)
{
	m_Pos += pos;
}

void Camera::Rotate(const glm::vec2& rot)
{
	m_DRot += rot;
}

void Camera::MoveX(float speed)
{
	m_DPos += glm::normalize(glm::cross(m_Front, m_Up)) * speed;
}

void Camera::MoveY(float speed)
{
	m_DPos += glm::normalize(m_Up) * speed;
}

void Camera::MoveZ(float speed)
{
	m_DPos += glm::normalize(m_Front) * speed;
}

void Camera::Update(float dt)
{
	m_Pos += m_DPos;
	m_Rot += m_DRot;


	/*
	glm::vec3 axis = glm::cross(m_Front, m_Up);
	glm::quat pitch = glm::angleAxis(m_DRot.y, axis);
	glm::quat heading = glm::angleAxis(m_DRot.x, m_Up);

	glm::quat rotation = glm::normalize(glm::cross(pitch, heading));

	auto newFront = glm::rotate(rotation, m_Front);

	m_LookAt = m_Pos + newFront;
	m_View = glm::lookAt(m_Pos, m_LookAt, m_Up);
	*/


	m_Front = glm::rotate(m_Front, glm::radians(-1.f * m_DRot.x), { 0.f, 1.f, 0.f });
	m_LookAt = glm::rotate(m_LookAt, glm::radians(-1.f * m_DRot.x), { 0.f, 1.f, 0.f });
	m_LookAt = glm::rotate(m_LookAt, glm::radians(m_DRot.y), { 1.f, 0.f, 0.f });

	m_View = glm::lookAt(m_Pos, m_Pos + m_LookAt, m_Up);

	m_DPos = { 0.f, 0.f, 0.f };
	m_DRot = { 0.f, 0.f};
}