#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cap_ffmpeg_api.hpp"
#include "debug.h"

#define CV_FOURCC(c1, c2, c3, c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))
#define MIN(a,b) ((a)<(b)?(a):(b))

static void test_decode_speed()
{
    TIME_DECLARE();
    char *fileName="D:/code/FFmpegDll/input/A3.mp4";
    TIME_BEGIN();
    CvCapture_FFMPEG* pCap=cvCreateFileCapture_FFMPEG(fileName);
    if(pCap==0)
    {
        printf("failed\n");
        return;
    }
    TIME_END("cvCreateFileCapture_FFMPEG");
    int w=(int)cvGetCaptureProperty_FFMPEG(pCap, CV_FFMPEG_CAP_PROP_FRAME_WIDTH);
    int h=(int)cvGetCaptureProperty_FFMPEG(pCap, CV_FFMPEG_CAP_PROP_FRAME_HEIGHT);
    int frameNum=(int)cvGetCaptureProperty_FFMPEG(pCap, CV_FFMPEG_CAP_PROP_FRAME_COUNT);
    printf("w=%d h=%d num=%d\n", w, h, frameNum);
    const CvStreamInfo* pInfo=cvGetStreamInfo_FFMPEG(pCap);
    printf("StreamInfo: num=%d  w=%d h=%d pixfmt=%d  bitRate=%d B  avg_frame_rate=%d/%d  duration=%f\n", pInfo->frameNum, pInfo->width, pInfo->height, pInfo->pixfmt, pInfo->bitRrate, pInfo->avgFrameRate.a, pInfo->avgFrameRate.b, pInfo->duration);
    int pStrideX[4]={w,w/2,w/2};
    unsigned char* pBuf=new unsigned char[w*h*3];
    unsigned char* ppDataX[3]={pBuf, pBuf+w*h, pBuf+w*h+w*h/4};
    //ff_cap_set(pCap, FFMPEG_PROP_PIXEL_FORMAT, AV_PIX_FMT_YUV420P);
    frameNum=MIN(frameNum, 4024);
    int *pFrameSize=new int[frameNum];
    char*pFrameType=new char[frameNum];
    TIME_BEGIN();
    for(int i=0; i<frameNum; i++)
    {
        //int w, h, cn;
        cvGrabFrame_FFMPEG(pCap);
        const CvFrameInfo* pInfo=cvGetFrameInfo_FFMPEG(pCap);
        pFrameType[i]=pInfo->frameType;
        pFrameSize[i]=pInfo->frameSize;
        int8* pQP=pInfo->qpTable.pQP;
        uint32* pMT=pInfo->mbType.pType;
        if(pQP && pMT)
        Printf_DEBUG(TEXT_COLOR_CYAN, "%4d/%d: qp=[%d %d %d %d]\tmbType=[%d %d %d %d]\n", i+1, frameNum, pQP[0], pQP[1], pQP[2], pQP[3], pMT[0], pMT[1], pMT[2], pMT[3]);

        PrintTextColor pColor[]={TEXT_COLOR_WHITE,TEXT_COLOR_RED, TEXT_COLOR_BLUE, TEXT_COLOR_GREEN};
        const char* pType[]={"x", "I", "P", "B", "S", "SI", "SP", "BI"};
        Printf_DEBUG(pColor[pFrameType[i]], "%3d/%d: %s-Frame [%5d B]\n", i+1, frameNum, pType[pFrameType[i]], pFrameSize[i]);
    }
    double t=TIME_END("Decode");
    cvReleaseCapture_FFMPEG(&pCap);
    Printf(TEXT_COLOR_GREEN, "%.2f ms/frame\n", t/frameNum);
//     for(int i=0; i<frameNum; i++)
//     {
//         PrintTextColor pColor[]={TEXT_COLOR_WHITE,TEXT_COLOR_RED, TEXT_COLOR_BLUE, TEXT_COLOR_GREEN};
//         const char* pType[]={"x", "I", "P", "B", "S", "SI", "SP", "BI"};
//         Printf(pColor[pFrameType[i]], "%3d/%d: %s-Frame [%5d B]\n", i+1, frameNum, pType[pFrameType[i]], pFrameSize[i]);
//     }
    delete[]pFrameSize;
    delete[]pFrameType;
}
static void test_decode()
{
    char *fileName="D:/code/FFmpegDll/input/A3.mp4";
    CvCapture_FFMPEG* pCap=cvCreateFileCapture_FFMPEG(fileName);
    if(pCap==0)
    {
        printf("failed\n");
        return;
    }
    int w=(int)cvGetCaptureProperty_FFMPEG(pCap, CV_FFMPEG_CAP_PROP_FRAME_WIDTH);
    int h=(int)cvGetCaptureProperty_FFMPEG(pCap, CV_FFMPEG_CAP_PROP_FRAME_HEIGHT);
    int frameNum=(int)cvGetCaptureProperty_FFMPEG(pCap, CV_FFMPEG_CAP_PROP_FRAME_COUNT);
    int bitrate=(int)cvGetCaptureProperty_FFMPEG(pCap, CV_FFMPEG_CAP_PROP_BITRATE);
    printf("w=%d h=%d num=%d  bitrate=%d B/s\n", w, h, frameNum, bitrate);
    unsigned char* ppData[4]={0};
    int pStep[4]={0};
    int pStrideX[4]={w,w/2,w/2};
    unsigned char* pBuf=new unsigned char[w*h*3];
    unsigned char* ppDataX[3]={pBuf, pBuf+w*h, pBuf+w*h+w*h/4};
    //ff_cap_set(pCap, FFMPEG_PROP_PIXEL_FORMAT, AV_PIX_FMT_YUV420P);
    char filename[256];
    sprintf(filename, "D:/code/FFmpegDll/input/A3_%dx%d.I420", w, h);
    FILE* fp=fopen(filename, "wb");
    frameNum=MIN(frameNum, 500);
//frameNum=25*25;
    for(int i=0; i<frameNum; i++)
    {
        //int w, h, cn;
        if(cvGrabFrame_FFMPEG(pCap)==0)
            break;
        const CvFrameInfo* pInfo=cvRetrieveFrameYUV_FFMPEG(pCap, ppDataX, pStrideX, w, h, CV_PIX_FMT_I420, CV_BICUBIC);
        if(pInfo)
        {
            PrintTextColor pColor[]={TEXT_COLOR_WHITE,TEXT_COLOR_RED, TEXT_COLOR_BLUE, TEXT_COLOR_GREEN};
            const char* pType[]={"x", "I", "P", "B", "S", "SI", "SP", "BI"};
            Printf(pColor[pInfo->frameType], "%3d/%d: %s-Frame [%5d B] mvSize=%d\n", pInfo->idxFrame+1, frameNum, pType[pInfo->frameType], pInfo->frameSize, pInfo->mvSize);
        }
        if(fp)
        {
            fwrite(ppDataX[0], pStrideX[0], h, fp);
            fwrite(ppDataX[1], pStrideX[1], h/2, fp);
            fwrite(ppDataX[2], pStrideX[2], h/2, fp);
        }
        //cvRetrieveFrame_FFMPEG(pCap, ppData, pStep, &w, &h, &cn);
        //printf("%3d/%d:  step=[%d %d %d]\n", i+1, frameNum, pStep[0], pStep[1], pStep[2]);
//         if(fp)
//         {
//             fwrite(ppData[0], pStep[0], h, fp);
//             fwrite(ppData[1], pStep[1], h, fp);
//             fwrite(ppData[2], pStep[2], h, fp);
//         }
    }
    fclose(fp);
    cvReleaseCapture_FFMPEG(&pCap);
}
#define Align(x) (((x)+32-1)/32*32)
static void test_encode()
{
   int w=960, h=540;
   int fcc= CV_FOURCC('H', '2', '6', '4');
   //int fcc= CV_FOURCC('A', 'V', '1', ' ');
   CvVideoWriter_FFMPEG* pCap=cvCreateVideoWriter_FFMPEG("D:/code/FFmpegDll/input/out.mp4", fcc, 10, w, h, 1);
   int frameNum=10;
   printf("w=%d h=%d num=%d\n", w, h, frameNum);
   if(pCap==0)
   {
       printf("CreateWriter failed\n");
       return;
   }
   unsigned char* ppData[4]={0};
   int step=(w*3), cn=3;
   int pStep[4]={step, 0, 0, 0};
   char filename[256];
   unsigned char* pDataY=(unsigned char*)malloc(h*step);
//    unsigned char* pDataU=pDataY+w*h;
//    unsigned char* pDataV=pDataU+w*h/4;
   sprintf(filename, "D:/code/FFmpegDll/input/av1_540p_%dx%d.BGR", w, h);
   FILE* fp=fopen(filename, "rb");
   for(int i=0; i<frameNum; i++)
   {
       if(fp)
       {
           fread(pDataY, h*step, 1, fp);
       }
       //ff_writer_write(pCap, pDataY);
       cvWriteFrame_FFMPEG(pCap, pDataY, step, w, h, cn, 0);
       printf("%3d/%d:  step=[%d %d %d]\n", i+1, frameNum, pStep[0], pStep[1], pStep[2]);
   }
   fclose(fp);
   cvReleaseVideoWriter_FFMPEG(&pCap);
   free(pDataY);
}
void main()
{
    //test_decode_speed();
    test_decode();
    //test_encode();
}
