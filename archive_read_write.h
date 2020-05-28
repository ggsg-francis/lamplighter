#ifndef ARCHIVERW_H
#define ARCHIVERW_H

namespace serializer
{
	#ifdef DEF_ARCHIVER
	void convert_files_src(char* fn);
	#endif
	void InterpretArchiveContents(char* fn);
	void LoadArchive(char* fn);
	void SaveArchive(char* fn);
}

#endif
