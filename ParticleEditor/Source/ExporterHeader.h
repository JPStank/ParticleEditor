#ifndef __EXPORTER_HEADER_H__
#define __EXPORTER_HEADER_H__
//--------------------------------------------------------------------------------
//THis file should be included in the loader and the exporter
#include <memory.h>
#include <stdint.h>
#include <sys/stat.h>

#include <time.h>
#include "Log.h"
//--------------------------------------------------------------------------------

//Change this when any changes are done to the struct ExporterHeader
#define EXPORTER_VERSION_NUMBER 1
//--------------------------------------------------------------------------------
namespace FileInfo {
	enum class FILE_TYPES  : int8_t { MESH, BIND_POSE, ANIMATION, NAV_MESH };
	enum class MODEL_TYPES : int8_t { COLOR, TEXTURE, TEXTURE_LIT, NORMALMAP, NORMALMAP_ANIMATED, BASIC, MAX_TYPES };
	enum class INDEX_TYPES : int8_t { INDEX32, INDEX16, TRI_STRIP };

	struct ExporterHeader {
		union {
			struct {
				uint32_t		numPoints;
				uint32_t		numIndex;
				unsigned int	vertSize;
				MODEL_TYPES		modelType;
				INDEX_TYPES		index;
			}mesh;
			struct {
				uint32_t		numBones;
			}bind;
			struct {
				uint32_t		numBones;
				uint32_t		numFrames;
				float			startTime;
				float			endTime;
			}anim;
			struct {
				uint32_t		totalSize;
				uint32_t		rowSize;
				uint32_t		numTriangles;
				uint32_t		elementSize;
			}navMesh;
		};
		FILE_TYPES		file;
		unsigned int	version;
		//Do a check to make sure the file has not been updated since you last changed it
		time_t		updateTime;

		//Used for reading in the header
		ExporterHeader() {}
		//used for writing out the header
		//Rootfile is the file we are exporting to get the last modified time
		ExporterHeader(_In_ FILE_TYPES _type, _In_ const char *_rootFile) {
			memset(this, 0, sizeof(ExporterHeader));
			version = EXPORTER_VERSION_NUMBER;
			file = _type;
			    
			//https://msdn.microsoft.com/query/dev12.query?appId=Dev12IDEF1&l=EN-US&k=k(wchar%2Fstat);k(stat);k(DevLang-C%2B%2B);k(TargetOS-Windows)&rd=true
			struct stat fileProperty;
			int retVal = stat(_rootFile, &fileProperty);
			//save last modified time to the file
			updateTime = fileProperty.st_mtime;
		}
		bool Check_stat(int _val) {
			if (_val == 0) {
				return true;
			}

			switch (errno)
			{
			case ENOENT:
				Log("File  not found.");
				break;
			case EINVAL:
				Log("Invalid parameter to _stat.");
				break;
			default:
				/* Should never be reached. */
				Log("Unexpected error in _stat.");
			}

			return false;
		}
		//Bin File is the binary path to read
		//Data File is the file used to create the bin file
		//Return number and type of errors 
		//Positive means out of date data
		//Negitive means cannot find header
		//zero means zero errors
		int ReadHeader(_Inout_ FILE **_file, _In_ const char *_binFile, _In_ const char *_dataFile) {
			fopen_s(_file, _binFile, "rb");
			fread(this, sizeof(ExporterHeader), 1, *_file);
			if (!_file) {
				Log("Could not open file ", _binFile);
				return -1;
			}

			if (version != EXPORTER_VERSION_NUMBER) {
				Log("ERROR:", _dataFile, " Needs to get exported again bad version");
				return 1;
			}

			//Check to see if the file was modified
			struct stat fileProperty;
			if (!Check_stat(stat(&_dataFile[0], &fileProperty))) {
				Log("Error Getting the file modifiied time for: ", _dataFile);
				//Not sure how to handle this case...pretend everythings OK?
				return 0;
			}
			if (fileProperty.st_mtime != updateTime) {
				Log("ERROR:", _dataFile, " Needs to get exported again\nData file was updated since last export");
				return 0;
			}

			return 0;
		}
		//Bin File is the binary path to read
		//Data File is the file used to create the bin file
		//Return number and type of errors 
		//Positive means out of date data
		//Negitive means cannot find header
		//zero means zero errors
		int ReadHeader(_Inout_ FILE **_file, _In_ const std::string &_binFile, _In_ const std::string & _dataFile) {
			return ReadHeader(_file, &_binFile[0], &_dataFile[0]);
		}
	};
};
//--------------------------------------------------------------------------------
#endif //__EXPORTER_HEADER_H__