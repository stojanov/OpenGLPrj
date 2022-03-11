// Local Headers
#include "OpenGLPrj.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include "DynamicMesh.h"
#include "Shader.h"

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

    DynamicMesh mesh({
        { Gl::ShaderDataType::Float3, "position" },
		{ Gl::ShaderDataType::Float3, "color" } 
    });

    mesh.AddVertex(glm::vec3{ -0.5f, -0.5f, 0.0f, });
    mesh.AddVertex(glm::vec3{ 1.f, 0.f, 0.f  });

    mesh.AddVertex(glm::vec3{ 0.5f, -0.5f, 0.0f, });
    mesh.AddVertex(glm::vec3{ 0.f, 1.f, 0.f });

    mesh.AddVertex(glm::vec3{ 0.0f,  0.5f, 0.0f });
    mesh.AddVertex(glm::vec3{ 0.f, 0.f, 1.f });
    
    mesh.FlushVertexData();

    auto shader = Gl::Shader::FromFiles("shaders/triangle.vert", "shaders/triangle.frag");

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Bind();
        mesh.DrawArrays();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }   glfwTerminate();
    return EXIT_SUCCESS;
}
