#include "archive_read_write.h"
#include "archive.hpp"

int main(int argc, char * argv[])
{
	serializer::convert_files_src("0000gameassets.uwu");
	serializer::InterpretArchiveContents("0000gamecontents.uwu");
	serializer::SaveArchive("res/archive.UwUa");
	acv::ClearMemory();
}