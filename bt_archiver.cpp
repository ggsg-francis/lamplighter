#include "serializer.h"
#include "resources-archive.h"

int main(int argc, char * argv[])
{
	serializer::convert_files_src("0000gameassets.uwu");
	serializer::interpret_archive_src("0000gamecontents.uwu");
	serializer::save_archive("resources/tsoa.UwUa");
	//serializer::load_archive("resources/tsoa.UwUa");
	archive::clear_memory();
}