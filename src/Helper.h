#pragma once
#include "RawFile.h"

#include <fstream>
#include <filesystem>

namespace PewParser {

    static RawFile LoadFile(const std::filesystem::path& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        if (file.is_open())
        {
            std::string filename = filepath.filename().u8string();
            uintmax_t filesize = std::filesystem::file_size(filepath);

            BYTE* buffer = new BYTE[filesize];
            file.read((char*)buffer, filesize);
            file.close();

            return RawFile(filepath, filename, filesize, buffer);
        }

        return RawFile();
    }

}
