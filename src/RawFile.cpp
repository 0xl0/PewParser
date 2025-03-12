#include "RawFile.h"

namespace PewParser {

    RawFile::RawFile()
        : filepath_(), filename_(), filesize_(0), buffer_(nullptr)
    {
    }

    RawFile::RawFile(const std::filesystem::path& filepath, const std::string& filename, uintmax_t filesize, BYTE* buffer)
        : filepath_(filepath), filename_(filename), filesize_(filesize), buffer_(buffer)
    {
    }

    void RawFile::Delete()
    {
        if(buffer_)
            delete buffer_;
    }

    RawFile::operator bool() const
    {
        return (buffer_);
    }

    bool RawFile::operator!() const
    {
        return !(buffer_);
    }
}
