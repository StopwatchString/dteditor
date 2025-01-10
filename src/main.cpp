#include "dted/DtedFile.h"
using dted::DtedFile;
#include "icon.h"

#include "dted/dtedParsedStructs.h"
#include "Threadpool.h"

#include "cpputils/windows/dwm.h"
#include "glh/classes/OpenGLApplication.h"
#include "cpputils/Timer.h"

#include <iostream>
#include <vector>

const std::string file1 = "data/n39_w084_1arc_v3.dt2";
const std::string file2 = "data/n39_w085_1arc_v3.dt2";
Threadpool threadpool(32);

const std::vector<std::string> getDtedFileDataLines(const DtedFile& file) {
    std::vector<std::string> lines;

    return lines;
}

bool loadNewFile = false;
std::vector<std::string> newTextData;
static void dropCallback(GLFWwindow* window, int count, const char** paths)
{
    DtedFile newFile(paths[0]);
    newFile.loadFile(true);
    if (newFile.valid()) {
        newTextData = getDtedFileDataLines(newFile);
    }
    else {
        newTextData = { newFile.filename(), "Not a valid Dted File!"};
    }
    loadNewFile = true;
}

static void render(GLFWwindow* window) 
{
    glfwMakeContextCurrent(window);

    // Text data
    DtedFile dtedFile1(file1);
    DtedFile dtedFile2(file2);
    for (int i = 0; i < 100; i++) {
        dtedFile1.loadFile(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::vector<std::string> textData = getDtedFileDataLines(dtedFile1);

    ImGuiIO& io = ImGui::GetIO();

    while (!glfwWindowShouldClose(window)) {
        if (loadNewFile) {
            textData = std::move(newTextData);
            loadNewFile = false;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create window which fills viewport
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        for (const auto& line : textData) {
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
}

int main()
{

    // Text data
    DtedFile dtedFile1(file1);
    DtedFile dtedFile2(file2);
    for (int i = 0; i < 100; i++) {
        {
            ScopePrintTimer<std::chrono::steady_clock, std::chrono::milliseconds> timer("Time(ms): ");
            dtedFile1.loadFile(false);
            //dtedFile2.loadFile(false);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    OpenGLApplication::ApplicationConfig appConfig = {};
    appConfig.windowName = "Dteditor";
    appConfig.windowInitWidth = 600;
    appConfig.windowInitHeight = 1000;
    appConfig.windowPosX = 200;
    appConfig.windowPosY = 200;
    appConfig.windowBorderless = false;
    appConfig.windowResizeEnable = true;
    appConfig.windowDarkmode = true;
    appConfig.windowRounded = true;
    appConfig.vsyncEnable = true;
    appConfig.glVersionMajor = 4;
    appConfig.glVersionMinor = 6;
    appConfig.glslVersionString; // leave default
    appConfig.customDrawFunc = render;
    appConfig.customKeyCallback = nullptr;
    appConfig.customErrorCallback = nullptr;
    appConfig.customDropCallback = dropCallback;

    try {
        OpenGLApplication app(appConfig);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}