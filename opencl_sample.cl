//#include<util.h>

__kernel void image_filter(__global uchar4* src,
                           __global uchar4* dst,
                           int row_width)
{
   int x = get_global_id(0);
   int y = get_global_id(1); 
  
   //My location in the image
   int position = x + y * row_width;
   
   //Read Input pixel
   uchar4 in  = src[position];

   //Convert to greyscale
   //uchar4 out = in.x * 0.299f + in.y * 0.587f + in.z * 0.114f;

   //Write out result to same location in destination image
    dst[position] = in;//out;
}