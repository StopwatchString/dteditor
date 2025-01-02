#include "dted/DtedFile.h"

#include <iostream>
#include <fstream>

namespace dted {

//-----------------------------------------------
// Constructor
//-----------------------------------------------
DtedFile::DtedFile(const std::string& filename)
    : m_FileName(filename)
{
    // Get file size
    std::ifstream file(m_FileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "ERROR Cannot open file " << filename << std::endl;
        return;
    }
    m_DataSize = file.tellg();

    // Create heap block sized to the file
    m_Data = std::make_unique<std::byte[]>(m_DataSize);

    // Go to beginning of file, then read into heap block
    file.seekg(0, std::ios::beg);
    if (!file.good()) {
        std::cerr << "ERROR Cannot seek to header!" << std::endl;
        return;
    }
    if (!file.read(reinterpret_cast<char*>(m_Data.get()), m_DataSize)) {
        std::cerr << "ERROR Failed to pull entire file into heap block." << std::endl;
    }



    file.close();
}

//-----------------------------------------------
// Destructor
//-----------------------------------------------
DtedFile::~DtedFile()
{

}

//-----------------------------------------------
// Move Constructor
//----------------------------------------------
DtedFile::DtedFile(DtedFile&& other) noexcept
    : m_FileName(std::move(other.m_FileName)),
      m_DataSize(std::move(other.m_DataSize)),
      m_Data(std::move(other.m_Data))
{
}

//-----------------------------------------------
// Move Assignment
//----------------------------------------------
DtedFile& DtedFile::operator=(DtedFile&& other) noexcept
{
    if (this != &other) {
        m_FileName = std::move(other.m_FileName);
        m_DataSize = std::move(other.m_DataSize);
        m_Data = std::move(other.m_Data);
    }
    return *this;
}

//-----------------------------------------------
// valid()
//----------------------------------------------
bool DtedFile::valid() const
{
    // If we don't have data, then we aren't valid
    if (m_Data.get() == nullptr) return false;
    // If we don't have enough data to have all 3 headers, then we aren't valid
    if (m_DataSize < USER_HEADER_LABEL_BLOB_SIZE + DATA_SET_IDENTIFICATION_BLOB_SIZE + ACCURACTY_DESCRIPTION_RECORD_BLOB_SIZE) return false;

    UserHeaderLabelBlob uhl = *(reinterpret_cast<UserHeaderLabelBlob*>(m_Data.get() + USER_HEADER_LABEL_BLOB_OFFSET));
    DataSetIdentificationBlob dsi = *(reinterpret_cast<DataSetIdentificationBlob*>(m_Data.get() + DATA_SET_IDENTIFICATION_BLOB_OFFSET));
    AccuracyDescriptionRecordBlob acc = *(reinterpret_cast<AccuracyDescriptionRecordBlob*>(m_Data.get() + ACCURACY_DESCRIPTION_RECORD_BLOB_OFFSET));

    return uhl.valid() && dsi.valid() && acc.valid();
}

} // End dted namespace