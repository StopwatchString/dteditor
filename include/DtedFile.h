#ifndef DTED_FILE_H
#define DTED_FILE_H

#include "dtedFields.h"

#include <string>

class DtedFile {

public:
    DtedFile(const std::string& file);

    const UserHeaderLabel& uhl() const { return m_Header.uhl; }
    const DataSetIdentification& dsi() const { return m_Header.dsi; }
    const AccuracyDescriptionRecord& acc() const { return m_Header.acc; }

    bool valid() { return m_Header.valid(); }

private:
    const std::string m_FileName;

    DtedHeader m_Header;
};

#endif