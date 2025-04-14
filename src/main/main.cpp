#include "cpputils/windows/simpleDirectIo.h"
#include "glh/glh.h"
#include "cpputils/Timer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

//#include "tracy/Tracy.hpp"

#include <iostream>
#include <array>
#include <thread>
#include <filesystem>
#include <algorithm>

bool hasFileChanged(const std::filesystem::path& path) {
    static std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> lastModifiedTimes;

    if (!std::filesystem::exists(path)) {
        std::cerr << "File does not exist: " << path << std::endl;
        return false;
    }

    auto currentWriteTime = std::filesystem::last_write_time(path);

    auto it = lastModifiedTimes.find(path);
    if (it == lastModifiedTimes.end()) {
        // First time seeing this file
        lastModifiedTimes[path] = currentWriteTime;
        return true; // consider it "changed"
    }

    if (it->second != currentWriteTime) {
        // File changed since last time
        it->second = currentWriteTime;
        return true;
    }

    return false;
}

struct vertex {
    glm::vec3 pos{};
    glm::vec3 color{};
    glm::vec2 texcoord{};
};

constexpr std::array<vertex, 8> cube = {{
        { {-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Front Bottom Left
        { { 0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // Front Bottom Right
        { {-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Front Top Left
        { { 0.5f,  0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}, // Front Top Right
        { {-0.5f, -0.5f,-0.5f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}}, // Back Bottom Left
        { { 0.5f, -0.5f,-0.5f}, {0.3f, 0.3f, 0.3f}, {0.0f, 0.0f}}, // Back Bottom Right
        { {-0.5f,  0.5f,-0.5f}, {0.4f, 0.4f, 0.4f}, {0.0f, 0.0f}}, // Back Top Left
        { { 0.5f,  0.5f,-0.5f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}  // Back Top Right
}};

template <size_t S>
constexpr glm::vec3 calculateCenterpoint(std::array<vertex, S> vertices)
{
    glm::vec3 sum(0.0f);
    for (const vertex& v : vertices) {
        sum += v.pos;
    }
    return sum / static_cast<float>(S);
}

constexpr glm::vec3 centerpoint = calculateCenterpoint(cube);

enum corner {
    FRONT_BOTTOM_LEFT = 0,
    FRONT_BOTTOM_RIGHT = 1,
    FRONT_TOP_LEFT = 2,
    FRONT_TOP_RIGHT = 3,
    BACK_BOTTOM_LEFT = 4,
    BACK_BOTTOM_RIGHT = 5,
    BACK_TOP_LEFT = 6,
    BACK_TOP_RIGHT = 7
};

constexpr std::array<uint32_t, 14> cube_indices
{
    corner::FRONT_BOTTOM_LEFT,
    corner::FRONT_BOTTOM_RIGHT,
    corner::BACK_BOTTOM_LEFT,
    corner::BACK_BOTTOM_RIGHT
};

struct buffers {
    GLuint vbo{ 0 };
    GLuint ebo{ 0 };
    GLuint ubo{ 0 };

    constexpr static GLsizei count() { return 3; }
    GLuint* data() { return reinterpret_cast<GLuint*>(this); }
};

struct matrices {
    glm::mat4 MVP{1};
};

const std::filesystem::path vertexShaderFile = "..\\data\\shaders\\shader.vert";
const std::filesystem::path fragmentShaderFile = "..\\data\\shaders\\shader.frag";

glh::OpenGLApplication::ApplicationConfig config;
bool refreshShaders = false;
bool mouseDrag = false;
double xRot = 0.0;
double yRot = 0.0;
double cameraDistance = 3.0f;

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS){
            mouseDrag = true;
        }
        else if (action == GLFW_RELEASE) {
            mouseDrag = false;
        }
    }
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    static double xprev = 0.0;
    static double yprev = 0.0;

    double xdelta = (xpos - xprev) / 3.0;
    double ydelta = (ypos - yprev) / 3.0;

    if (mouseDrag) {
        xRot += -1.0f * xdelta;// glm::clamp<double>(xRot + (-1.0f * xdelta), -89.9f, 89.9f);
        yRot = glm::clamp<double>(yRot + (-1.0f * ydelta), -89.9f, 89.9f);
    }

    xprev = xpos;
    yprev = ypos;
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (std::abs(yoffset) > 0.95) {
        cameraDistance = std::max(cameraDistance + ((-1.0 * yoffset) / 2), 1.0);
    }
}

void render(GLFWwindow* window)
{
    std::thread refreshShaderThread([&]() {
        while (!glfwWindowShouldClose(window)) {
            refreshShaders = hasFileChanged(vertexShaderFile) || hasFileChanged(fragmentShaderFile);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    cpputils::AlignedBuffer vertexShaderSource = cpputils::win::directFileToBuffer(vertexShaderFile, 1, true);
    cpputils::AlignedBuffer fragmentShaderSource = cpputils::win::directFileToBuffer(fragmentShaderFile, 1, true);

    GLuint vao{ 0 };
    glhGenVertexArrays(1, &vao);
    glhBindVertexArray(vao);

    buffers buffers{};
    glhGenBuffers(buffers.count(), buffers.data());

    glhBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
    glhBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube.data(), GL_STATIC_DRAW);

    glhBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
    glhBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices.data(), GL_STATIC_DRAW);

    matrices matrices{};
    glhBindBuffer(GL_UNIFORM_BUFFER, buffers.ubo);
    glhBufferData(GL_UNIFORM_BUFFER, sizeof(matrices), &matrices, GL_DYNAMIC_DRAW);
    glhBindBufferBase(GL_UNIFORM_BUFFER, 0, buffers.ubo);

    constexpr GLsizei stride = sizeof(vertex);
    glhVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(vertex, pos));
    glhEnableVertexAttribArray(0);
    glhVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(vertex, color));
    glhEnableVertexAttribArray(1);
    glhVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(vertex, texcoord));
    glhEnableVertexAttribArray(2);

    GLint program = glh::createProgram((const char*)vertexShaderSource.buf, (const char*)fragmentShaderSource.buf);

    glhUseProgram(program);

    cpputils::FramerateTimer<std::chrono::steady_clock, 10> framerateTimer;
    while (!glfwWindowShouldClose(window)) {

        if (refreshShaders) {
            glhDeleteProgram(program);
            cpputils::AlignedBuffer vertexShaderSourceRefresh = cpputils::win::directFileToBuffer(vertexShaderFile, 1, true);
            cpputils::AlignedBuffer fragmentShaderSourceRefresh = cpputils::win::directFileToBuffer(fragmentShaderFile, 1, true);
            program = glh::createProgram((const char*)vertexShaderSourceRefresh.buf, (const char*)fragmentShaderSourceRefresh.buf);
            glhUseProgram(program);
            refreshShaders = false;
        }

        // Model
        glm::mat4 model(1.0f);

        // View
        constexpr glm::vec3 target{ centerpoint };  // Center of the shape or scene
        float distance = cameraDistance;       // Zoom level (distance from target)
        float pitch = glm::radians(yRot);    // Vertical angle (X axis)
        float yaw = glm::radians(xRot);    // Horizontal angle (Y axis)

        glm::vec3 cameraPos{};
        cameraPos.x = target.x + distance * cos(pitch) * sin(yaw);
        cameraPos.y = target.y + distance * sin(pitch);
        cameraPos.z = target.z + distance * cos(pitch) * cos(yaw);

        glm::mat4 view = glm::lookAt(cameraPos, target, glm::vec3(0.0f, 1.0f, 0.0f));

        // Projection
        constexpr float fov = glm::radians(45.0f);          // Vertical field of view in radians
        float aspect = (float)config.windowInitWidth / (float)config.windowInitHeight; // Width divided by height
        float n = 0.1f;
        float f = 100.0f;
        glm::mat4 projection = glm::perspective(fov, aspect, n, f);

        matrices.MVP = projection * view * model;


        glhBindBuffer(GL_UNIFORM_BUFFER, buffers.ubo);
        glhBufferData(GL_UNIFORM_BUFFER, sizeof(matrices), &matrices, GL_DYNAMIC_DRAW);

        glhClearColor(0.1176f, 0.1176f, 0.1176f, 1.0f);
        glhClear(GL_COLOR_BUFFER_BIT);

        glhDrawElements(GL_TRIANGLE_STRIP, cube_indices.size(), GL_UNSIGNED_INT, nullptr);

        framerateTimer.markFrame();
        static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - lastTime).count() << std::endl;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime).count() > 250) {
            lastTime = std::chrono::steady_clock::now();
            std::cout << framerateTimer.getFramerate() << std::endl;
        }

        //std::cout << framerateTimer.getFramerate() << std::endl;

        glfwSwapBuffers(window);
    }

    refreshShaderThread.join();
}

int main()
{
    config.windowName =                 "Dted Viewer";
    config.windowInitWidth =            500;
    config.windowInitHeight =           500;
    config.windowPosX =                 1000;
    config.windowPosY =                 100;
    config.windowBorderless =           false;
    config.windowResizeEnable =         false;
    config.windowDarkmode =             true;
    config.windowRounded =              true;
    config.windowAlwaysOnTop =          false;
    config.vsyncEnable =                false;
    config.transparentFramebuffer =     false;
    config.glVersionMajor =             4;
    config.glVersionMinor =             6;
    config.dearImguiGlslVersionString = "#version 460";
    config.imguiIniFileName =           nullptr;
    config.customDrawFunc =             render;
    // std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)>
    config.customKeyCallback =          nullptr;
    config.customMouseButtonCallback =  mouseButtonCallback;
    config.customCursorPosCallback =    cursorPosCallback;
    config.customScrollCallback =       scrollCallback;
    // std::function<void(int error_code, const char* description)>
    config.customErrorCallback =        nullptr;
    // std::function<void(GLFWwindow* window, int count, const char** paths)>
    config.customDropCallback =         nullptr;
    config.customPollingFunc =          nullptr;

    try {
        glh::OpenGLApplication app(config);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
