
#ifndef __BASE_H__
#define __BASE_H__

#pragma warning(disable:4996)

#ifdef _WIN32
#define ENABLE_DEBUG  // all debug functions will be disabled if close this macro
#endif

//#define ENABLE_DEBUG  // all debug functions will be disabled if close this macro

#ifdef ENABLE_DEBUG
#   define ENABLE_PRINTF
#   define ENABLE_ASSERT
#   define ENABLE_TIME_LOG
#endif

#define KB 1024.0
#define MB (1024.0*1024.0)
#define GB (1024.0*1024.0*1024.0)

#ifndef MAX
#   define MAX(a, b) ((a)<(b)?(b):(a))
#   define MIN(a, b) ((a)<(b)?(a):(b))
#   define MAX3(a,b,c) MAX(MAX(a,b),c)
#   define MIN3(a,b,c) MIN(MIN(a,b),c) 
#endif

#ifndef BETWEEN
#   define BETWEEN(x, xMin, xMax) ((x)<(xMin)?(xMin):((x)>(xMax)?(xMax):(x)))
#   define ROUND(x) (((x)<0)?0:(((x)>255)?255:(unsigned char)(x)))
#   define SWAP(x, y, t) (t=x, x=y, y=t)
#endif

#ifndef ABS
#   define ABS(a) ((a)<0?-(a):(a))
#endif

#define IDX_R  2
#define IDX_G  1
#define IDX_B  0

#define PowOf2(n) (1<<(n))
#define IsPowOf2(n) ((((n)-1)&(n))==0)
#define TO_LOWERCASE(x) (('A'<=(x) && (x)<='Z')?(x)+('a'-'A'):(x))
#define IS_NUMBER(x) ('0'<=(x) && (x)<='9')
#define IS_LETTER(x) (('a'<=(x) && (x)<='z') || ('A'<=(x) && (x)<='Z'))

#if defined(ENABLE_DEBUG) || defined(_WIN32)
#include "types.h"
namespace cv
{
    char* toString(const char* format, ...);
    void __assert(const char* expression, const char *fullPath, int lineNum);
    
    int LoadData(const char* pFileName, void* pData, int dataSize=0);
    int SaveData(const char* pFileName, void* pData, int dataSize);
}
#endif

#if defined(ENABLE_DEBUG) || defined(_WIN32)
enum PrintTextColor
{
    TEXT_COLOR_BLACK, TEXT_COLOR_DaBLUE, TEXT_COLOR_DaGREEN, TEXT_COLOR_DaCYAN,
    TEXT_COLOR_DaRED, TEXT_COLOR_DaMAGENTA, TEXT_COLOR_GOLDEN, TEXT_COLOR_GRAY,
    TEXT_COLOR_DaGRAY, TEXT_COLOR_BLUE, TEXT_COLOR_GREEN, TEXT_COLOR_CYAN,
    TEXT_COLOR_RED, TEXT_COLOR_MAGENTA, TEXT_COLOR_YELLOW, TEXT_COLOR_WHITE
};
#endif

#if defined(ENABLE_PRINTF)
namespace cv
{
    int _Printf(PrintTextColor textColor, const char* format, ...);
}
#   define Printf(textColor, format, ...) cv::_Printf(textColor, format, ##__VA_ARGS__)
#else
#   define Printf(textColor, format, ...) //FilePrintf(textColor, format, ##__VA_ARGS__)
#endif // ENABLE_PRINTF


#ifdef ENABLE_ASSERT
#   define Assert(x) ((x)||(cv::__assert(#x, __FILE__, __LINE__), 0))
#   define Alert(x) cv::__assert(x, __FILE__, __LINE__)
#else
#   define Assert(x) 0
#   define Alert(x)
#endif // ENABLE_ASSERT

#if defined(ENABLE_TIME_LOG)
namespace cv
{
    struct Time
    {
        static double millisecPerClock;
        double t1, t2;
        static double Init();
        double Start();
        double End(const char* pNote);
    };
}
#   define TIME_DECLARE() cv::Time _t
#   define TIME_BEGIN() _t.Start()
#   define TIME_END(x) _t.End(x)
#else
#   define TIME_DECLARE()
#   define TIME_BEGIN()
#   define TIME_END(x)
#endif // ENABLE_TIME_LOG

#ifdef _DEBUG
#   define Printf_DEBUG(textColor, format, ...) cv::_Printf(textColor, format, ##__VA_ARGS__)
#else
#   define Printf_DEBUG(textColor, format, ...)
#endif

#endif // __BASE_H__