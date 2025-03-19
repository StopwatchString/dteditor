#include "dted/DtedFile.h"
using dted::DtedFile;

#include "dted/dtedFileDefinitions.h"
#include "dted/dtedParsedStructs.h"

using namespace dted;

#include "cpputils/Timer.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

const std::filesystem::path file1 = "..\\data\\n39_w084_1arc_v3.dt2";
const std::filesystem::path file2 = "..\\data\\n39_w085_1arc_v3.dt2";
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

    file.write(buf, bufSize);
    size_t padding = alignment - (bufSize % alignment);
    file.write(fillData.data(), padding);

    return (bufSize / alignment) + 1;
}

template<std::integral T, size_t S>
static std::from_chars_result
integralFromChars(T& target, const std::array<std::byte, S>& bytes, size_t offset, size_t size, int base = 10)
{
    return std::from_chars(
        reinterpret_cast<const char*>(bytes.data() + offset),
        reinterpret_cast<const char*>(bytes.data() + offset + size),
        target,
        base);
}

//-------------------------------------------------------------------------------
// generateAlignedDtedFile()
//     Takes a dted file and outputs it such that its header and data columns exist
//     on disk-aligned boundaries. This inflates the size of the file but better
//     accomodates batching direct I/O on the files.
//-------------------------------------------------------------------------------
void generateAlignedDtedFile(const std::filesystem::path& file)
{
    std::ifstream inputFile(file, std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        std::cerr << "ERROR Cannot open file " << file << std::endl;
        return;
    }
    std::streampos fileSize = inputFile.tellg();

    // Create heap block sized to the file
    std::unique_ptr<std::byte[]> data = std::make_unique<std::byte[]>(fileSize);

    // Go to beginning of file, then read into heap block
    inputFile.seekg(0, std::ios::beg);
    if (!inputFile.good()) {
        std::cerr << "ERROR Cannot seek to header!" << std::endl;
        return;
    }
    if (!inputFile.read(reinterpret_cast<char*>(data.get()), fileSize)) {
        std::cerr << "ERROR Failed to pull entire file into heap block." << std::endl;
    }

    inputFile.close();

    std::filesystem::path newFile = file.string() + ".aligned";
    std::ofstream outputFile(newFile);
    if (!outputFile.is_open()) {
        std::cerr << "ERROR Cannot open file " << newFile << std::endl;
        return;
    }

    uint32_t headerSize = USER_HEADER_LABEL_BLOB_SIZE + DATA_SET_IDENTIFICATION_BLOB_SIZE + ACCURACY_DESCRIPTION_RECORD_BLOB_SIZE;
    size_t sectors = writeDataAligned(outputFile, (const char*)data.get(), headerSize);

    UserHeaderLabelBlob uhlBlob = *reinterpret_cast<UserHeaderLabelBlob*>(data.get() + USER_HEADER_LABEL_BLOB_OFFSET);
    // numberOfLongitudeLines
    uint16_t lonLines;
    integralFromChars(lonLines, uhlBlob.numberOfLongitudeLines, 0, 4);
    uint16_t latLines;
    integralFromChars(latLines, uhlBlob.numberOfLongitudeLines, 0, 4);

    uint32_t recordsOffset = headerSize;
    uint32_t columnHeaderSize = COLUMN_HEADER_BLOB_SIZE;
    uint32_t columnDataSize = sizeof(int16_t) * latLines;
    uint32_t columnFooterSize = COLUMN_FOOTER_BLOB_SIZE;
    uint32_t recordSize = columnHeaderSize + columnDataSize + columnFooterSize;

    for (size_t lon = 0; lon < lonLines; lon++) {
        uint32_t baseOffset = recordsOffset + lon * recordSize;
        writeDataAligned(outputFile, (const char*)data.get(), recordSize);
    }

    std::cout << sectors << std::endl;

    outputFile.close();

}

int main()
{
    generateAlignedDtedFile(file1);

    //DtedFile file(file1.string());
    //file.loadFile(DtedFile::LoadStrategy::STL_IFSTREAM);
    //std::cout << "Lat: " << file.dsi().numberLatitudeLines << " Lon: " << file.dsi().numberLongitudeLines << std::endl;

    return EXIT_SUCCESS;
}
