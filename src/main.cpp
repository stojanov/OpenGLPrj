#include "OpenGLPrj.hpp"

#include <cstdio>
#include <cstdlib>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>

#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Shader.h"

template<typename T>
using Ptr = std::unique_ptr<T>;

using DMeshPtr = Ptr<DynamicMesh>;

using SMeshPtr = Ptr<StaticMesh>;

static const auto PI = glm::pi<float>();
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

DMeshPtr CreateCircle()
{
    auto mesh = std::make_unique<DynamicMesh>(Gl::BufferLayout({
        { Gl::ShaderDataType::Float3, "position" },
        { Gl::ShaderDataType::Float3, "color" }
    }));

    mesh->SetDrawType(GL_TRIANGLE_STRIP);

    std::vector<glm::vec3> colors{
        { 1.f, 0.f, 0.f },
        { 1.f, 1.f, 0.f },
        { 0.f, 1.f, 0.f },
        { 0.f, 1.f, 1.f },
        { 0.f, 0.f, 1.f },
        { 1.f, 0.f, 1.f },
        { 1.f, 0.f, 0.f },
    };

    const float radius = 0.5;
    const int samples = 360;

    float clrAcc = 0;
    const float clrStep = (1.f * colors.size() - 1) / samples;

    for (int i = 0; i < samples; i++)
    {
        const unsigned sClrIdx = glm::floor(clrAcc);
        const unsigned eClrIdx = glm::ceil(clrAcc);
        const float fracClr = clrAcc - sClrIdx;

        float angle = (1.f * i / (samples - 1)) * (PI * 2);

        glm::vec3 clr{
            lerp(colors[sClrIdx].r, colors[eClrIdx].r, fracClr),
            lerp(colors[sClrIdx].g, colors[eClrIdx].g, fracClr),
            lerp(colors[sClrIdx].b, colors[eClrIdx].b, fracClr),
        };

        mesh->AddVertex(glm::vec3{ 
            glm::cos(angle) * radius,
            glm::sin(angle) * radius,
            0.f
        });

        mesh->AddVertex(clr);
        mesh->AddVertex(glm::vec3{ 0.f, 0.f, 0.f });
        mesh->AddVertex(clr);

        clrAcc += clrStep;
    }

    mesh->FlushVertexData();

    return std::move(mesh);
}

DMeshPtr CreateGradients()
{
    auto mesh = std::make_unique<DynamicMesh>(Gl::BufferLayout({
		{ Gl::ShaderDataType::Float3, "position" },
		{ Gl::ShaderDataType::Float3, "color" }
    }));

    static constexpr auto CreateGradient = [](const DMeshPtr& mesh, int steps, const glm::vec2& pos, const glm::vec2& size, std::array<glm::vec3, 2> clrs)
    {
        const float clrStep = (clrs.size() - 1) * 1.f / steps;

        float clrAcc = 0.f;
        float xOffset = pos.x;
        const float xStep = size.x / steps;

        for (int i = 0; i < steps; i++)
        {
            int prevClr = glm::floor(clrAcc);
        	int nextClr = glm::ceil(clrAcc);
            float fracClr = clrAcc - prevClr;

            glm::vec3 clr = {
                lerp(clrs[prevClr].r, clrs[nextClr].r, fracClr),
                lerp(clrs[prevClr].g, clrs[nextClr].g, fracClr),
                lerp(clrs[prevClr].b, clrs[nextClr].b, fracClr),
            };

            CreateQuad(mesh, clr, {
                glm::vec3{ xOffset, pos.y, 0.f },
                glm::vec3{ xOffset + xStep, pos.y, 0.f },
                glm::vec3{ xOffset + xStep, pos.y + size.y, 0.f },
                glm::vec3{ xOffset, pos.y + size.y, 0.f },
            });

            clrAcc += clrStep;
            xOffset += xStep;
        }
    };

    CreateGradient(mesh, 10, { -0.8f, 0.6f }, { 1.6f, 0.2f }, {
        glm::vec3{ 0.f, 0.f, 0.f },
        glm::vec3{ 1.f, 0.f, 0.f }
    });

    CreateGradient(mesh, 10, { -0.8f, 0.2f }, { 1.6f, 0.2f }, {
		glm::vec3{ 0.f, 0.f, 0.f },
    	glm::vec3{ 0.f, 1.f, 0.f }
    });

    CreateGradient(mesh, 10, { -0.8f, -0.2f }, { 1.6f, 0.2f }, {
		glm::vec3{ 0.f, 0.f, 0.f },
		glm::vec3{ 0.f, 0.f, 1.f }
    });

    mesh->Flush();

    return std::move(mesh);
}

std::vector<DMeshPtr> CreateLogo()
{
    std::vector<DMeshPtr> vec;
    glm::vec3 clr{ 1.f, 1.f, 1.f };

    auto mesh = std::make_unique<DynamicMesh>(Gl::BufferLayout({
        { Gl::ShaderDataType::Float3, "position" },
        { Gl::ShaderDataType::Float3, "color" }
    }));

    CreateQuad(mesh, clr, {
        glm::vec3{ -0.8f, 0.8f, 0.f },
        glm::vec3{ -0.6f, 0.8f, 0.f },
        glm::vec3{ -0.6f, -0.8f, 0.f },
        glm::vec3{ -0.8f, -0.8f, 0.f },
    });

    mesh->Flush();

    vec.push_back(std::move(mesh));
    vec.push_back(CreatePie({ 1.f, 1.f, 1.f }, { 0.4f, 0.f }, 60, 0.f, 2 * PI, 0.6, 0.8));

    return vec;
}

DMeshPtr CreateCheckerTriangle()
{
    auto mesh = std::make_unique<DynamicMesh>(Gl::BufferLayout{
        { Gl::ShaderDataType::Float3, "position" },
		{ Gl::ShaderDataType::Float2, "uv"}
    });

    std::vector vec{
        -0.5f, -0.5f, 0.0f,     0.f, 0.f,
		0.5f, -0.5f, 0.0f,      1.f, 0.f,
		0.0f,  0.5f, 0.0f,      0.5f, 1.0f,
    };

    mesh->AddVertexData(vec);

    mesh->Flush();

    return std::move(mesh);
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

    auto logo = CreateLogo();
	auto gradients = CreateGradients();
    auto circle = CreateCircle();
    auto checkers = CreateCheckerTriangle();

    const std::vector<std::function<void()>> funcs{
        [&circle, &shader]()
        {
            shader.Bind();
            shader.SetInt("use_color", 0);
        	circle->DrawArrays();
        },
        [&logo, &shader]()
        {
            shader.Bind();
            shader.SetInt("use_color", 1);
            shader.SetFloat3("color", { 0.f, 0.f, 0.6f });
            logo[0]->DrawIndexed();

            shader.SetFloat3("color", { 0.f, 0.6f, 0.95f });
            logo[1]->DrawArrays();
        },
        [&gradients, &shader]()
        {
            shader.Bind();
            shader.SetInt("use_color", 0);
	        gradients->DrawIndexed();
        },
        [&checkers, &checkerShader]()
        {
            checkerShader.Bind();
            checkerShader.SetFloat("check_size", 5);
            checkers->DrawArrays();
        }
    };

    int idx = 0;

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        if (glfwGetKey(mWindow, GLFW_KEY_1) == GLFW_PRESS)
            idx = 0;
        if (glfwGetKey(mWindow, GLFW_KEY_2) == GLFW_PRESS)
            idx = 1;
        if (glfwGetKey(mWindow, GLFW_KEY_3) == GLFW_PRESS)
            idx = 2;
        if (glfwGetKey(mWindow, GLFW_KEY_4) == GLFW_PRESS)
            idx = 3;

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        funcs[idx]();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(mWindow);
	glfwTerminate();
    return EXIT_SUCCESS;
}
