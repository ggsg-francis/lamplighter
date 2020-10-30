#define DEF_ONLY_CONVERT

//-------------------------------- WINDOWING GLOBAL VARIABLES

#include "archive_read_write.h"
#include "archive.hpp"

bool focus = true;

void RunSerializer()
{
	serializer::convert_files_src("0000gameassets.uwu");
	serializer::InterpretArchiveContents("0000gamecontents.uwu");
	serializer::SaveArchive(ARCHIVE_FILENAME);
	acv::ClearMemory();
	#ifdef DEF_ARCHIVE_IN_CODE
	serializer::ARCHIVE_to_C();
	#endif
}

int main(int argc, char * argv[])
{
	RunSerializer();
	return 0; // Goodbye
}