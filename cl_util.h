#ifndef CL_UTIL_H
#define CL_UTIL_H

#include<iostream>
#include<OpenCL/opencl.h>

#define CHK_ERROR(err, str) ((err) ? printf("%s returned error %d\n", str, err) : printf("%s returned success\n", str));


//Without this statement resv1 & resv2 are paaded with 2 bytes to be algined on a 4-byte boundary. 
//This won't work. They have to be 2 bytes each without padding. 
//Else you land up reading values incorrectly from the bmp_header.
#pragma pack(2)

typedef struct {
  unsigned short signature; //signature = 0x4D42
  unsigned int size; //size fo bmp file in bytes
  unsigned short resv1; //reserved 1
  unsigned short resv2; //reserved 2
  unsigned int offset; //offset to start of image data in bytes
} struct_bmp_header;

#pragma pack()

typedef struct {
  unsigned int structure_size; //sizeof struct_bmp_info_header structure
  unsigned int width; //image width in pixels
  unsigned int height; //image height in pixels
  unsigned short num_planes; //number of planes
  unsigned short bpp; //bits per pixel
  unsigned int compression_type; // 0=none, 1=RLE-8, 2=RLE-4
  unsigned int image_size; //size of image data in bytes
  unsigned int hres; //horizontal resolution in pixels per meter
  unsigned int vres; //vertical resolution in pixels per meter
  unsigned int num_colors; //number of colors in the image
  unsigned int num_imp_colors; //number of important colors
} struct_bmp_info_header;

int* readBmp(const char* /*filename*/, int* /*width*/, int* /*height*/);
int writeBmp(const char* /*filename*/, const int* /*image data*/, int /*width*/, int /*height*/);
cl_device_id getDeviceId();
cl_kernel getKernel(cl_context /*context*/, cl_device_id /*device_id*/);

#endif /*CL_UTIL_H*/
