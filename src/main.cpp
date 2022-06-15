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

inline void CreateColoredQuad(const DMeshPtr& mesh, std::array<glm::vec3, 4> clr, std::array<glm::vec3, 4> points)
{
    mesh->AddVertex(points[0]);
    auto p0 = mesh->AddVertex(clr[0]);
    mesh->AddVertex(points[1]);
    auto p1 = mesh->AddVertex(clr[1]);
    mesh->AddVertex(points[2]);
    auto p2 = mesh->AddVertex(clr[2]);
    mesh->AddVertex(points[3]);
    auto p3 = mesh->AddVertex(clr[3]);

    mesh->ConnectVertices(p0, p1, p2);
    mesh->ConnectVertices(p2, p3, p0);
}

inline DMeshPtr Generate3DCube(std::array<glm::vec3, 8> clrs)
{
    DMeshPtr mesh = std::make_unique<DynamicMesh>(Gl::BufferLayout{
        { Gl::ShaderDataType::Float3, "position" },
        { Gl::ShaderDataType::Float3, "color" }
    });

    for (int i = 0; i < 2; i++)
    {
        float z = i == 0 ? 1.f : -1.f;

        CreateColoredQuad(mesh, { clrs[0], clrs[1], clrs[2], clrs[3] }, {
            glm::vec3{ -1.f,  1.f, z },
            glm::vec3{  1.f,  1.f, z },
            glm::vec3{  1.f, -1.f, z },
            glm::vec3{ -1.f, -1.f, z },
        });
    }

    for (int i = 0; i < 2; i++)
    {
        float x = i == 0 ? -1.f : 1.f;

        CreateColoredQuad(mesh, { clrs[0], clrs[1], clrs[2], clrs[3] }, {
            glm::vec3{ x,  1.f,  1.f },
            glm::vec3{ x,  1.f, -1.f },
            glm::vec3{ x, -1.f, -1.f },
            glm::vec3{ x, -1.f,  1.f },
        });
    }

    for (int i = 0; i < 2; i++)
    {
        float y = i == 0 ? -1.f : 1.f;

        CreateColoredQuad(mesh, { clrs[0], clrs[1], clrs[2], clrs[3] }, {
            glm::vec3{ -1.f, y,  1.f },
            glm::vec3{ -1.f, y, -1.f },
            glm::vec3{  1.f, y, -1.f },
            glm::vec3{  1.f, y,  1.f },
        });
    }

    mesh->Flush();

    return mesh;
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
    auto shader3d = Gl::Shader::FromFiles("shaders/3d_obj.vert", "shaders/3d_obj.frag");

    const float moveSpeed = 1.f;

    auto last = std::chrono::high_resolution_clock::now();

    float dt = 0.016;
    // Rendering Loop
    glm::vec3 pos{ 0.f, 0.f, 0.f };

    auto cube = Generate3DCube({
        glm::vec3{ 1.f, 0.f, 0.f },
        glm::vec3{ 0.f, 1.f, 0.f },
        glm::vec3{ 1.f, 0.f, 1.f },
        glm::vec3{ 1.f, 0.f, 0.f },
        glm::vec3{ 0.f, 1.f, 0.f },
        glm::vec3{ 1.f, 0.f, 0.f },
        glm::vec3{ 0.f, 1.f, 0.f },
        glm::vec3{ 0.f, 1.f, 0.f },
    });

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.1f, 100.0f);
    glm::mat4 transform = glm::mat4(1.f);

    glEnable(GL_DEPTH_TEST);

    glm::vec2 rotSpeed = { 25.f, 30.f };

    while (glfwWindowShouldClose(mWindow) == false) 
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto sec = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
        last = now;

        float dt = sec * 1.f / 1000;

        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        transform = glm::rotate(transform, glm::radians(rotSpeed.x * dt), glm::vec3{ 1.f, 0.f, 0.f });
        transform = glm::rotate(transform, glm::radians(rotSpeed.y * dt), glm::vec3{ 0.f, 1.f, 0.f });

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader3d.Bind();
        shader3d.SetMat4("transform", transform);
        shader3d.SetMat4("view", view);
        shader3d.SetMat4("proj", proj);

        cube->DrawIndexed();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(mWindow);
	glfwTerminate();
    return EXIT_SUCCESS;
}
