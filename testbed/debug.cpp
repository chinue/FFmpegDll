// DataBase.cpp: implementation of the DataType class.
//
//////////////////////////////////////////////////////////////////////

#include "debug.h"
#if defined(ENABLE_DEBUG) || defined(_WIN32)
#pragma message("ENABLE_DEBUG")
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "math.h"
namespace cv
{
#define MAX_BUF_SIZE (1024*512)
    char* toString(const char* format, ...)
    {
        static char buf[MAX_BUF_SIZE];
        static int idx=0;
        char* pBuf;
        va_list varList;

        va_start(varList, format);
        pBuf=buf+idx;
        idx+=vsprintf(pBuf, format, varList)+1;
        va_end(varList);
        if(idx>MAX_BUF_SIZE/2)
            idx=0;
        return pBuf;
    }
#ifdef ENABLE_PRINTF
#include "stdio.h"
#ifdef _WIN32
#include "windows.h"
#define forceinline __forceinline
    typedef CRITICAL_SECTION Mutex;
    static forceinline Mutex* createMutex() { static Mutex g_Mutex; InitializeCriticalSection(&g_Mutex); return &g_Mutex; }
    static Mutex* g_pMutex=createMutex();
    static forceinline void mutexLock(){ EnterCriticalSection(g_pMutex); }
    static forceinline void mutexUnlock(){ LeaveCriticalSection(g_pMutex); }
    int _Printf(PrintTextColor textColor, const char* format, ...)
    {
        int n=0;
        va_list varList;
        mutexLock();
        va_start(varList, format);
        if(textColor!=TEXT_COLOR_WHITE)
        {
            HANDLE hWnd=GetStdHandle(STD_OUTPUT_HANDLE);
            // format=va_arg(varList, char*);       
            SetConsoleTextAttribute(hWnd, (WORD)(textColor/*|BACKGROUND_INTENSITY*/));
            n=vprintf(format, varList);
            SetConsoleTextAttribute(hWnd, (WORD)TEXT_COLOR_WHITE);
        }
        else
        {
            n=vprintf(format, varList);
        }
        va_end(varList);
        fflush(stdout);
        mutexUnlock();
        return n;
    }
#else
#   pragma push_macro("FORCE_INLINE")
#   define forceinline       inline __attribute__((always_inline))
#include <pthread.h>
#include <unistd.h>
#include "semaphore.h"
#include "stdlib.h"
    typedef pthread_mutex_t Mutex;
    static forceinline Mutex* createMutex()
    {
        static pthread_mutex_t g_Mutex;
        pthread_mutexattr_t attr;
        int ret = pthread_mutexattr_init(&attr); /* * resetting to its default value: private */
        ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
        pthread_mutex_init(&g_Mutex, &attr);
        return &g_Mutex;
    }
    static Mutex* g_pMutex=createMutex();
    static forceinline void mutexLock(){ pthread_mutex_lock(g_pMutex); }
    static forceinline void mutexUnlock(){ pthread_mutex_unlock(g_pMutex); }
    // {"end":"\033[0m", "black":"\033[0;30m", "red":"\033[1;31m", "green":"\033[1;32m", "yellow":"\033[1;33m", 
    // "blue":"\033[1;34m", "magenta":"\033[1;35m", "pink":"\033[1;35m", "cyan":"\033[1;36m", "white":"\033[1;37m","gray":"\033[0;37m",
    // "dark blue":"\033[0;34m", "dark red":"\033[0;31m", "dark green":"\033[0;32m", "golden":"\033[0;33m", 
    // "dark magenta":"\033[0;35m", "dark gray":"\033[0;37m", "dark cyan":"\033[0;36m"}
    int _Printf(PrintTextColor textColor, const char* format, ...)
    {
        static const char* ppColorList[]={"\033[0;30m", "\033[0;34m", "\033[0;32m", "\033[0;36m",
            "\033[0;31m", "\033[1;35m", "\033[0;33m", "\033[0;37m", "\033[0;37m", "\033[1;34m", "\033[1;32m", "\033[1;36m", "\033[1;31m", "\033[1;35m", "\033[1;33m", "\033[1;37m"
        };
        static const char* pEnd="\033[0m";
        int n=0;
        va_list varList;
        mutexLock();
        va_start(varList, format);
#ifdef ENABLE_ARM_NEON
        n=vprintf(format, varList);
#else
        printf("%s", ppColorList[textColor]);
        n=vprintf(format, varList);
        printf("%s", pEnd);
#endif
        va_end(varList);
        fflush(stdout);
        mutexUnlock();
        return n;
    }
#endif
#endif

#if defined(ENABLE_ASSERT)
#include "stdio.h"
#ifdef _WIN32
#include "windows.h"
    void __assert(const char* expression, const char *fullPath, int lineNum)
    {
        int len=0;
        char buf[256];
        while(fullPath[len]!=0)
            len++;
        while(len>=0 && fullPath[len]!='\\' && fullPath[len]!='/')
            len--;
        sprintf(buf, "File: %s\r\nLine: %d\r\n\r\n\tAssert Failed: %s\r\n\tDo you want abort\r\n", fullPath+len+1, lineNum, expression);
        Printf(TEXT_COLOR_RED, buf);
        if(IDYES==MessageBox(0, buf, "AssertFailed", MB_YESNO))
            exit(-1);
    }
#else
    void __assert(const char* expression, const char *fullPath, int lineNum)
    {
        int len=0;
        char buf[256];
        while(fullPath[len]!=0)
            len++;
        while(len>=0 && fullPath[len]!='\\' && fullPath[len]!='/')
            len--;
        sprintf(buf, "File: %s\r\nLine: %d\r\n\r\n\tAssert Failed: %s\r\n\tDo you want abort\r\n", fullPath+len+1, lineNum, expression);
        Printf(TEXT_COLOR_RED, buf);
    }
#endif
#endif

#ifdef ENABLE_TIME_LOG
    double Time::millisecPerClock=Time::Init();
#ifdef _WIN32
#include "windows.h"
    double Time::Init()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return 1000.0/(double)freq.QuadPart;
    }
    double Time::Start()
    {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return t1=(double)t.QuadPart;
    }
    double Time::End(const char* pNote)
    {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        t2=(double)t.QuadPart;
        double millisec=(t2-t1)*millisecPerClock;
        t1=t2;
//#ifdef ENABLE_PRINTF
        if(pNote)
        {
            if(millisec<1)
                Printf(TEXT_COLOR_YELLOW, "%s: %.3f us\n", pNote, millisec*1000);
            else if(millisec<1000)
                Printf(TEXT_COLOR_GREEN, "%s: %.3f ms\n", pNote, millisec);
            else Printf(TEXT_COLOR_RED, "%s: %.2f sec\n", pNote, millisec/1000);
            //WriteLog("%s: %.3f ms\n", pNote, millisec);
        }
        else
        {
            if(millisec<1)
                Printf(TEXT_COLOR_YELLOW, "  %.3f us\n", millisec*1000);
            else if(millisec<1000)
                Printf(TEXT_COLOR_GREEN, "  %.3f ms\n", millisec);
            else Printf(TEXT_COLOR_RED, "  %.2f sec\n", millisec/1000);
            //WriteLog("  %.3f ms\n", millisec);
        }
//#endif
        return millisec;
    }
#else // !_WIN32
#include<sys/time.h>
#include<unistd.h>
    double Time::Init()
    {
        return 1/1000.0;
    }
    double Time::Start()
    {
        timeval tv;
        gettimeofday(&tv, NULL);
        t1=(double)(tv.tv_sec*1000000.0 + tv.tv_usec);
        return t1;
    }
    double Time::End(const char* pNote)
    {
        timeval tv;
        gettimeofday(&tv, NULL);
        t2=(double)(tv.tv_sec*1000000.0 + tv.tv_usec);
        double millisec=(t2-t1)*millisecPerClock;
        t1=t2;
        if(pNote)
        {
            if(millisec<1)
                Printf(TEXT_COLOR_YELLOW, "%s: %.3f us\n", pNote, millisec*1000);
            else if(millisec<1000)
                Printf(TEXT_COLOR_GREEN, "%s: %.3f ms\n", pNote, millisec);
            else Printf(TEXT_COLOR_RED, "%s: %.1f sec\n", pNote, millisec/1000);
            //WriteLog("%s: %.3f ms\n", pNote, millisec);
        }
        else
        {
            if(millisec<1)
                Printf(TEXT_COLOR_YELLOW, "  %.3f us\n", millisec*1000);
            else if(millisec<1000)
                Printf(TEXT_COLOR_GREEN, "  %.3f ms\n", millisec);
            else Printf(TEXT_COLOR_RED, "  %.1f sec\n", millisec/1000);
            //WriteLog("  %.3f ms\n", millisec);
        }
        return millisec;
    }
#endif // _WIN32
#endif // ENABLE_TIME_LOG

    int LoadData(const char* pFileName, void* pData, int dataSize)
    {
        FILE* fp = fopen(pFileName, "rb");
        if(fp == 0)
        {
            Printf(TEXT_COLOR_RED, "Load file '%s' faild\n", pFileName);
            return 0;
        }
        if(dataSize <= 0)
        {
            fseek(fp, 0, SEEK_END);
            dataSize = ftell(fp);
            rewind(fp);
        }
        fread(pData, dataSize, 1, fp);
        fclose(fp);
        return dataSize;
    }
    int SaveData(const char* pFileName, void* pData, int dataSize)
    {
        FILE* fp = fopen(pFileName, "wb");
        if(fp == 0)
        {
            Printf(TEXT_COLOR_RED, "Save file '%s' faild\n", pFileName);
            return 0;
        }
        fwrite(pData, dataSize, 1, fp);
        fclose(fp);
        return dataSize;
    }
}

#endif // ENABLE_DEBUG
