#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define HEADER_TYPE_READY 0
#define HEADER_TYPE_PLAYER_UPDATES 1
#define HEADER_TYPE_INDEX 2
#define HEADER_TYPE_ENGINE 3
#define HEADER_TYPE_VICTORY 4

#ifdef _WIN32
    void bzero(char *arr, size_t size)
    {
        for(unsigned int i = 0; i < size; i++)
        {
            arr[i] = 0;
        }
    }

    string to_string(int num)
    {
        stringstream strm;
        strm.str("");

        strm << num;

        return strm.str();
    }
#endif

#endif
