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

  printf("width %d height %d\n", w, h);

  fwrite(&bmp_header, sizeof(struct_bmp_header), 1, fp);
  fwrite(&bmp_info_header, sizeof(struct_bmp_info_header), 1, fp);

  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){

       int pixel = data[j+i*w];

//       printf("%d\t",pixel);
       int r = pixel & 0xff;
       int g = (pixel >> 8) & 0xff;
       int b = (pixel >> 16) & 0xff;
       fputc(r, fp);
       fputc(g, fp);
       fputc(b, fp);
    }
 }
 fclose(fp);

printf("Wrote Output File\n");
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

 //printf("size of bmp_header %u\n",sizeof(struct_bmp_header));
 //printf("size of bmp_info_header %u\n",sizeof(struct_bmp_info_header));

 fread(&bmp_header, sizeof(struct_bmp_header), 1, fp);
 //fseek ( fp , sizeof(bmp_header) , SEEK_SET );
 fread(&bmp_info_header, sizeof(struct_bmp_info_header), 1 , fp);

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
       //printf("%d\t",pixel);
      data[j+i*width] = pixel;
       //printf("%d\t",pixel);
    }
 }
 fclose(fp);

 return data;
}

cl_device_id getDeviceId()
{
   int err = 0;
   cl_platform_id platform_id;
   cl_device_id device_id;

   char deviceinfostr[2048];

   //Get Platform IDs
   err = clGetPlatformIDs(1, &platform_id, NULL);
   CHK_ERROR(err, "clGetPlatformIDs");

   //Get Device IDs
   err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
   CHK_ERROR(err, "clGetDeviceIds");

   //Get Device Information
   err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(deviceinfostr), &deviceinfostr, NULL);
   CHK_ERROR(err, "clGetDeviceInfo");

   printf("CL Device: %s\n", deviceinfostr);
   return device_id;
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