#ifndef OFX_CL_SCHEDULER_CPP
#define OFX_CL_SCHEDULER_CPP

#include "ofxClScheduler.h"

ofxClScheduler::ofxClScheduler() {
	// create the OpenCL context 
#ifdef GL_INTEROP
	CGLContextObj kCGLContext = CGLGetCurrentContext();              
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    
    cl_context_properties properties[] = { 
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, 
        (cl_context_properties)kCGLShareGroup, 0 
    };
	context = clCreateContext(properties, 0, 0, 0, 0, &clErr);
//	context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &clErr);
#else
	context = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &clErr);
#endif
	
	checkOpenClError(clErr, "clCreateContextFromType");

	device = getMaxFlopsDev(context);

	globalQ = clCreateCommandQueue(context, device, 0, &clErr);	
	checkOpenClError(clErr, "clCreateCommandQueue: global");
}

bool ofxClScheduler::initKernel(ofxClKernel& krnl) {
	cl_command_queue q = clCreateCommandQueue(context, device, 0, &clErr);
	if (!checkOpenClError(clErr, "clCreateCommandQueue: private")) return false;

	return krnl.setup(context, q);
	
}

void ofxClScheduler::enqueueKernel(ofxClKernel& krnl) {
	krnl.setKernelArgs();
	krnl.update();
}

ofxClScheduler::~ofxClScheduler() {
	if(globalQ)	clReleaseCommandQueue(globalQ);
	if(context)	clReleaseContext(context);
}

void ofxClScheduler::printDeviceInfo(int device_nr) {
	// get and log device info
	if (device_nr != -1)
		device = oclGetDev(context, device_nr);
	else
		device = oclGetMaxFlopsDev(context);
	
	oclPrintDevInfo(0, device);
}

//--------------------------------------------------------------
cl_device_id ofxClScheduler::getMaxFlopsDev(cl_context ctx) {
	cl_int clErr;
	
	cl_uint platform_count = 0;
	cl_platform_id* cdPlatforms;
	
	cl_uint device_count = 0;
    cl_device_id* cdDevices;
	
	clErr = clGetPlatformIDs(0, NULL, &platform_count);
	if (!checkOpenClError(clErr, "clGetPlatformIDs [count]")) return NULL;
	
	cdPlatforms = (cl_platform_id*) alloca(platform_count * sizeof(cl_platform_id));
	
	platform_count = clGetPlatformIDs(platform_count, cdPlatforms, NULL);
	if (!checkOpenClError(clErr, "clGetPlatformIDs [data]")) return NULL;
	
	// Default to first platform ID
	cl_platform_id platform_id = cdPlatforms[0];
	
	clErr = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 0, NULL, &device_count);
	if (!checkOpenClError(clErr, "clGetDeviceIDs [count]")) return NULL;
	
//	cdDevices = (cl_device_id*) alloca(device_count * sizeof(cl_device_id));
	cdDevices = (cl_device_id*) malloc(device_count * sizeof(cl_device_id));
	
	clErr = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, device_count, cdDevices, NULL);
	if (!checkOpenClError(clErr, "clGetDeviceIDs [data]")) return NULL;
	
    cl_device_id max_flops_device = cdDevices[0];
	int max_flops = 0;
	
	size_t current_device = 0;
	
    // CL_DEVICE_MAX_COMPUTE_UNITS
    cl_uint compute_units;
    clGetDeviceInfo(cdDevices[current_device], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
	
    // CL_DEVICE_MAX_CLOCK_FREQUENCY
    cl_uint clock_frequency;
    clGetDeviceInfo(cdDevices[current_device], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
    
	max_flops = compute_units * clock_frequency;
	++current_device;
	
	while( current_device < device_count )
	{
        // CL_DEVICE_MAX_COMPUTE_UNITS
        cl_uint compute_units;
        clGetDeviceInfo(cdDevices[current_device], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
		
        // CL_DEVICE_MAX_CLOCK_FREQUENCY
        cl_uint clock_frequency;
        clGetDeviceInfo(cdDevices[current_device], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
		
        int flops = compute_units * clock_frequency;
		if( flops > max_flops )
		{
			max_flops        = flops;
			max_flops_device = cdDevices[current_device];
		}
		++current_device;
	}
	
    free(cdDevices);
	
	return max_flops_device;
}

#endif
