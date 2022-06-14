#include "OpenGLPrj.hpp"

#include <cstdio>
#include <cstdlib>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <iomanip>

#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Shader.h"

#include <chrono>

template<typename T>
using Ptr = std::unique_ptr<T>;

using DMeshPtr = Ptr<DynamicMesh>;

using SMeshPtr = Ptr<StaticMesh>;

static const auto PI = glm::pi<float>();
static const auto PI2 = PI * 2;

const int mWidth = 800;
const int mHeight = 800;

float lerp(float a, float b, float t)
{
    return (b - a) * t + a;
}

DMeshPtr CreatePie(const glm::vec3& clr, const glm::vec2& pos, int samples, float sAngle, float eAngle, float sRadius, float eRadius)
{
    auto mesh = std::make_unique<DynamicMesh>(Gl::BufferLayout({
        { Gl::ShaderDataType::Float3, "position" },
        { Gl::ShaderDataType::Float3, "color" }
        }));

    mesh->SetDrawType(GL_TRIANGLE_STRIP);

    float angleRange = eAngle - sAngle;
    for (int i = 0; i < samples; i++)
    {
        float dA = ((1.f * i) / (samples - 1)) * angleRange + sAngle;

        float x = glm::cos(dA);
        float y = glm::sin(dA);

        mesh->AddVertex(glm::vec3{ pos.x + x * sRadius, pos.y + y * sRadius, 0.f });
        mesh->AddVertex(clr);
        mesh->AddVertex(glm::vec3{ pos.x + x * eRadius, pos.y + y * eRadius, 0.f });
        mesh->AddVertex(clr);
    }

    mesh->FlushVertexData();

    return std::move(mesh);
};

inline void CreateQuad(const DMeshPtr& mesh, glm::vec3& clr, std::array<glm::vec3, 4> points)
{
    mesh->AddVertex(points[0]);
    auto p0 = mesh->AddVertex(clr);
    mesh->AddVertex(points[1]);
    auto p1 = mesh->AddVertex(clr);
    mesh->AddVertex(points[2]);
    auto p2 = mesh->AddVertex(clr);
    mesh->AddVertex(points[3]);
    auto p3 = mesh->AddVertex(clr);

    mesh->ConnectVertices(p0, p1, p2);
    mesh->ConnectVertices(p2, p3, p0);
}

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) { 
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    auto shader = Gl::Shader::FromFiles("shaders/triangle.vert", "shaders/triangle.frag");
    auto checkerShader = Gl::Shader::FromFiles("shaders/checker.vert", "shaders/checker.frag");

    const auto piSection = PI2 / 5;

    const auto pacman = CreatePie({ 1.f, 1.f, 0.f }, { 0.f, 0.f }, 120, piSection, piSection * 5, 0.f, 0.25f);

    bool keys[4] = { 0, 0, 0, 0 };

    const float moveSpeed = 1.f;

    auto last = std::chrono::high_resolution_clock::now();

    float dt = 0.016;
    // Rendering Loop
    glm::vec3 pos{ 0.f, 0.f, 0.f };
    float rotation = 0.f;
    while (glfwWindowShouldClose(mWindow) == false) 
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto sec = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
        last = now;

        float dt = sec * 1.f / 1000;

        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        glm::mat4 transform = glm::mat4(1.f);


        if (!keys[0] && glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            rotation = 90.f;
            keys[0] = true;
        }
        if (!keys[1] && glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            rotation = -90.f;
            keys[1] = true;
        }
        if (!keys[2] && glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS)
        {
            rotation = 0.f;
            keys[2] = true;
        }
        if (!keys[3] && glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            rotation = 180.f;
            keys[3] = true;
        }

        if (keys[0] && glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_RELEASE)
        {
            keys[0] = false;
        }
        if (keys[1] && glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_RELEASE)
        {
            keys[1] = false;
        }
        if (keys[2] && glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_RELEASE)
        {
            keys[2] = false;
        }
        if (keys[3] && glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_RELEASE)
        {
            keys[3] = false;
        }

        glm::vec3 movePos{ 0.f, 0.f, 0.f };

        if (keys[0])
        {
            movePos.x = -1.f;
        }
        if (keys[1])
        {
            movePos.x = 1.f;
        }
        if (keys[2])
        {
            movePos.y = 1.f;
        }
        if (keys[3])
        {
            movePos.y = -1.f;
        }

        pos += movePos * moveSpeed * dt;

        transform = glm::translate(transform, pos);
        transform = glm::rotate(transform, glm::radians(52.f + rotation), { 0.f, 0.f, 1.f });
        
        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Bind();
        shader.SetMat4("transform", transform);
        pacman->DrawArrays();


        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(mWindow);
	glfwTerminate();
    return EXIT_SUCCESS;
}
