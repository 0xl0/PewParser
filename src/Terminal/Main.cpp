#include <PewParser/PewParser.h>

#include "Platform.h"
#include "Terminal.h"

int PewMain(int argc, arg_t* argv[])
{
    using namespace PewParser;

    Terminal terminal;

    if (argc > 1)
    {
        terminal.SetFilepath(argv[1]);
        terminal.PrintFilepath();
    }
    else
        terminal.NewFilepath();

    do
    {
        RawFile raw_file = LoadFile(terminal.GetFilepath());
        if (raw_file)
        {
            PEType pe_type = PEParser::ValidatePE(raw_file);

            if (pe_type == PEType::NotPE)
            {
                PEW_ERROR("File is not portable executable\n");
                raw_file.Delete();
                terminal.NewFilepath();
                continue;
            }
            else if (pe_type == PEType::Corrupted)
            {
                PEW_ERROR("PE is corrupted\n");
                raw_file.Delete();
                terminal.NewFilepath();
                continue;
            }

            PEFile* pe = PEParser::MakePE(raw_file, pe_type);

            terminal.Listen(pe);
            delete pe;
        }
        else
        {
            PEW_ERROR("Failed to load file\n");
            terminal.NewFilepath();
        }

    } while (true);
}
