#include "filesystem.h"
#include <portable-file-dialogs/portable-file-dialogs.h>
#include <fstream>


void RunningDirectory(char* path, size_t length)
{
#ifdef _WIN32
    GetCurrentDirectory(length, path);
#else
#error UNSUPPORTED_PLATFORM
#endif

}

bool filesystem::SaveFile(const char* path, char* data)
{
    std::ofstream file;
    file.open(path);
    file << data;
    file.close();
    return false;
}

bool filesystem::SaveFileWithDialog(char* data, const char* fileType)
{

    char runningDir[128];
    RunningDirectory(runningDir, 128);

    pfd::save_file f = pfd::save_file("Choose files to read", runningDir,
        { fileType, fileType,
          "All Files", "*" });
    
    std::string path = f.result();

    if (path.find_last_of('.') == std::string::npos)
    {
        // No extension... Add one on
        
        // We have a * infront of fileType. Skip over it.
        const char* ext = strchr(fileType, '.');
        if (ext)
            path.append(ext);
    }

    SaveFile(path.c_str(), data);

    return false;
}

char* filesystem::LoadFile(const char* path, size_t& length)
{

    FILE* file = fopen(path, "r");
    if (!file)
    {
        printf("Failed to read file: %s\n", path);
        return nullptr;
    }

    fseek(file, 0L, SEEK_END);
    length = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* data = (char*)calloc(length+1,1);

    fread(data, 1, length, file);
    fclose(file);

    data[length] = 0;

    return data;
}
