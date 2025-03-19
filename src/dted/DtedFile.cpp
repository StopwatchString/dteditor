#include "dted/DtedFile.h"

#include "dted/dtedFileDefinitions.h"

#include "vectorclass.h"

#define _WIN32_WINNT 0x0602 // Windows 8+
#include <windows.h>
#include <memoryapi.h>

#include <iostream>
#include <fstream>
#include <thread>

namespace dted {

//-----------------------------------------------
// Constructor
//-----------------------------------------------
DtedFile::DtedFile(const std::string& filename) :
    _filename(filename), _valid(false), _elevations(nullptr), _rowCount(0), _columnCount(0)
{}

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
DtedFile::DtedFile(DtedFile&& other) noexcept :
    _filename(std::move(other._filename)),
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
void DtedFile::loadFile(const LoadStrategy strategy)
{
    switch (strategy) {
        //////////////////////////////////////////////////
        // STL_IFSTREAM
        //////////////////////////////////////////////////
        case LoadStrategy::STL_IFSTREAM: {
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

            _valid = parseFile(data.get());
            break;
        }
        //////////////////////////////////////////////////
        // WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN
        //////////////////////////////////////////////////
        case LoadStrategy::WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN: {
            HANDLE hFile = CreateFileA(
                _filename.c_str(),         // lpFileName,
                GENERIC_READ,              // dwDesiredAccess,
                FILE_SHARE_READ,           // dwShareMode,
                NULL,                      // lpSecurityAttributes,
                OPEN_EXISTING,             // dwCreationDisposition,
                FILE_FLAG_SEQUENTIAL_SCAN, // dwFlagsAndAttribute,
                NULL                       // hTemplateFile
            );

            if (hFile == NULL) {
                std::cout << "ERROR: Could not open file " << _filename << std::endl;
                return;
            }

            HANDLE hFileMapping = CreateFileMappingA(
                hFile,         // hFile,
                NULL,          // lpFileMappingAttributes,
                PAGE_READONLY, // flProtect,
                0,             // dwMaximumSizeHigh,
                0,             // dwMaximumSizeLow,
                NULL           // lpName
            );

            if (hFileMapping == NULL) {
                std::cout << "ERROR: Could not map view of file " << _filename << std::endl;
                CloseHandle(hFile);
                return;
            }

            LPVOID lpData = MapViewOfFile(
                hFileMapping,  // hFileMappingObject,
                FILE_MAP_READ, // dwDesiredAccess,
                0,             // dwFileOffsetHigh,
                0,             // dwFileOffsetLow,
                0              // dwNumberOfBytesToMap
            );

            if (lpData != nullptr) {
                _valid = parseFile((std::byte*)lpData);
            }

            UnmapViewOfFile(lpData);
            CloseHandle(hFileMapping);
            CloseHandle(hFile);
            break;
        }
        //////////////////////////////////////////////////
        // WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN_PREFETCH
        //////////////////////////////////////////////////
        case LoadStrategy::WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN_PREFETCH: {
            HANDLE hFile = CreateFileA(
                _filename.c_str(),         // lpFileName,
                GENERIC_READ,              // dwDesiredAccess,
                FILE_SHARE_READ,           // dwShareMode,
                NULL,                      // lpSecurityAttributes,
                OPEN_EXISTING,             // dwCreationDisposition,
                FILE_FLAG_SEQUENTIAL_SCAN, // dwFlagsAndAttribute,
                NULL                       // hTemplateFile
            );

            if (hFile == NULL) {
                std::cout << "ERROR: Could not open file " << _filename << std::endl;
                return;
            }

            HANDLE hFileMapping = CreateFileMappingA(
                hFile,         // hFile,
                NULL,          // lpFileMappingAttributes,
                PAGE_READONLY, // flProtect,
                0,             // dwMaximumSizeHigh,
                0,             // dwMaximumSizeLow,
                NULL           // lpName
            );

            if (hFileMapping == NULL) {
                std::cout << "ERROR: Could not map view of file " << _filename << std::endl;
                CloseHandle(hFile);
                return;
            }

            LPVOID lpData = MapViewOfFile(
                hFileMapping,  // hFileMappingObject,
                FILE_MAP_READ, // dwDesiredAccess,
                0,             // dwFileOffsetHigh,
                0,             // dwFileOffsetLow,
                0              // dwNumberOfBytesToMap
            );

            // Get file size
            LARGE_INTEGER fileSize;
            if (!GetFileSizeEx(hFile, &fileSize)) {
                std::cerr << "ERROR: Could not get file size" << std::endl;
                UnmapViewOfFile(lpData);
                CloseHandle(hFileMapping);
                CloseHandle(hFile);
                return;
            }

            WIN32_MEMORY_RANGE_ENTRY range;
            range.VirtualAddress = lpData;
            range.NumberOfBytes = fileSize.QuadPart;
            PrefetchVirtualMemory(GetCurrentProcess(), 1, &range, 0);

            if (lpData != nullptr) {
                _valid = parseFile((std::byte*)lpData);
            }

            UnmapViewOfFile(lpData);
            CloseHandle(hFileMapping);
            CloseHandle(hFile);
            break;
        }
        //////////////////////////////////////////////////
        // WINDOWS_MEMORY_MAP_NON_BUFFERED
        //////////////////////////////////////////////////
        case LoadStrategy::WINDOWS_MEMORY_MAP_NON_BUFFERED: {
            HANDLE hFile = CreateFileA(
                _filename.c_str(),      // lpFileName,
                GENERIC_READ,           // dwDesiredAccess,
                FILE_SHARE_READ,        // dwShareMode,
                NULL,                   // lpSecurityAttributes,
                OPEN_EXISTING,          // dwCreationDisposition,
                FILE_FLAG_NO_BUFFERING, // dwFlagsAndAttribute,
                NULL                    // hTemplateFile
            );

            if (hFile == NULL) {
                std::cout << "ERROR: Could not open file " << _filename << std::endl;
                return;
            }

            HANDLE hFileMapping = CreateFileMappingA(
                hFile,         // hFile,
                NULL,          // lpFileMappingAttributes,
                PAGE_READONLY, // flProtect,
                0,             // dwMaximumSizeHigh,
                0,             // dwMaximumSizeLow,
                NULL           // lpName
            );

            if (hFileMapping == NULL) {
                std::cout << "ERROR: Could not map view of file " << _filename << std::endl;
                CloseHandle(hFile);
                return;
            }

            LPVOID lpData = MapViewOfFile(
                hFileMapping,  // hFileMappingObject,
                FILE_MAP_READ, // dwDesiredAccess,
                0,             // dwFileOffsetHigh,
                0,             // dwFileOffsetLow,
                0              // dwNumberOfBytesToMap
            );

            if (lpData != nullptr) {
                _valid = parseFile((std::byte*)lpData);
            }

            UnmapViewOfFile(lpData);
            CloseHandle(hFileMapping);
            CloseHandle(hFile);
            break;
        }
        //////////////////////////////////////////////////
        // WINDOWS_MEMORY_MAP_NON_BUFFERED_PREFETCH
        //////////////////////////////////////////////////
        case LoadStrategy::WINDOWS_MEMORY_MAP_NON_BUFFERED_PREFETCH: {
            HANDLE hFile = CreateFileA(
                _filename.c_str(),      // lpFileName,
                GENERIC_READ,           // dwDesiredAccess,
                FILE_SHARE_READ,        // dwShareMode,
                NULL,                   // lpSecurityAttributes,
                OPEN_EXISTING,          // dwCreationDisposition,
                FILE_FLAG_NO_BUFFERING, // dwFlagsAndAttribute,
                NULL                    // hTemplateFile
            );

            if (hFile == NULL) {
                std::cout << "ERROR: Could not open file " << _filename << std::endl;
                return;
            }

            HANDLE hFileMapping = CreateFileMappingA(
                hFile,         // hFile,
                NULL,          // lpFileMappingAttributes,
                PAGE_READONLY, // flProtect,
                0,             // dwMaximumSizeHigh,
                0,             // dwMaximumSizeLow,
                NULL           // lpName
            );

            if (hFileMapping == NULL) {
                std::cout << "ERROR: Could not map view of file " << _filename << std::endl;
                CloseHandle(hFile);
                return;
            }

            LPVOID lpData = MapViewOfFile(
                hFileMapping,  // hFileMappingObject,
                FILE_MAP_READ, // dwDesiredAccess,
                0,             // dwFileOffsetHigh,
                0,             // dwFileOffsetLow,
                0              // dwNumberOfBytesToMap
            );

            // Get file size
            LARGE_INTEGER fileSize;
            if (!GetFileSizeEx(hFile, &fileSize)) {
                std::cerr << "ERROR: Could not get file size" << std::endl;
                UnmapViewOfFile(lpData);
                CloseHandle(hFileMapping);
                CloseHandle(hFile);
                return;
            }

            WIN32_MEMORY_RANGE_ENTRY range;
            range.VirtualAddress = lpData;
            range.NumberOfBytes = fileSize.QuadPart;
            PrefetchVirtualMemory(GetCurrentProcess(), 1, &range, 0);

            if (lpData != nullptr) {
                _valid = parseFile((std::byte*)lpData);
            }

            UnmapViewOfFile(lpData);
            CloseHandle(hFileMapping);
            CloseHandle(hFile);
            break;
        }
        //////////////////////////////////////////////////
        // WINDOWS_DIRECT_READ_NON_BUFFERED
        //////////////////////////////////////////////////
        case LoadStrategy::WINDOWS_DIRECT_READ_NON_BUFFERED: {
            HANDLE hFile = CreateFileA(
                _filename.c_str(),      // lpFileName,
                GENERIC_READ,           // dwDesiredAccess,
                FILE_SHARE_READ,        // dwShareMode,
                NULL,                   // lpSecurityAttributes,
                OPEN_EXISTING,          // dwCreationDisposition,
                FILE_FLAG_NO_BUFFERING, // dwFlagsAndAttribute,
                NULL                    // hTemplateFile
            );

            if (hFile == NULL) {
                std::cout << "ERROR: Could not open file " << _filename << std::endl;
                return;
            }

            // Get file size
            LARGE_INTEGER fileSize;
            if (!GetFileSizeEx(hFile, &fileSize)) {
                std::cerr << "ERROR: Could not get file size" << std::endl;
                CloseHandle(hFile);
                return;
            }

            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            size_t alignment = sysInfo.dwPageSize; // Typically 4096

            // Adjust to be a multiple of the page size
            size_t adjustedSize = fileSize.QuadPart + (sysInfo.dwPageSize - (fileSize.QuadPart % sysInfo.dwPageSize));

            void* buffer = _aligned_malloc(adjustedSize, alignment);
            if (!buffer) {
                std::cerr << "ERROR: Could not allocate buffer" << std::endl;
                CloseHandle(hFile);
                return;
            }

            // Read file data into buffer
            DWORD bytesRead = 0;
            BOOL result = ReadFile(hFile, buffer, adjustedSize, &bytesRead, NULL);

            if (!result) {
                DWORD err = GetLastError();
                if (err == ERROR_IO_PENDING) {
                    // Wait for asynchronous read to complete
                    // GetOverlappedResult(hFile, &overlapped, &bytesRead, TRUE);
                }
                else {
                    std::cerr << "ERROR: ReadFile failed with code " << err << std::endl;
                    _aligned_free(buffer);
                    CloseHandle(hFile);
                    return;
                }
            }

            if (buffer != nullptr) {
                _valid = parseFile((std::byte*)buffer);
            }

            CloseHandle(hFile);
            break;
        }
        //////////////////////////////////////////////////
        // WINDOWS_DIRECT_READ_NON_BUFFERED_OVERLAPPED
        //////////////////////////////////////////////////
        case LoadStrategy::WINDOWS_DIRECT_READ_NON_BUFFERED_OVERLAPPED: {
            HANDLE hFile = CreateFileA(
                _filename.c_str(),                             // lpFileName,
                GENERIC_READ,                                  // dwDesiredAccess,
                FILE_SHARE_READ,                               // dwShareMode,
                NULL,                                          // lpSecurityAttributes,
                OPEN_EXISTING,                                 // dwCreationDisposition,
                FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, // dwFlagsAndAttribute,
                NULL                                           // hTemplateFile
            );

            if (hFile == NULL) {
                std::cout << "ERROR: Could not open file " << _filename << std::endl;
                return;
            }

            // Get file size
            LARGE_INTEGER fileSize;
            if (!GetFileSizeEx(hFile, &fileSize)) {
                std::cerr << "ERROR: Could not get file size" << std::endl;
                CloseHandle(hFile);
                return;
            }

            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            size_t alignment = sysInfo.dwPageSize; // Typically 4096

            // Adjust to be a multiple of the page size
            size_t adjustedSize = fileSize.QuadPart + (sysInfo.dwPageSize - (fileSize.QuadPart % sysInfo.dwPageSize));

            void* buffer = _aligned_malloc(adjustedSize, alignment);
            if (!buffer) {
                std::cerr << "ERROR: Could not allocate buffer" << std::endl;
                CloseHandle(hFile);
                return;
            }

            struct readJob
            {
                OVERLAPPED overlapped;
                DWORD bytesRead;
            };

            const static size_t SECTORS = 8;
            const size_t sectorSize = adjustedSize / SECTORS;
            std::array<readJob, SECTORS> events = {};
            HANDLE waitEvents[SECTORS] = {};
            for (int i = 0; i < SECTORS; i++) {
                events[i].overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                waitEvents[i] = events[i].overlapped.hEvent;
                events[i].overlapped.Offset = i * sectorSize;

                ReadFile(hFile, (std::byte*)buffer + events[i].overlapped.Offset, sectorSize, &events[i].bytesRead, &events[i].overlapped);
            }

            WaitForMultipleObjects(SECTORS, waitEvents, TRUE, INFINITE);

            if (buffer != nullptr) {
                _valid = parseFile((std::byte*)buffer);
            }

            CloseHandle(hFile);
            break;
        }
    }
}

//-----------------------------------------------
// parseFile()
//-----------------------------------------------
bool DtedFile::parseFile(const std::byte* data)
{
    const UserHeaderLabelBlob* uhlBlob
        = reinterpret_cast<const UserHeaderLabelBlob*>(data + USER_HEADER_LABEL_BLOB_OFFSET);
    if (!uhlBlob->valid()) {
        std::cerr << "WARNING DtedFile::loadFile() Parsed UserHeaderLabel is NOT valid. Failed to load file: "
                  << _filename << std::endl;
        return false;
    }
    _uhl = UserHeaderLabel(*uhlBlob);

    const DataSetIdentificationBlob* dsiBlob
        = reinterpret_cast<const DataSetIdentificationBlob*>(data + DATA_SET_IDENTIFICATION_BLOB_OFFSET);
    if (!uhlBlob->valid()) {
        std::cerr << "WARNING DtedFile::loadFile() Parsed DataSetIdentification is NOT valid. Failed to load file: "
                  << _filename << std::endl;
        return false;
    }
    _dsi = DataSetIdentification(*dsiBlob);

    const AccuracyDescriptionRecordBlob* accBlob
        = reinterpret_cast<const AccuracyDescriptionRecordBlob*>(data + ACCURACY_DESCRIPTION_RECORD_BLOB_OFFSET);
    if (!uhlBlob->valid()) {
        std::cerr << "WARNING DtedFile::loadFile() Parsed AccuracyDescriptionRecord is NOT valid. Failed to load file: "
                  << _filename << std::endl;
        return false;
    }
    _acc = AccuracyDescriptionRecord(*accBlob);

    _columnCount = _dsi.numberLatitudeLines;
    _rowCount = _dsi.numberLongitudeLines;

    bool loadedElevations = loadElevations(data);

    return true;
}

//-----------------------------------------------
// loadElevations()
//-----------------------------------------------
bool DtedFile::loadElevations(const std::byte* data)
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
        const ColumnHeaderBlob* header = reinterpret_cast<const ColumnHeaderBlob*>(data + columnOffset);
        for (uint32_t i = 0; i < COLUMN_HEADER_BLOB_SIZE; i++) {
            checksum += *(uint8_t*)(data + columnOffset + i);
        }

        const std::byte* recordData = data + columnOffset + COLUMN_HEADER_BLOB_SIZE;

        Vec8us signMask(0b0111'1111'1111'1111);     // Mask to keep lower 15 bits
        Vec8us negativeMask(0b1000'0000'0000'0000); // Mask to detect sign bit
        uint32_t evenRowUpperLimit = (_rowCount / 8) * 8;
        for (uint32_t lat = 0; lat < evenRowUpperLimit; lat += 8) {
            // Load into SIMD register
            Vec8us rawVals(_mm_loadu_si128(reinterpret_cast<const __m128i*>(recordData + lat * 2)));

            // Checksum
            for (int i = 0; i < 8; i++) {
                uint16_t rawVal = rawVals[i];
                uint8_t high = (rawVal >> 8) & 0xFF;
                uint8_t low = rawVal & 0xFF;
                checksum += static_cast<uint64_t>(high) + static_cast<uint64_t>(low);
            }

            // Vectorized handling of high bit sign check
            Vec8us signBits = rawVals & negativeMask;
            Vec8us magnitudes = rawVals & signMask;
            Vec8us convertedVals = magnitudes - (signBits << 1);

            // Save out of SIMD register into _elevations
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&_elevations[lon * _rowCount + lat]), convertedVals);
        }

        // uint32_t leftoverCount = _rowCount % 8;
        // for (uint32_t lat = _rowCount - leftoverCount; lat < _rowCount; lat++) {}

        // for (uint32_t lat = 0; lat < _rowCount; lat++) {
        //     uint8_t high = (uint8_t)(recordData + lat * 2)[0];
        //     uint8_t low =  (uint8_t)(recordData + lat * 2)[1];

        //    checksum += (uint64_t)high + (uint64_t)low;

        //    // DTED does not save elevation data as 2s compliment, so we need to convert.
        //    // Highest bit is used to set sign, rest is interpreted as if unsigned.
        //    uint8_t highestBit = high & 0b1000'0000;
        //    high = high & 0b0111'1111;
        //    int8_t negative = -1 * (highestBit >> 7) + 1 * (0b1000'0000 - highestBit);

        //    _elevations[lon * _rowCount + lat] = ((static_cast<int16_t>(high << 8)) + static_cast<int16_t>(low)) *
        //    negative;
        //}
    }

    delete[] _elevations;
    _elevations = nullptr;

    return retVal;
}

} // namespace dted
