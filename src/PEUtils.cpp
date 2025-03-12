#include "PEUtils.h"

#include <ctime>

namespace PewParser {

    std::string PEUtils::TimeDateStampConverter(DWORD time)
    {
        time_t time_v = (time_t)time;
        tm* gmt = gmtime(&time_v);
        char format_buffer[50];
        strftime(format_buffer, 50, "%A, %d/%m/%Y %H:%M:%S UTC", gmt);
        return format_buffer;
    }
}