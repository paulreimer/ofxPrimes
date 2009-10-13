#ifndef OFX_CL_KERNEL_CPP
#define OFX_CL_KERNEL_CPP

#include "ofMain.h"
#include "ofxClKernel.h"

ofxClKernel::ofxClKernel(char* _kernelName, size_t _kernelDims) {
	kernelDims = (cl_uint)_kernelDims;

	kernelName = _kernelName;
	
	kernelFile = NULL;
	context = NULL;
	commandQ = NULL;
	
	globalWorkSize[0] = 0;
	globalWorkSize[1] = 0;
	localWorkSize[0] = 0;
	localWorkSize[1] = 0;
}

bool ofxClKernel::loadFromFile(char* _kernelFile) {
	kernelFile = _kernelFile;
	const char* kernelPath = ofToDataPath(kernelFile).c_str();
	
	kernelSource = oclLoadProgSource(kernelPath,
									 "",//"// ofxClKernel: "+kernelName,
									 &kernelSourceSize);
	return checkOpenClError(clErr, "oclLoadProgSource");
}

bool ofxClKernel::loadFromSource(char* _kernelSource, size_t _kernelSourceSize) {
	kernelSource = _kernelSource;
	kernelSourceSize = _kernelSourceSize;
	return true;
}

bool ofxClKernel::setup(const cl_context& _context, const cl_command_queue& _commandQ) {
	context = _context;
	commandQ = _commandQ;
	// Create the program
	kernelProgram = clCreateProgramWithSource(context, 1,
											  (const char **)&kernelSource,
											  &kernelSourceSize,
											  &clErr);
	if (!checkOpenClError(clErr, "clCreateProgramWithSource")) return false;
	
	// Build the program
	printf("clBuildProgram... "); 
	clErr = clBuildProgram(kernelProgram, 0, NULL, NULL, NULL, NULL);
	if (clErr != CL_SUCCESS) {
		printf("error.\n");
		delete this;
		//->destroy();
	}
	printf("ok.\n");
	
	// Create kernels
	kernel = clCreateKernel(kernelProgram, kernelName, &clErr);
	if (!checkOpenClError(clErr, "Created kernel")) return false;
	
	initKernelAssets();
	initKernelArgs();
	return true;
}

void ofxClKernel::destroy() {
	if(kernel)			clReleaseKernel(kernel);  
	if(kernelProgram)	clReleaseProgram(kernelProgram);
}

/*
void ofxClKernel::setWorkSize(size_t _global[2], size_t _local[2], size_t _offset[2]) {
	globalWorkSize = &_global;
	localWorkSize = &_local;
	globalOffset = &_offset;
}
*/
bool ofxClKernel::enqueue() {
	setKernelArgs();
	
/*
	printf("Global Work Size:\t%u w x %u h = %u\nLocal Work Size:\t%u w x %u h = %u\n", 
           globalWorkSize[0], globalWorkSize[1], (globalWorkSize[0] * globalWorkSize[1]), 
           localWorkSize[0], localWorkSize[1], (localWorkSize[0] * localWorkSize[1])); 
*/
	if (localWorkSize[0] == 0 && localWorkSize[1] == 0)
		clErr = clEnqueueNDRangeKernel(commandQ, kernel, kernelDims,
									   NULL, //globalOffset (not supported)
									   globalWorkSize, NULL,
									   0, NULL, NULL);
	else
		clErr = clEnqueueNDRangeKernel(commandQ, kernel, kernelDims,
									   NULL, //globalOffset (not supported)
									   globalWorkSize, localWorkSize,
									   0, NULL, NULL);

	//checkOpenClError(clErr, "Enqueue kernel");
	
	return (clErr == CL_SUCCESS);
}

ofxClKernel::~ofxClKernel() {
	if(kernelSource)	free(kernelSource);
	this->destroy();
}
#endif
