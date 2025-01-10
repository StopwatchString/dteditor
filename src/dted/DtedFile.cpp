#include "dted/DtedFile.h"

#include "dted/dtedFileDefinitions.h"

#include "vectorclass.h"

#include <iostream>
#include <fstream>
#include <thread>

namespace dted {

//-----------------------------------------------
// Constructor
//-----------------------------------------------
DtedFile::DtedFile(const std::string& filename)
    : _filename(filename),
      _valid(false),
      _elevations(nullptr),
      _rowCount(0),
      _columnCount(0)
{
}

//-----------------------------------------------
// Destructor
//-----------------------------------------------
DtedFile::~DtedFile()
{
    delete _elevations;
}

//-----------------------------------------------
// Move Constructor
//-----------------------------------------------
DtedFile::DtedFile(DtedFile&& other) noexcept 
    : _filename(std::move(other._filename)),
      _valid(other._valid),
      _elevations(other._elevations),
      _rowCount(other._rowCount),
      _columnCount(other._columnCount)
{
    other._valid = false;
    other._elevations = nullptr;
    other._rowCount = 0;
    other._columnCount = 0;
}

//-----------------------------------------------
// Move Assignment
//-----------------------------------------------
DtedFile& DtedFile::operator=(DtedFile&& other) noexcept
{
    if (this != &other) {
        _filename = std::move(other._filename);
        _valid = other._valid;

        other._valid = false;
    }
    return *this;
}

//-----------------------------------------------
// loadFile()
//-----------------------------------------------
void DtedFile::loadFile(bool printLoadStats)
{
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    // Get file size
    std::ifstream file(_filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "ERROR Cannot open file " << _filename << std::endl;
        return;
    }
    std::streampos fileSize = file.tellg();

    // Create heap block sized to the file
    std::unique_ptr<std::byte[]> data = std::make_unique<std::byte[]>(fileSize);

    // Go to beginning of file, then read into heap block
    file.seekg(0, std::ios::beg);
    if (!file.good()) {
        std::cerr << "ERROR Cannot seek to header!" << std::endl;
        return;
    }
    if (!file.read(reinterpret_cast<char*>(data.get()), fileSize)) {
        std::cerr << "ERROR Failed to pull entire file into heap block." << std::endl;
    }

    file.close();

    UserHeaderLabelBlob* uhlBlob = reinterpret_cast<UserHeaderLabelBlob*>(data.get() + USER_HEADER_LABEL_BLOB_OFFSET);
    if (!uhlBlob->valid()) {
        std::cerr << "WARNING DtedFile::loadFile() Parsed UserHeaderLabel is NOT valid. Failed to load file: " << _filename << std::endl;
        return;
    }
    _uhl = UserHeaderLabel(*uhlBlob);

    DataSetIdentificationBlob* dsiBlob = reinterpret_cast<DataSetIdentificationBlob*>(data.get() + DATA_SET_IDENTIFICATION_BLOB_OFFSET);
    if (!uhlBlob->valid()) {
        std::cerr << "WARNING DtedFile::loadFile() Parsed DataSetIdentification is NOT valid. Failed to load file: " << _filename << std::endl;
        return;
    }
    _dsi = DataSetIdentification(*dsiBlob);

    AccuracyDescriptionRecordBlob* accBlob = reinterpret_cast<AccuracyDescriptionRecordBlob*>(data.get() + ACCURACY_DESCRIPTION_RECORD_BLOB_OFFSET);
    if (!uhlBlob->valid()) {
        std::cerr << "WARNING DtedFile::loadFile() Parsed AccuracyDescriptionRecord is NOT valid. Failed to load file: " << _filename << std::endl;
        return;
    }
    _acc = AccuracyDescriptionRecord(*accBlob);

    _columnCount = _dsi.numberLatitudeLines;
    _rowCount = _dsi.numberLongitudeLines;

    _valid = loadElevations(data);

    if (printLoadStats) {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration elapsedTime = end - start;
        std::chrono::milliseconds elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);
        std::cout << "Time to load " << _filename << ": " << elapsedMs.count() << "ms" << std::endl;
    }
}

//-----------------------------------------------
// loadElevations()
//-----------------------------------------------
bool DtedFile::loadElevations(const std::unique_ptr<std::byte[]>& data)
{
    bool retVal = false;

    _elevations = new int16_t[_columnCount * _rowCount];

    uint32_t baseOffset = DATA_RECORDS_FILE_OFFSET;
    uint32_t headerSize = COLUMN_HEADER_BLOB_SIZE;
    uint32_t dataSize = sizeof(int16_t) * _rowCount;
    uint32_t footerSize = COLUMN_FOOTER_BLOB_SIZE;
    uint32_t recordSize = headerSize + dataSize + footerSize;

    for (uint32_t lon = 0; lon < _columnCount; lon++) {
        uint64_t checksum = 0;

        uint32_t columnOffset = baseOffset + lon * recordSize;
        ColumnHeaderBlob* header = reinterpret_cast<ColumnHeaderBlob*>(data.get() + columnOffset);
        for (uint32_t i = 0; i < COLUMN_HEADER_BLOB_SIZE; i++) {
            checksum += *(uint8_t*)(data.get() + columnOffset + i);
        }

        std::byte* recordData = data.get() + columnOffset + COLUMN_HEADER_BLOB_SIZE;

        //Vec16us signMask(0b0111'1111'1111'1111); // Mask to keep lower 15 bits
        //Vec16us negativeMask(0b1000'0000'0000'0000); // Mask to detect sign bit
        //uint32_t evenRowUpperLimit = (_rowCount / 16) * 16; // Process rows in blocks of 16
        //for (uint32_t lat = 0; lat < evenRowUpperLimit; lat += 16) {
        //    // Load 16 values (32 bytes) into a SIMD register
        //    Vec16us rawVals(
        //        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(recordData + lat * 2))
        //    );

        //    // Calculate checksum (sum of high and low bytes)
        //    for (int i = 0; i < 16; i++) {
        //        uint16_t rawVal = rawVals[i];
        //        uint8_t high = (rawVal >> 8) & 0xFF;
        //        uint8_t low = rawVal & 0xFF;
        //        checksum += high + low;
        //    }

        //    // Apply sign conversion
        //    Vec16us signBits = rawVals & negativeMask;
        //    Vec16us magnitudes = rawVals & signMask;
        //    Vec16us convertedVals = magnitudes - (signBits >> 15) * 2 * magnitudes;

        //    // Store processed values in _elevations
        //    _mm256_storeu_si256(
        //        reinterpret_cast<__m256i*>(&_elevations[lon * _rowCount + lat]),
        //        convertedVals
        //    );
        //}

        Vec8us signMask(0b0111'1111'1111'1111); // Mask to keep lower 15 bits
        Vec8us negativeMask(0b1000'0000'0000'0000); // Mask to detect sign bit
        uint32_t evenRowUpperLimit = (_rowCount / 8) * 8;
        for (uint32_t lat = 0; lat < evenRowUpperLimit; lat += 8) {
            // Load 8 values (16 bytes) into a SIMD register
            Vec8us rawVals(_mm_loadu_si128(reinterpret_cast<const __m128i*>(recordData + lat * 2)));

            // Calculate checksum (sum of high and low bytes)
            for (int i = 0; i < 8; i++) {
                uint16_t rawVal = rawVals[i];
                uint8_t high = (rawVal >> 8) & 0xFF;
                uint8_t low = rawVal & 0xFF;
                checksum += high + low;
            }

            // Apply sign conversion
            Vec8us signBits = rawVals & negativeMask;
            Vec8us magnitudes = rawVals & signMask;
            Vec8us convertedVals = magnitudes - (signBits << 1);

            // Store processed values in _elevations
            _mm_storeu_si128(
                reinterpret_cast<__m128i*>(&_elevations[lon * _rowCount + lat]),
                convertedVals
            );
        }

        //for (uint32_t lat = 0; lat < _rowCount; lat++) {
        //    uint8_t high = (uint8_t)(recordData + lat * 2)[0];
        //    uint8_t low =  (uint8_t)(recordData + lat * 2)[1];

        //    checksum += (uint64_t)high + (uint64_t)low;

        //    // DTED does not save elevation data as 2s compliment, so we need to convert.
        //    // Highest bit is used to set sign, rest is interpreted as if unsigned.
        //    uint8_t highestBit = high & 0b1000'0000;
        //    high = high & 0b0111'1111;
        //    int8_t negative = -1 * (highestBit >> 7) + 1 * (0b1000'0000 - highestBit);

        //    _elevations[lon * _rowCount + lat] = ((static_cast<int16_t>(high << 8)) + static_cast<int16_t>(low)) * negative;
        //}
    }

    return retVal;
}

} // End dted namespace