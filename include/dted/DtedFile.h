#ifndef DTED_FILE_H
#define DTED_FILE_H

#include "dted/dtedFileDefinitions.h"
#include "dted/dtedParsedStructs.h"

#include <string>
#include <memory>

namespace dted {

class DtedFile
{
public:
    DtedFile(const std::string& file);
    ~DtedFile();

    // Disable copy since it's expensive
    DtedFile(const DtedFile& other) = delete;
    DtedFile& operator=(const DtedFile& other) = delete;

    DtedFile(DtedFile&& other) noexcept;
    DtedFile& operator=(DtedFile&& other) noexcept;

    const UserHeaderLabelBlob& uhl() const { return reinterpret_cast<UserHeaderLabelBlob&>(m_Data.get()[USER_HEADER_LABEL_BLOB_OFFSET]); }
    const DataSetIdentificationBlob& dsi() const { return reinterpret_cast<DataSetIdentificationBlob&>(m_Data.get()[DATA_SET_IDENTIFICATION_BLOB_OFFSET]); }
    const AccuracyDescriptionRecordBlob& acc() const { return reinterpret_cast<AccuracyDescriptionRecordBlob&>(m_Data.get()[ACCURACY_DESCRIPTION_RECORD_BLOB_OFFSET]); }

    bool isDataLoaded();

    bool valid() const;
    std::string filename() const { return m_FileName; }
    const std::streamsize dataSize() const { return m_DataSize; }
    const std::byte* data() const { return m_Data.get(); }



private:
    std::string m_FileName;
    std::streamsize m_DataSize{ 0 };
    std::unique_ptr<std::byte[]> m_Data{nullptr};
};

} // End dted namespace

#endif