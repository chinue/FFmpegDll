#ifndef __OPENCV_FFMPEG_H__
#define __OPENCV_FFMPEG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef OPENCV_FFMPEG_API
#if defined(__OPENCV_BUILD) || defined(BUILD_PLUGIN)
#   define OPENCV_FFMPEG_API
#elif defined _WIN32
#   define OPENCV_FFMPEG_API __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#   define OPENCV_FFMPEG_API __attribute__ ((visibility ("default")))
#else
#   define OPENCV_FFMPEG_API
#endif
#endif

#ifndef __OPENCV_BUILD
#define CV_FOURCC(c1, c2, c3, c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))
#endif

#define CV_MAX_FRMAE_NUM (4*1024*1024)
enum CvPixFormat
{
    CV_PIX_FMT_NONE,
    CV_PIX_FMT_I420, CV_PIX_FMT_I444, CV_PIX_FMT_NV12, CV_PIX_FMT_NV21, CV_PIX_FMT_I422H, CV_PIX_FMT_I422V, CV_PIX_FMT_YUYV, CV_PIX_FMT_UYVY, CV_PIX_FMT_GRAY, CV_PIX_FMT_I420P10LE, CV_PIX_FMT_I422H10LE,  // pixel format for Video
    CV_PIX_FMT_J420, CV_PIX_FMT_J444, CV_PIX_FMT_J422H, CV_PIX_FMT_J422V, // pixel format for JPEG
    CV_PIX_FMT_RGB, CV_PIX_FMT_BGR
};
enum CvScaleMethod
{
    CV_FAST_BILINEAR=1, CV_BILINEAR=2, CV_BICUBIC=4, CV_EXPER=8, CV_NEAREST=0x10, CV_AREA=0x20, CV_BICUBLIN=0x40, CV_GAUSS=0x80, CV_SINC=0x100, CV_LANCZOS=0x200, CV_SPLINE=0x400
};
enum CvFrameType
{
    CV_FRAME_TYPE_NONE = 0, ///< Undefined
    CV_FRAME_TYPE_I,     ///< Intra
    CV_FRAME_TYPE_P,     ///< Predicted
    CV_FRAME_TYPE_B,     ///< Bi-dir predicted
    CV_FRAME_TYPE_S,     ///< S(GMC)-VOP MPEG-4
    CV_FRAME_TYPE_SI,    ///< Switching Intra
    CV_FRAME_TYPE_SP,    ///< Switching Predicted
    CV_FRAME_TYPE_BI,    ///< BI type
};
typedef struct CvRational {
    int a; ///< Numerator
    int b; ///< Denominator
} CvRational;
typedef struct CvMotionVector
{
    int32_t source;
    uint8_t blockW, blockH; // Width and height of the block.
    int16_t src_x, src_y; // Absolute source position. Can be outside the frame area.
    int16_t dst_x, dst_y; // Absolute destination position. Can be outside the frame area.
    uint64_t flags; // Extra flag information. Currently unused.
    int32_t motion_x, motion_y; // Motion vector: src_x = dst_x + motion_x / motion_scale; src_y = dst_y + motion_y / motion_scale
    uint16_t motion_scale;
} CvMotionVector;
typedef struct CvQpTable
{
    int8_t *pQP;
    int stride, xNum, yNum;
    int type;
}CvQpTable;
typedef struct CvMbType
{
    uint32_t *pType;
    int stride, xNum, yNum;
}CvMbType;
typedef struct CvFrameYUV
{
    unsigned char* ppData[4];
    int pStride[4];
    int width, height;
    CvPixFormat pixfmt;
}CvFrameYUV;
typedef struct CvFrameInfo
{
    int idxFrame;
    int frameSize; // bytes of current frame after encoding
    CvFrameType frameType; // I-frame, B-frame, P-frame
    int mvSize;
    const CvMotionVector* pMV;
    CvQpTable qpTable;
    CvMbType mbType;
}CvFrameInfo;

typedef enum CvCodecID
{
    CV_CODEC_UNKNOWN, CV_CODEC_H264, CV_CODEC_H265, CV_CODEC_AV1
}CvCodecID;
typedef struct CvStreamInfo
{
    int frameNum;
    int width, height;
    CvPixFormat pixfmt;
    int bitRrate; // average bitrate
    CvRational avgFrameRate; // average frame rate
    double duration;
    CvCodecID codecID;
}CvStreamInfo;

enum
{
    CV_FFMPEG_CAP_PROP_POS_MSEC=0,
    CV_FFMPEG_CAP_PROP_POS_FRAMES=1,
    CV_FFMPEG_CAP_PROP_POS_AVI_RATIO=2,
    CV_FFMPEG_CAP_PROP_FRAME_WIDTH=3,
    CV_FFMPEG_CAP_PROP_FRAME_HEIGHT=4,
    CV_FFMPEG_CAP_PROP_FPS=5,
    CV_FFMPEG_CAP_PROP_FOURCC=6,
    CV_FFMPEG_CAP_PROP_FRAME_COUNT=7,
    CV_FFMPEG_CAP_PROP_FORMAT     =8, //!< Format of the %Mat objects (see Mat::type()) returned by VideoCapture::retrieve().
                                //!< Set value -1 to fetch undecoded RAW video streams (as Mat 8UC1).
    CV_FFMPEG_CAP_PROP_SAR_NUM=40,
    CV_FFMPEG_CAP_PROP_SAR_DEN=41,
    CV_FFMPEG_CAP_PROP_CODEC_PIXEL_FORMAT =46,    //!< (read-only) codec's pixel format. 4-character code - see VideoWriter::fourcc . Subset of [AV_PIX_FMT_*](https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/raw.c) or -1 if unknown
    CV_FFMPEG_CAP_PROP_BITRATE       =47, //!< (read-only) Video bitrate in bits/s
    CV_FFMPEG_CAP_PROP_CODEC_ID
};
typedef struct CvCapture_FFMPEG CvCapture_FFMPEG;
typedef struct CvVideoWriter_FFMPEG CvVideoWriter_FFMPEG;

OPENCV_FFMPEG_API struct CvCapture_FFMPEG* cvCreateFileCapture_FFMPEG(const char* filename);
OPENCV_FFMPEG_API struct CvCapture_FFMPEG* cvCreateFileCapture2_FFMPEG(const char* filename, int thread_num);
OPENCV_FFMPEG_API int cvSetCaptureProperty_FFMPEG(struct CvCapture_FFMPEG* cap, int prop, double value);
OPENCV_FFMPEG_API double cvGetCaptureProperty_FFMPEG(struct CvCapture_FFMPEG* cap, int prop);
OPENCV_FFMPEG_API int cvGrabFrame_FFMPEG(struct CvCapture_FFMPEG* cap);
OPENCV_FFMPEG_API int cvRetrieveFrame_FFMPEG(struct CvCapture_FFMPEG* capture, unsigned char** data, int* step, int* width, int* height, int* cn);
OPENCV_FFMPEG_API const CvFrameInfo* cvRetrieveFrameRGB_FFMPEG(CvCapture_FFMPEG* capture, unsigned char* pData, int stride, int width, int height, CvScaleMethod method);
OPENCV_FFMPEG_API const CvFrameInfo* cvRetrieveFrameYUV_FFMPEG(CvCapture_FFMPEG* pCapture, unsigned char* ppData[4], int pStride[4], int width, int height, CvPixFormat pixfmt, CvScaleMethod method); // ppData[4] should allocate before use, otherwise return CvFrameInfo only
OPENCV_FFMPEG_API const CvFrameInfo* cvGetFrameInfo_FFMPEG(CvCapture_FFMPEG* pCapture); // use cvGrabFrame_FFMPEG before this function
OPENCV_FFMPEG_API const CvFrameYUV* cvGetFrameYUV_FFMPEG(CvCapture_FFMPEG* pCapture);
OPENCV_FFMPEG_API const CvStreamInfo* cvGetStreamInfo_FFMPEG(CvCapture_FFMPEG* pCapture);
OPENCV_FFMPEG_API int cvResizeImage_FFMPEG(unsigned char* ppSrcData[4], int pSrcStride[4], int srcW, int srcH, CvPixFormat srcFmt, unsigned char* ppDstData[4], int pDstStride[4], int dstW, int dstH, CvPixFormat dstFmt, CvScaleMethod method);
OPENCV_FFMPEG_API void cvReleaseCapture_FFMPEG(struct CvCapture_FFMPEG** cap);

OPENCV_FFMPEG_API struct CvVideoWriter_FFMPEG* cvCreateVideoWriter_FFMPEG(const char* filename, int fourcc, double fps, int width, int height, int isColor );
OPENCV_FFMPEG_API int cvWriteFrame_FFMPEG(struct CvVideoWriter_FFMPEG* writer, const unsigned char* data, int step, int width, int height, int cn, int origin);
OPENCV_FFMPEG_API void cvReleaseVideoWriter_FFMPEG(struct CvVideoWriter_FFMPEG** writer);

typedef CvCapture_FFMPEG* (*CvCreateFileCapture_Plugin)( const char* filename );
typedef CvCapture_FFMPEG* (*CvCreateCameraCapture_Plugin)( int index );
typedef int (*CvGrabFrame_Plugin)( CvCapture_FFMPEG* capture_handle );
typedef int (*CvRetrieveFrame_Plugin)( CvCapture_FFMPEG* capture_handle, unsigned char** data, int* step, int* width, int* height, int* cn );
typedef int (*CvSetCaptureProperty_Plugin)( CvCapture_FFMPEG* capture_handle, int prop_id, double value );
typedef double (*CvGetCaptureProperty_Plugin)( CvCapture_FFMPEG* capture_handle, int prop_id );
typedef void (*CvReleaseCapture_Plugin)( CvCapture_FFMPEG** capture_handle );
typedef CvVideoWriter_FFMPEG* (*CvCreateVideoWriter_Plugin)( const char* filename, int fourcc, double fps, int width, int height, int iscolor );
typedef int (*CvWriteFrame_Plugin)( CvVideoWriter_FFMPEG* writer_handle, const unsigned char* data, int step, int width, int height, int cn, int origin);
typedef void (*CvReleaseVideoWriter_Plugin)( CvVideoWriter_FFMPEG** writer );

/*
 * For CUDA encoder
 */

OPENCV_FFMPEG_API struct OutputMediaStream_FFMPEG* create_OutputMediaStream_FFMPEG(const char* fileName, int width, int height, double fps);
OPENCV_FFMPEG_API void release_OutputMediaStream_FFMPEG(struct OutputMediaStream_FFMPEG* stream);
OPENCV_FFMPEG_API void write_OutputMediaStream_FFMPEG(struct OutputMediaStream_FFMPEG* stream, unsigned char* data, int size, int keyFrame);

typedef struct OutputMediaStream_FFMPEG* (*Create_OutputMediaStream_FFMPEG_Plugin)(const char* fileName, int width, int height, double fps);
typedef void (*Release_OutputMediaStream_FFMPEG_Plugin)(struct OutputMediaStream_FFMPEG* stream);
typedef void (*Write_OutputMediaStream_FFMPEG_Plugin)(struct OutputMediaStream_FFMPEG* stream, unsigned char* data, int size, int keyFrame);

/*
 * For CUDA decoder
 */

OPENCV_FFMPEG_API struct InputMediaStream_FFMPEG* create_InputMediaStream_FFMPEG(const char* fileName, int* codec, int* chroma_format, int* width, int* height);
OPENCV_FFMPEG_API void release_InputMediaStream_FFMPEG(struct InputMediaStream_FFMPEG* stream);
OPENCV_FFMPEG_API int read_InputMediaStream_FFMPEG(struct InputMediaStream_FFMPEG* stream, unsigned char** data, int* size, int* endOfFile);

typedef struct InputMediaStream_FFMPEG* (*Create_InputMediaStream_FFMPEG_Plugin)(const char* fileName, int* codec, int* chroma_format, int* width, int* height);
typedef void (*Release_InputMediaStream_FFMPEG_Plugin)(struct InputMediaStream_FFMPEG* stream);
typedef int (*Read_InputMediaStream_FFMPEG_Plugin)(struct InputMediaStream_FFMPEG* stream, unsigned char** data, int* size, int* endOfFile);

#ifdef __cplusplus
}
#endif

#endif
