#pragma once

namespace serializer
{
	#ifdef DEF_ARCHIVER
	void convert_files_src(char* fn);
	#endif
	void interpret_archive_src(char* fn);
	void LoadArchive(char* fn);
	void SaveArchive(char* fn);
}