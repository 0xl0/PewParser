#pragma once
#include "PEFile.h"

namespace PewParser {

    class PEParser
    {
    public:
        static PEType ValidatePE(const RawFile& raw_file);
        static PEFile* MakePE(const RawFile& raw_file, PEType type);
    };

}