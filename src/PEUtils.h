#pragma once
#include "PEFormat.h"

#include <string>

namespace PewParser {

    class PEUtils
    {
    public:
        static std::string TimeDateStampConverter(DWORD time);
    };
}