#include "dted/DtedFile.h"
using dted::DtedFile;
#include "icon.h"

#include "dted/dtedParsedStructs.h"

#include "cpputils/windows/dwm.h"
#include "glh/classes/OpenGLApplication.h"

#include <iostream>
#include <vector>

const std::string file = "data/n39_w084_1arc_v3.dt2";

const std::vector<std::string> getDtedFileDataLines(const DtedFile& file) {
    std::vector<std::string> lines;
    //// User Header Label
    //lines.push_back("----User Header Label----");
    //lines.push_back("recognitionSentinel: " + file.uhl().recognitionSentinel);
    //lines.push_back("fixedStandard: " + file.uhl().fixedByStandard);
    //lines.push_back("longitudeOfOrigin: " + std::to_string(file.uhl().longitudeOfOriginTenthsArcSeconds));
    //lines.push_back("latitudeOfOrigin: " + file.uhl().latitudeOfOriginTenthsArcSeconds);
    //lines.push_back("longitudeInterval: " + file.uhl().longitudeIntervalArcSeconds);
    //lines.push_back("latitudeInterval: " + file.uhl().latitudeIntervalArcSeconds);
    //lines.push_back("absoluteVerticalAccuracy: " + file.uhl().absoluteVerticalAccuracy);
    //lines.push_back("securityCode: " + file.uhl().securityCode);
    //lines.push_back("uniqueReference: " + file.uhl().uniqueReference);
    //lines.push_back("numberOfLongitudeLines: " + file.uhl().numberOfLongitudeLines);
    //lines.push_back("numberOfLatitudePoints: " + file.uhl().numberOfLatitudePoints);
    //lines.push_back("multipleAccuracy: " + file.uhl().multipleAccuracy);
    //lines.push_back("reserved: " + file.uhl().reserved);
    //lines.push_back("");

    //// Data Set Identification
    //lines.push_back("----Data Set Identification----");
    //lines.push_back("recognitionSentinel: " + file.dsi().recognitionSentinel);
    //lines.push_back("securityCode: " + file.dsi().securityCode);
    //lines.push_back("securityControlMarkings: " + file.dsi().securityControlMarkings);
    //lines.push_back("securityHandlingDescription: " + file.dsi().securityHandlingDescription);
    //lines.push_back("reserved1: " + file.dsi().reserved1);
    //lines.push_back("dtedLevel: " + file.dsi().dtedLevel);
    //lines.push_back("uniqueReference: " + file.dsi().uniqueReference);
    //lines.push_back("reserved2: " + file.dsi().reserved2);
    //lines.push_back("dataEditionNumber: " + file.dsi().dataEditionNumber);
    //lines.push_back("matchMergeVersion: " + file.dsi().matchMergeVersion);
    //lines.push_back("maintenanceDate: " + file.dsi().maintenanceDate);
    //lines.push_back("matchMergeDate: " + file.dsi().matchMergeDate);
    //lines.push_back("maintenanceDescCode: " + file.dsi().maintenanceDescCode);
    //lines.push_back("producerCode: " + file.dsi().producerCode);
    //lines.push_back("reserved3: " + file.dsi().reserved3);
    //lines.push_back("productSpec: " + file.dsi().productSpec);
    //lines.push_back("productSpecAmend: " + file.dsi().productSpecAmend);
    //lines.push_back("productSpecDate: " + file.dsi().productSpecDate);
    //lines.push_back("verticalDatum: " + file.dsi().verticalDatum);
    //lines.push_back("horizontalDatum: " + file.dsi().horizontalDatum);
    //lines.push_back("digitizingSystem: " + file.dsi().digitizingSystem);
    //lines.push_back("compilationDate: " + file.dsi().compilationDate);
    //lines.push_back("reserved4: " + file.dsi().reserved4);
    //lines.push_back("latOrigin: " + file.dsi().latitudeOfOriginTenthsArcSeconds);
    //lines.push_back("lonOrigin: " + file.dsi().longitudeOfOriginTenthsArcSeconds);
    //lines.push_back("latSWCorner: " + file.dsi().latSWCorner);
    //lines.push_back("lonSWCorner: " + file.dsi().lonSWCorner);
    //lines.push_back("latNWCorner: " + file.dsi().latNWCorner);
    //lines.push_back("lonNWCorner: " + file.dsi().lonNWCorner);
    //lines.push_back("latNECorner: " + file.dsi().latNECorner);
    //lines.push_back("lonNECorner: " + file.dsi().lonNECorner);
    //lines.push_back("latSECorner: " + file.dsi().latSECorner);
    //lines.push_back("lonSECorner: " + file.dsi().lonSECorner);
    //lines.push_back("orientationAngle: " + file.dsi().orientationAngle);
    //lines.push_back("latitudeInterval: " + file.dsi().latitudeIntervalArcSeconds);
    //lines.push_back("longitudeInterval: " + file.dsi().longitudeIntervalArcSeconds);
    //lines.push_back("numberLatitudeLines: " + file.dsi().numberLatitudeLines);
    //lines.push_back("numberLongitudeLines: " + file.dsi().numberLongitudeLines);
    //lines.push_back("partialCellIndicator: " + file.dsi().partialCellIndicator);
    //lines.push_back("coveragePercent: " + file.dsi().coveragePercent);
    //lines.push_back("geoidUndulation: " + file.dsi().geoidUndulation);
    //lines.push_back("reserved5: " + file.dsi().reserved5);
    //lines.push_back("");

    //// Accuracy Description Record
    //lines.push_back("----Accuracy Description Record----");
    //lines.push_back("recognitionSentinel: " + file.acc().recognitionSentinel);
    //lines.push_back("absHorizontalAccuracy: " + file.acc().absHorizontalAccuracy);
    //lines.push_back("absVerticalAccuracy: " + file.acc().absVerticalAccuracy);
    //lines.push_back("relHorizontalAccuracy: " + file.acc().relHorizontalAccuracy);
    //lines.push_back("relVerticalAccuracy: " + file.acc().relVerticalAccuracy);
    //lines.push_back("reserved1: " + file.acc().reserved1);
    //lines.push_back("reservedDMA: " + file.acc().reservedDMA);
    //lines.push_back("reserved2: " + file.acc().reserved2);
    //lines.push_back("accuracyOutlineFlag: " + file.acc().accuracyOutlineFlag);
    //lines.push_back("accuracySubregions: " + file.acc().accuracySubregions);
    //lines.push_back("reservedDMA2: " + file.acc().reservedDMA2);
    //lines.push_back("reserved3: " + file.acc().reserved3);
    //lines.push_back("");

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
    DtedFile dtedFile(file);
    dtedFile.loadFile(true);
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