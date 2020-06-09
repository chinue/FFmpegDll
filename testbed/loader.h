#ifndef __LOADER_H__
#define __LOADER_H__

#include "windows.h"
struct Loader
{
    HMODULE hModule;
    Loader(const char* filename)
    {
        hModule=LoadLibrary(filename);
    }
    ~Loader()
    {
        if(hModule)
            FreeLibrary(hModule);
    }
    template<typename T>
    T GetAddress(const char* pFuncName)
    {
        return (T)GetProcAddress(hModule, pFuncName);
    }
};

#define CV_GET_PROC_ADDRESS(loader, func) (loader).GetAddress<decltype(&func)>(#func)

#endif
