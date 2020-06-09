/************************************************************************/
/* By: Chen Yu (sir.chen100@163.com)                                    */
/* version 1.0 (2016-04-20)                                             */
/* 1. Initial version.                                                  */
/************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef OUT
#   define OUT
#   define IN
#endif


#ifdef __cplusplus
#   define DEFAULT(x) =x
#   define EXTERN_C extern "C"
#else
#   define DEFAULT(x) // =x
#   define EXTERN_C // extern "C"
#endif

#ifdef _WIN32
#   define DLL_API EXTERN_C __declspec(dllexport)
#else
#   define DLL_API EXTERN_C // __attribute__((dllexport))
#endif

typedef  signed   char       int8;
typedef  unsigned char       uint8, Byte;
typedef           short      int16;
typedef  unsigned short      uint16, Word, float16;
typedef           int        int32;
typedef  unsigned int        uint32, DWord;
typedef  void*               Handle;
typedef  unsigned int        RGB4,YUV4;

typedef uint16 uint10;
//typedef Byte uint10;

#if defined(_MSC_VER)
    typedef           __int64    int64;
    typedef  unsigned __int64    uint64;
#else
    typedef           long long  int64;
    typedef  unsigned long long  uint64;
#endif

#endif // __TYPES_H__