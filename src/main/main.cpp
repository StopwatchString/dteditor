#include "dted/DtedFile.h"
using dted::DtedFile;

#include "dted/dtedFileDefinitions.h"
#include "dted/dtedParsedStructs.h"

using namespace dted;

#include "cpputils/Timer.h"
#include "cpputils/windows/simpleDirectIo.h"

#include "tracy/Tracy.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <conio.h>

const std::filesystem::path file1 = "..\\data\\n39_w084_1arc_v3.dt2";
const std::filesystem::path file2 = "..\\data\\n39_w085_1arc_v3.dt2";
const std::filesystem::path file3 = "C:\\data\\n39_w084_1arc_v3.dt2";
static constexpr size_t ALIGNMENT = 4096;
static constexpr char FILL = 0;

//-------------------------------------------------------------------------------
// writeDataAligned()
//     Writes a buffer of data to a file, padding it to the given alignment size
//-------------------------------------------------------------------------------
template<size_t alignment = ALIGNMENT, char fill = FILL>
size_t writeDataAligned(std::ofstream& file, const char* buf, const size_t bufSize)
{
    // Read-made vector 
    static constexpr std::array<char, alignment> fillData{ fill };

    if (bufSize > 0 && buf != nullptr) {
        file.write(buf, bufSize);
    }
    size_t padding = (alignment - (bufSize % alignment)) % alignment;
    file.write(fillData.data(), padding);

    return (bufSize / alignment) + 1;
}

//-------------------------------------------------------------------------------
// generateAlignedDtedFile()
//     Takes a dted file and outputs it such that its header and data columns exist
//     on disk-aligned boundaries. This inflates the size of the file but better
//     accomodates batching direct I/O on the files.
//-------------------------------------------------------------------------------
bool generateAlignedDtedFile(const std::filesystem::path& file)
{
    ZoneScoped;

    cpputils::win::AlignedBuffer fileBuffer;

    {
        ZoneScopedN("Read File");
        fileBuffer = cpputils::win::directFileToBuffer(file);
        if (fileBuffer.buf == nullptr) return false;
    }

    {
        ZoneScopedN("Write File");

        std::filesystem::path newFile = file.string() + ".aligned";
        std::ofstream outputFile(newFile, std::ios::binary);
        if (!outputFile.is_open()) {
            std::cerr << "ERROR Cannot open file " << newFile << std::endl;
            return false;
        }

        uint32_t headerSize = USER_HEADER_LABEL_BLOB_SIZE + DATA_SET_IDENTIFICATION_BLOB_SIZE + ACCURACY_DESCRIPTION_RECORD_BLOB_SIZE;
        size_t sectors = writeDataAligned(outputFile, (const char*)fileBuffer.buf, headerSize);

        UserHeaderLabelBlob uhlBlob = *reinterpret_cast<UserHeaderLabelBlob*>((char*)fileBuffer.buf + USER_HEADER_LABEL_BLOB_OFFSET);
        UserHeaderLabel uhl(uhlBlob);

        uint32_t recordsOffset = headerSize;
        uint32_t columnHeaderSize = COLUMN_HEADER_BLOB_SIZE;
        uint32_t columnDataSize = sizeof(int16_t) * uhl.numberOfLatitudePoints;
        uint32_t columnFooterSize = COLUMN_FOOTER_BLOB_SIZE;
        uint32_t recordSize = columnHeaderSize + columnDataSize + columnFooterSize;

        for (size_t lon = 0; lon < uhl.numberOfLongitudeLines; lon++) {
            uint32_t baseOffset = recordsOffset + lon * recordSize;
            writeDataAligned(outputFile, (const char*)fileBuffer.buf + baseOffset, recordSize);
        }

        outputFile.close();
    }
    return false;
}

int main()
{
    bool running = true;
    while (running) {
        generateAlignedDtedFile(file3);

        if (_kbhit()) {
            int ch = std::tolower(_getch());
            if (ch == 'q') {
                running = false;
            }
        }

        FrameMark;
    }

    return EXIT_SUCCESS;
}
