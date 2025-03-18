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
#include <iomanip>

const std::string file1 = "../data/n39_w084_1arc_v3.dt2";
const std::string file2 = "../data/n39_w085_1arc_v3.dt2";
Threadpool threadpool(32);

const std::vector<std::string> getDtedFileDataLines(const DtedFile& file)
{
    std::vector<std::string> lines;

    return lines;
}

bool loadNewFile = false;
std::vector<std::string> newTextData;
static void dropCallback(GLFWwindow* window, int count, const char** paths)
{
    DtedFile newFile(paths[0]);
    newFile.loadFile(DtedFile::LoadStrategy::STL_IFSTREAM);
    if (newFile.valid()) {
        newTextData = getDtedFileDataLines(newFile);
    }
    else {
        newTextData = {newFile.filename(), "Not a valid Dted File!"};
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
        dtedFile1.loadFile(DtedFile::LoadStrategy::STL_IFSTREAM);
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
        ImGui::Begin(
            "Main Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

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
    static const size_t ITERS = 100;
    cpputils::ResettableTimer<std::chrono::steady_clock> timer;
    DtedFile dtedFile(file1);

    std::cout << "Average over " << ITERS << " iterations:" << std::endl;
    std::cout << std::fixed << std::setprecision(3);

    std::cout << std::endl << "CACHE DEPENDENT" << std::endl;

    timer.reset();
    for (int i = 0; i < ITERS; i++) {
        dtedFile.loadFile(DtedFile::LoadStrategy::STL_IFSTREAM);
    }
    double ifstreamTime = (double)timer.getElapsedTimeMs() / ITERS;
    std::cout << std::left << std::setw(60) << "ifstream: " << std::right << std::setw(12) << ifstreamTime << "ms"
              << std::endl;

    timer.reset();
    for (int i = 0; i < ITERS; i++) {
        dtedFile.loadFile(DtedFile::LoadStrategy::WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN);
    }
    double windowsMemoryMapBufferedSequentialScanTime = (double)timer.getElapsedTimeMs() / ITERS;
    std::cout << std::left << std::setw(60) << "windows memory map buffered sequential scan: " << std::right
              << std::setw(12) << windowsMemoryMapBufferedSequentialScanTime << "ms" << std::endl;

    timer.reset();
    for (int i = 0; i < ITERS; i++) {
        dtedFile.loadFile(DtedFile::LoadStrategy::WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN_PREFETCH);
    }
    double windowsMemoryMapBufferedSequentialScanPrefetchTime = (double)timer.getElapsedTimeMs() / ITERS;
    std::cout << std::left << std::setw(60) << "windows memory map buffered sequential scan (prefetch): " << std::right
              << std::setw(12) << windowsMemoryMapBufferedSequentialScanPrefetchTime << "ms" << std::endl;

    std::cout << std::endl << "CACHE INDEPENDENT" << std::endl;

    timer.reset();
    for (int i = 0; i < ITERS; i++) {
        dtedFile.loadFile(DtedFile::LoadStrategy::WINDOWS_MEMORY_MAP_NON_BUFFERED);
    }
    double windowsMemmoryMapNonBufferedTime = (double)timer.getElapsedTimeMs() / ITERS;
    std::cout << std::left << std::setw(60) << "windows memory map non-buffered: " << std::right << std::setw(12)
              << windowsMemmoryMapNonBufferedTime << "ms" << std::endl;

    timer.reset();
    for (int i = 0; i < ITERS; i++) {
        dtedFile.loadFile(DtedFile::LoadStrategy::WINDOWS_MEMORY_MAP_NON_BUFFERED_PREFETCH);
    }
    double windowsMemmoryMapNonBufferedPrefetchTime = (double)timer.getElapsedTimeMs() / ITERS;
    std::cout << std::left << std::setw(60) << "windows memory mapped non-buffered (prefetch): " << std::right
              << std::setw(12) << windowsMemmoryMapNonBufferedPrefetchTime << "ms" << std::endl;

    timer.reset();
    for (int i = 0; i < ITERS; i++) {
        dtedFile.loadFile(DtedFile::LoadStrategy::WINDOWS_DIRECT_READ_NON_BUFFERED);
    }
    double windowsDirectReadTime = (double)timer.getElapsedTimeMs() / ITERS;
    std::cout << std::left << std::setw(60) << "windows direct read non-buffered: " << std::right << std::setw(12)
              << windowsDirectReadTime << "ms" << std::endl;

    timer.reset();
    for (int i = 0; i < ITERS; i++) {
        dtedFile.loadFile(DtedFile::LoadStrategy::WINDOWS_DIRECT_READ_NON_BUFFERED_OVERLAPPED);
    }
    double windowsDirectReadOverlappedTime = (double)timer.getElapsedTimeMs() / ITERS;
    std::cout << std::left << std::setw(60) << "windows direct read non-buffered overlapped: " << std::right << std::setw(12)
              << windowsDirectReadOverlappedTime << "ms" << std::endl;

    // OpenGLApplication::ApplicationConfig appConfig = {};
    // appConfig.windowName = "Dteditor";
    // appConfig.windowInitWidth = 600;
    // appConfig.windowInitHeight = 1000;
    // appConfig.windowPosX = 200;
    // appConfig.windowPosY = 200;
    // appConfig.windowBorderless = false;
    // appConfig.windowResizeEnable = true;
    // appConfig.windowDarkmode = true;
    // appConfig.windowRounded = true;
    // appConfig.vsyncEnable = true;
    // appConfig.glVersionMajor = 4;
    // appConfig.glVersionMinor = 6;
    // appConfig.dearImguiGlslVersionString; // leave default
    // appConfig.customDrawFunc = render;
    // appConfig.customKeyCallback = nullptr;
    // appConfig.customErrorCallback = nullptr;
    // appConfig.customDropCallback = dropCallback;

    // try {
    //     OpenGLApplication app(appConfig);
    // }
    // catch (const std::exception& e) {
    //     std::cout << e.what() << std::endl;
    // }

    return EXIT_SUCCESS;
}
