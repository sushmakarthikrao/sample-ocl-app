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
   uchar out = in.x * 0.299f + in.y * 0.587f + in.z * 0.114f;
 
   /*For Negative of the image*/
   //uchar4 maxpixel = (uchar4)(255,255,255,0);
   //uchar4 out = maxpixel - in;

   //Write out result to same location in destination image
    dst[position] = (uchar4)(out, out, out, 0);

   //dst[position] = out;
}
