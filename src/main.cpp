#include "DtedFile.h"
#include "icon.h"

#include "cpputils/windows/dwm.h"
#include "glh/classes/OpenGLApplication.h"

#include <iostream>
#include <vector>

//const char* iconPath = "dteditor.ico";
//// Load the .ico file as an HICON
//HICON hIcon = (HICON)LoadImageA(
//    NULL,              // No instance handle required for a file
//    iconPath,          // Path to the .ico file
//    IMAGE_ICON,        // Specify that we're loading an icon
//    0, 0,              // Default size; let Windows scale it
//    LR_LOADFROMFILE |  // Load from file
//    LR_DEFAULTSIZE     // Use default size if not specified
//);
//SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
//SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

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
    lines.push_back("recognitionSentinel: " + bytesToString(file.uhl().recognitionSentinel));
    lines.push_back("fixedStandard: " + bytesToString(file.uhl().fixedStandard));
    lines.push_back("longitudeOfOrigin: " + bytesToString(file.uhl().longitudeOfOrigin));
    lines.push_back("latitudeOfOrigin: " + bytesToString(file.uhl().latitudeOfOrigin));
    lines.push_back("longitudeInterval: " + bytesToString(file.uhl().longitudeInterval));
    lines.push_back("latitudeInterval: " + bytesToString(file.uhl().latitudeInterval));
    lines.push_back("absoluteVerticalAccuracy: " + bytesToString(file.uhl().absoluteVerticalAccuracy));
    lines.push_back("securityCode: " + bytesToString(file.uhl().securityCode));
    lines.push_back("uniqueReference: " + bytesToString(file.uhl().uniqueReference));
    lines.push_back("numberOfLongitudeLines: " + bytesToString(file.uhl().numberOfLongitudeLines));
    lines.push_back("numberOfLatitudePoints: " + bytesToString(file.uhl().numberOfLatitudePoints));
    lines.push_back("multipleAccuracy: " + bytesToString(file.uhl().multipleAccuracy));
    lines.push_back("reserved: " + bytesToString(file.uhl().reserved));
    lines.push_back("");

    // Data Set Identification
    lines.push_back("----Data Set Identification----");
    lines.push_back("recognitionSentinel: " + bytesToString(file.dsi().recognitionSentinel));
    lines.push_back("securityCode: " + bytesToString(file.dsi().securityCode));
    lines.push_back("securityControlMarkings: " + bytesToString(file.dsi().securityControlMarkings));
    lines.push_back("securityHandlingDescription: " + bytesToString(file.dsi().securityHandlingDescription));
    lines.push_back("reserved1: " + bytesToString(file.dsi().reserved1));
    lines.push_back("dtedLevel: " + bytesToString(file.dsi().dtedLevel));
    lines.push_back("uniqueReference: " + bytesToString(file.dsi().uniqueReference));
    lines.push_back("reserved2: " + bytesToString(file.dsi().reserved2));
    lines.push_back("dataEditionNumber: " + bytesToString(file.dsi().dataEditionNumber));
    lines.push_back("matchMergeVersion: " + bytesToString(file.dsi().matchMergeVersion));
    lines.push_back("maintenanceDate: " + bytesToString(file.dsi().maintenanceDate));
    lines.push_back("matchMergeDate: " + bytesToString(file.dsi().matchMergeDate));
    lines.push_back("maintenanceDescCode: " + bytesToString(file.dsi().maintenanceDescCode));
    lines.push_back("producerCode: " + bytesToString(file.dsi().producerCode));
    lines.push_back("reserved3: " + bytesToString(file.dsi().reserved3));
    lines.push_back("productSpec: " + bytesToString(file.dsi().productSpec));
    lines.push_back("productSpecAmend: " + bytesToString(file.dsi().productSpecAmend));
    lines.push_back("productSpecDate: " + bytesToString(file.dsi().productSpecDate));
    lines.push_back("verticalDatum: " + bytesToString(file.dsi().verticalDatum));
    lines.push_back("horizontalDatum: " + bytesToString(file.dsi().horizontalDatum));
    lines.push_back("digitizingSystem: " + bytesToString(file.dsi().digitizingSystem));
    lines.push_back("compilationDate: " + bytesToString(file.dsi().compilationDate));
    lines.push_back("reserved4: " + bytesToString(file.dsi().reserved4));
    lines.push_back("latOrigin: " + bytesToString(file.dsi().latOrigin));
    lines.push_back("lonOrigin: " + bytesToString(file.dsi().lonOrigin));
    lines.push_back("latSWCorner: " + bytesToString(file.dsi().latSWCorner));
    lines.push_back("lonSWCorner: " + bytesToString(file.dsi().lonSWCorner));
    lines.push_back("latNWCorner: " + bytesToString(file.dsi().latNWCorner));
    lines.push_back("lonNWCorner: " + bytesToString(file.dsi().lonNWCorner));
    lines.push_back("latNECorner: " + bytesToString(file.dsi().latNECorner));
    lines.push_back("lonNECorner: " + bytesToString(file.dsi().lonNECorner));
    lines.push_back("latSECorner: " + bytesToString(file.dsi().latSECorner));
    lines.push_back("lonSECorner: " + bytesToString(file.dsi().lonSECorner));
    lines.push_back("orientationAngle: " + bytesToString(file.dsi().orientationAngle));
    lines.push_back("latitudeInterval: " + bytesToString(file.dsi().latitudeInterval));
    lines.push_back("longitudeInterval: " + bytesToString(file.dsi().longitudeInterval));
    lines.push_back("numberLatitudeLines: " + bytesToString(file.dsi().numberLatitudeLines));
    lines.push_back("numberLongitudeLines: " + bytesToString(file.dsi().numberLongitudeLines));
    lines.push_back("partialCellIndicator: " + bytesToString(file.dsi().partialCellIndicator));
    lines.push_back("coveragePercent: " + bytesToString(file.dsi().coveragePercent));
    lines.push_back("geoidUndulation: " + bytesToString(file.dsi().geoidUndulation));
    lines.push_back("reserved5: " + bytesToString(file.dsi().reserved5));
    lines.push_back("");

    // Accuracy Description Record
    lines.push_back("----Accuracy Description Record----");
    lines.push_back("recognitionSentinel: " + bytesToString(file.acc().recognitionSentinel));
    lines.push_back("absHorizontalAccuracy: " + bytesToString(file.acc().absHorizontalAccuracy));
    lines.push_back("absVerticalAccuracy: " + bytesToString(file.acc().absVerticalAccuracy));
    lines.push_back("relHorizontalAccuracy: " + bytesToString(file.acc().relHorizontalAccuracy));
    lines.push_back("relVerticalAccuracy: " + bytesToString(file.acc().relVerticalAccuracy));
    lines.push_back("reserved1: " + bytesToString(file.acc().reserved1));
    lines.push_back("reservedDMA: " + bytesToString(file.acc().reservedDMA));
    lines.push_back("reserved2: " + bytesToString(file.acc().reserved2));
    lines.push_back("accuracyOutlineFlag: " + bytesToString(file.acc().accuracyOutlineFlag));
    lines.push_back("accuracySubregions: " + bytesToString(file.acc().accuracySubregions));
    lines.push_back("reservedDMA2: " + bytesToString(file.acc().reservedDMA2));
    lines.push_back("reserved3: " + bytesToString(file.acc().reserved3));
    lines.push_back("");

    return lines;
}

bool loadNewFile = false;
std::vector<std::string> newTextData;
static void dropCallback(GLFWwindow* window, int count, const char** paths)
{
    DtedFile newFile(paths[0]);
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
    DtedFile dtedFile(file);
    std::vector<std::string> textData = getDtedFileDataLines(dtedFile);

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

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
               LPSTR     lpCmdLine,
               int       nShowCmd
)
{
    main();
}