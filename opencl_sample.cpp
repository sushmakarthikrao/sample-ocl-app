#include<iostream>
#include<OpenCL/opencl.h>

#define CHK_ERROR(err, str) ((err) ? printf("%s returned error \n", str) : printf("%s returned success\n", str));


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
 
       printf("%d\t",pixel);
       char r = pixel & 0xff;
       char g = (pixel >> 8) & 0xff;
       char b = (pixel >> 16) & 0xff;
       fputc(r, fp);
       fputc(g, fp);
       fputc(r, fp);
    }
 } 
 fclose(fp); 
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
       char r = fgetc(fp);
       char g = fgetc(fp);
       char b = fgetc(fp);
       int pixel = r | (g << 8) | (b << 16);
       data[j+i*width] = pixel;
       printf("%d\t",pixel);
    }
 } 
 fclose(fp);
 
 return data;
}





int main(int argc, char** argv)
{
   cl_mem src;
   cl_mem dst;
   cl_platform_id platform_id;
   cl_device_id device_id;
   cl_context context;
   cl_context_properties *properties = NULL;
   cl_command_queue queue;
   
   int w;
   int h;
   
//   int size = w*h*sizeof(int);

  // int* src_ptr = (int*)malloc(size);
   //int* dst_ptr = (int*)malloc(size);


   char deviceinfostr[2048];

   int err = CL_SUCCESS;
  
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

   //Create Context
   context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err); 
   CHK_ERROR(err, "clCreateContext");

   //Create Command Queue
   queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
   CHK_ERROR(err, "clCreateCommandQueue");

   //Query Capabilities

   //Create memory objects
   //src = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, size, src_ptr, &err);
   //CHK_ERROR(err, "clCreateBuffer source buffer");
   //dst = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, size, dst_ptr, &err);
   //CHK_ERROR(err, "clCreateBuffer destination buffer");

   int* data = readBmp("sample.bmp", &w, &h);
   writeBmp("out.bmp", data, w, h);

   int* dump = readBmp("out.bmp", &w, &h); 

   //Create Program, Kernel

   //Build Program

   //Set kernel arguments

   //Enqueue Kernel

   //Get profiling information

}
