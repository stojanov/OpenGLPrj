#pragma once

#include <iostream>

class IScene
{
public:

	void OnEvent(const Event& e) = 0;
	void Update(float dt) = 0;
	void Render() = 0;

private:
	std::string m_Name;
}