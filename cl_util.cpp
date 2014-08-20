#include<iostream>
#include"cl_util.h"

//should do better than making it global  
struct_bmp_header bmp_header;
struct_bmp_info_header bmp_info_header;

int writeBmp(const char* filename, const int* data, int w, int h)
{
  FILE *fp;
  fp = fopen(filename, "wb");
  if(!fp){
    printf("File open error\n");
    return -1;
  }

  fwrite(&bmp_header, sizeof(struct_bmp_header), 1, fp);
  fwrite(&bmp_info_header, sizeof(struct_bmp_info_header), 1, fp);

  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){

       int pixel = data[j+i*w];

       int r = pixel & 0xff;
       int g = (pixel >> 8) & 0xff;
       int b = (pixel >> 16) & 0xff;
       fputc(r, fp);
       fputc(g, fp);
       fputc(b, fp);
    }
 }
 fclose(fp);

 printf("Wrote Output File out.bmp\n");
 return 0;
}

int* readBmp(const char* filename, int* w, int* h)
{
 FILE* fp;
 fp = fopen(filename, "rb");
 if(!fp) {
   printf("File open error\n");
   return NULL;
 }

 fread(&bmp_header, sizeof(struct_bmp_header), 1, fp);
 fread(&bmp_info_header, sizeof(struct_bmp_info_header), 1 , fp);

 #ifdef DEBUG
 printf("Debug Info\n");
 printf("signature %x\n", bmp_header.signature);
 printf("size %d\n", bmp_header.size);

 printf("structure size %d\n", bmp_info_header.structure_size);
 printf("width %d\n", bmp_info_header.width);
 printf("height %d\n", bmp_info_header.height);
 printf("number of planes %u\n", bmp_info_header.num_planes);
 printf("bpp %d\n", bmp_info_header.bpp);
 printf("compression type %d\n", bmp_info_header.compression_type);
 printf("image size %d\n", bmp_info_header.image_size);
 printf("horizontal resolution %d\n", bmp_info_header.hres);
 printf("vertical resolution %d\n", bmp_info_header.vres);
 printf("number of colors %d\n", bmp_info_header.num_colors);
 printf("number of important colors %d\n", bmp_info_header.num_imp_colors);
 #endif

 fseek(fp, bmp_header.offset, SEEK_SET);

 int height = bmp_info_header.height;
 int width  = bmp_info_header.width;

 *h = height;
 *w = width;


 int* data = (int*)malloc(sizeof(int) * width * height);

 for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
       int r = fgetc(fp);
       int g = fgetc(fp);
       int b = fgetc(fp);
       int pixel = r | (g << 8) | (b << 16);
       data[j+i*width] = pixel;
    }
 }
 fclose(fp);

 return data;
}

cl_device_id getDeviceId()
{
   int err = 0;
   cl_device_id* device_id;
   cl_uint num = 0;
   int dev_num = 0;
   char** deviceinfostr;

   //Get Number of Device IDs
   err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 0, NULL, &num);
   CHK_ERROR(err, "clGetDeviceIds");

   device_id = new cl_device_id[num];
   deviceinfostr = new char*[num];

   //Get Device IDs
   err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, num, device_id, NULL);
   CHK_ERROR(err, "clGetDeviceIds");

   printf("Available OpenCL Devices\n");

   for(int i = 0; i < num; i++)
   {
      deviceinfostr[i] = new char[MAX_DEVICE_NAME_LENGTH];
      //Get Device Information
      err = clGetDeviceInfo(device_id[i], CL_DEVICE_NAME, MAX_DEVICE_NAME_LENGTH, deviceinfostr[i], NULL);
      CHK_ERROR(err, "clGetDeviceInfo");
      printf("CL Device %d: %s\n", i+1, deviceinfostr[i]);
   }

   printf("Enter CL device number to select a device");
   printf("(Valid selections from 1 to %d):", num);
   scanf("%d", &dev_num);

   if(dev_num < 1 || dev_num > num)
   {
      printf("Invalid Device\n");
      exit(1);
   }

   return [dev_num-1];
}

cl_kernel getKernel(cl_context context, cl_device_id device_id)
{
   FILE* fp;
   size_t file_size;
   char* src_string;
   cl_program program;
   int err = 0;
   cl_kernel kernel;

   char program_source[2048];
   fp = fopen ( "opencl_sample.cl" , "r" );
   if(fp == NULL){
     printf("File open error\n");
     return NULL;
   }

  // get the file size:
  fseek (fp , 0 , SEEK_END);
  file_size = ftell (fp);
  rewind (fp);

  //allocate memory to store the cl file
  src_string = (char*) malloc(sizeof(char) * file_size);

  //copy contents of cl file into string
  fread(src_string,1,file_size,fp);

  //Create program from source
  program = clCreateProgramWithSource(
                context, 
                1, 
                (const char**)&src_string, 
                &file_size, 
                &err);
  CHK_ERROR(err, "clCreateProgramWithSource");

  //Build the program - TBD check for build errors
  err = clBuildProgram(
                program,
                1,
                &device_id,
                NULL,
                NULL,
                NULL);

  CHK_ERROR(err, "clBuildProgram");

  //Create kernel object
  kernel = clCreateKernel(
                program,
                "image_filter",
                &err);
  CHK_ERROR(err, "clCreateKernel");
  
  return kernel;
}

void queryTimingInfo(cl_event event)
{

  cl_ulong time_start, time_end;
  double total_time;
  int err;
 
  err = clGetEventProfilingInfo(
		event,
		CL_PROFILING_COMMAND_START,
 		sizeof(cl_ulong),
        	&time_start,
 		NULL);

  err |= clGetEventProfilingInfo(
		event,
		CL_PROFILING_COMMAND_END,
 		sizeof(cl_ulong),
        	&time_end,
 		NULL);
  CHK_ERROR(err, "clGetEventProfilingInfo");

  total_time = time_end - time_start;

  printf("Kernel Execution Time on Device: %f milliseconds\n", total_time/1e6f); 

}
