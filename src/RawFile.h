#pragma once
#include <string>
#include <filesystem>

typedef unsigned char BYTE;

namespace PewParser {

    class RawFile
    {
    public:
        RawFile();
        RawFile(const std::filesystem::path& filepath, const std::string& filename, uintmax_t filesize, BYTE* buffer);

        std::filesystem::path Path() const { return filepath_; }
        std::string Name() const { return filename_; }
        uintmax_t Size() const { return filesize_; }
        BYTE* Buffer() const { return buffer_; }

        void Delete();

        explicit operator bool() const;
        bool operator!() const;
    private:
        std::filesystem::path filepath_;
        std::string filename_;
        uintmax_t filesize_;
        BYTE* buffer_;
    };

}
