#include "dted/DtedFile.h"

#include "dted/dtedFileDefinitions.h"

#include <iostream>
#include <fstream>

namespace dted {

//-----------------------------------------------
// Constructor
//-----------------------------------------------
DtedFile::DtedFile(const std::string& filename)
    : _filename(filename),
      _valid(false),
      _data(nullptr),
      _rowCount(0),
      _columnCount(0)
{
}

//-----------------------------------------------
// Destructor
//-----------------------------------------------
DtedFile::~DtedFile()
{
    delete _data;
}

//-----------------------------------------------
// Move Constructor
//-----------------------------------------------
DtedFile::DtedFile(DtedFile&& other) noexcept 
    : _filename(std::move(other._filename)),
      _valid(other._valid),
      _data(other._data),
      _rowCount(other._rowCount),
      _columnCount(other._columnCount)
{
    other._valid = false;
    other._data = nullptr;
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

    _data = new int16_t[_columnCount * _rowCount];

    uint32_t baseOffset = DATA_RECORDS_FILE_OFFSET;
    uint32_t headerSize = COLUMN_HEADER_BLOB_SIZE;
    uint32_t dataSize = sizeof(int16_t) * _rowCount;
    uint32_t footerSize = COLUMN_FOOTER_BLOB_SIZE;
    uint32_t recordSize = headerSize + dataSize + footerSize;

    for (uint32_t lon = 0; lon < _columnCount; lon++) {
        uint32_t columnOffset = baseOffset + lon * recordSize;
        ColumnHeaderBlob* header = reinterpret_cast<ColumnHeaderBlob*>(data.get() + columnOffset);
        std::byte* recordData = data.get() + columnOffset + COLUMN_HEADER_BLOB_SIZE;
        for (uint32_t lat = 0; lat < _rowCount; lat++) {
            uint8_t high = (uint8_t)(recordData + lat * 2)[0];
            uint8_t low = (uint8_t)(recordData + lat * 2)[1];
            int16_t negative = 1;

            // DTED does not save elevation data as 2's compliment, so we need to convert.
            // Highest bit is used to set sign, rest is interpreted as if unsigned.
            if ((high & 0b1000'0000) > 0b0000'0000) {
                negative = -1;
                high = high & 0b0111'1111;
            }

            _data[lon * _rowCount + lat] = ((static_cast<int16_t>(high << 8)) + static_cast<int16_t>(low)) * negative;
        }
    }

    return retVal;
}

} // End dted namespace