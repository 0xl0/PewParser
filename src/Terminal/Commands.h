#pragma once
#include <string>
#include <iostream>

#include <PEFile.h>

#include "Log.h"

namespace PewParser {

	class Commands
	{
	public:
		enum class Command
		{
			DOS_HDR = 0, FILE_HDR, OPT_HDR, SEC_HDRS,
			EXPORT_DIR, EXPORTS, IMPORTS,
			RSRC_DIR, DEBUG_DIR, BOUND_IMPORTS,
			INVALID
		};
	public:
		Commands(PEFile* pe);

		void PrintDosHdr();
		void PrintFileHdr();
		void PrintOptHdr();
		void PrintSecHdrs();
		//DataDir
		void PrintExportDir();
		void PrintExports();
		void PrintImports();
		void PrintRsrcDir();
		void PrintDebugDir();
		void PrintBoundImportsDir();

		Command ParseCommands(const std::string& cmd);

		void Listen();
	private:
		bool listening_;

		PEFile* loaded_pe_;
	};

}
