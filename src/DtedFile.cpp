#include "DtedFile.h"

#include <iostream>
#include <fstream>

DtedFile::DtedFile(const std::string& filename)
    : m_FileName(filename)
{
    std::ifstream file(m_FileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "ERROR Cannot open file " << filename << std::endl;
        return;
    }

    file.seekg(DTED_HEADER_OFFSET, std::ios::beg);
    if (!file.good()) {
        std::cerr << "ERROR Cannot seek to header!" << std::endl;
        return;
    }

    if (!file.read(reinterpret_cast<char*>(&m_Header), DTED_HEADER_SIZE)) {
        std::cerr << "ERROR Cannot read header from file!" << std::endl;
        return;
    }

    file.close();
}