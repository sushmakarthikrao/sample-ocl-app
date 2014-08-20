#include"cl_util.h"

int main(int argc, char** argv)
{
  cl_device_id device_id;
  cl_context context;
  cl_kernel kernel;

  cl_mem cl_src;
  cl_mem cl_dst;
  cl_command_queue queue;
  cl_context_properties *properties = NULL;    
  cl_event event;

  int w;
  int h;
   
  int err = CL_SUCCESS;

  device_id = getDeviceId();

  //Create Context
  context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
  CHK_ERROR(err, "clCreateContext");

   //Create Command Queue
  queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
  CHK_ERROR(err, "clCreateCommandQueue");

  //Query Capabilities - TBD
  int* src = readBmp("sample.bmp", &w, &h);
  int size = w*h*sizeof(int);

  int* dst = (int*)malloc(size);
  cl_src = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, size, src, &err);
  CHK_ERROR(err, "clCreateBuffer source buffer");
  cl_dst = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, size, dst, &err);
  CHK_ERROR(err, "clCreateBuffer destination buffer");

  kernel = getKernel(context, device_id);

   //set kernel arguments
  err = clSetKernelArg(
		kernel,
		0,
		sizeof(cl_mem),
		&cl_src);
  err |= clSetKernelArg(
		kernel,
		1,
		sizeof(cl_mem),
		&cl_dst);
  err |= clSetKernelArg(
		kernel,
		2,
		sizeof(int),
		&w);

  CHK_ERROR(err, "clSetKernelArg");
  
  const size_t global_work_size[2] = {w, h};

  //Enqueue the kernel for execution
  err = clEnqueueNDRangeKernel(
		queue, 
		kernel, 
		2,
		NULL,
                global_work_size, 
		NULL,
        	0, 
		NULL, 
		&event);
  CHK_ERROR(err, "clEnqueueNDRangeKernel");

  //Map the destination buffer back to a pointer usable on the host side
  //Its a blocking map (CL_TRUE for 3rd parameter) in order to force all enqueues in this queue to execute on the device 
  void* host_data = clEnqueueMapBuffer(queue,
				cl_dst,
				CL_TRUE,
				CL_MAP_READ,
				0,
				size,
				0,
				NULL,
				NULL,
				&err);
  CHK_ERROR(err, "clEnqueueMapBuffer");

  queryTimingInfo(event);

  //Write output to bmp file
  writeBmp("out.bmp", (int*)host_data, w, h);
}
