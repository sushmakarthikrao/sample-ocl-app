opencl_sample: opencl_sample.cpp cl_util.cpp
	g++ -o opencl_sample opencl_sample.cpp cl_util.cpp -framework OpenCL
