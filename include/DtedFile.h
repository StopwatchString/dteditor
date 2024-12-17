#ifndef DTED_FILE_H
#define DTED_FILE_H

#include "dtedFields.h"

#include <string>
#include <memory>

class DtedFile {

public:
    DtedFile(const std::string& file);
    ~DtedFile();

    // Disable copy since it's expensive
    DtedFile(const DtedFile& other) = delete;
    DtedFile& operator=(const DtedFile& other) = delete;

    DtedFile(DtedFile&& other) noexcept;
    DtedFile& operator=(DtedFile&& other) noexcept;

    const UserHeaderLabel& uhl() const { return reinterpret_cast<UserHeaderLabel&>(m_Data.get()[USER_HEADER_LABEL_OFFSET]); }
    const DataSetIdentification& dsi() const { return reinterpret_cast<DataSetIdentification&>(m_Data.get()[DATA_SET_IDENTIFICATION_OFFSET]); }
    const AccuracyDescriptionRecord& acc() const { return reinterpret_cast<AccuracyDescriptionRecord&>(m_Data.get()[ACCURACY_DESCRIPTION_RECORD_OFFSET]); }

    bool valid() const;
    std::string filename() const { return m_FileName; }
    const std::streamsize dataSize() const { return m_DataSize; }
    const std::byte* data() const { return m_Data.get(); }

private:
    std::string m_FileName;
    std::streamsize m_DataSize{ 0 };
    std::unique_ptr<std::byte[]> m_Data{nullptr};
};

#endif