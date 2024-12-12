#include "DtedFile.h"

#include <iostream>
#include <vector>

// My stuff
const std::string file = "data/n39_w084_1arc_v3.dt2";
template <size_t S>
std::string bytesToString(const std::array<std::byte, S>& bytes) {
    std::string s;
    s.reserve(S);
    for (const std::byte& b : bytes) {
        s.push_back(static_cast<char>(b));
    }
    return s;
}

const std::vector<std::string> getDtedFileDataLines(const DtedFile& file) {
    std::vector<std::string> lines;

    // User Header Label
    lines.push_back("----User Header Label----");
    lines.push_back("");

    // Data Set Identification
    lines.push_back("----Data Set Identification----");
    lines.push_back("");

    // Accuracy Description Record
    lines.push_back("----Accuracy Description Record----");
    lines.push_back("");

    return lines;
}

// DearImgui/GLFW Stuff
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int main()
{
    DtedFile dtedFile(file);

    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    const char* glsl_version = "#version 460";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    GLFWwindow* window = glfwCreateWindow(720, 1080, "Dteditor", nullptr, nullptr);
    if (window == nullptr) {
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Text data
    std::vector<std::string> text_data = getDtedFileDataLines(dtedFile);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create a fullscreen window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Fullscreen Text Display", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        for (const auto& line : text_data) {
            ImGui::Text("%s", line.c_str());
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}