#include "serializer.h"
#include "archive.hpp"

int main(int argc, char * argv[])
{
	serializer::convert_files_src("0000gameassets.uwu");
	serializer::interpret_archive_src("0000gamecontents.uwu");
	serializer::save_archive("res/archive.UwUa");
	//serializer::load_archive("resources/tsoa.UwUa");
	archive::ClearMemory();
}