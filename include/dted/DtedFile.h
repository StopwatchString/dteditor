#ifndef DTED_FILE_H
#define DTED_FILE_H

#include "dted/dtedParsedStructs.h"

#include <string>
#include <memory>
#include <optional>

namespace dted {

class DtedFile
{
public:
    enum class LoadStrategy
    {
        STL_IFSTREAM,
        WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN,
        WINDOWS_MEMORY_MAP_BUFFERED_SEQUENTIAL_SCAN_PREFETCH,
        WINDOWS_MEMORY_MAP_NON_BUFFERED,
        WINDOWS_MEMORY_MAP_NON_BUFFERED_PREFETCH,
        WINDOWS_DIRECT_READ_NON_BUFFERED,
        WINDOWS_DIRECT_READ_NON_BUFFERED_OVERLAPPED,
    };

    DtedFile(const std::string& file);
    ~DtedFile();

    // Disable copy since it's expensive
    DtedFile(const DtedFile& other) = delete;
    DtedFile& operator=(const DtedFile& other) = delete;

    DtedFile(DtedFile&& other) noexcept;
    DtedFile& operator=(DtedFile&& other) noexcept;

    const UserHeaderLabel& uhl() const { return _uhl; }
    const DataSetIdentification& dsi() const { return _dsi; }
    const AccuracyDescriptionRecord& acc() const { return _acc; }

    void loadFile(const LoadStrategy strategy);

    bool valid() const { return _valid; }
    std::string filename() const { return _filename; }

private:
    bool parseFile(const std::byte* data);
    bool loadElevations(const std::byte* data);

    bool _valid;
    int16_t* _elevations;
    uint16_t _columnCount;
    uint16_t _rowCount;

    UserHeaderLabel _uhl{};
    DataSetIdentification _dsi{};
    AccuracyDescriptionRecord _acc{};

    std::string _filename;
};

} // namespace dted

#endif
