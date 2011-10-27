/*
 * ofxClKernel.h
 * openFrameworks
 *
 * A ClKernel enqueues OpenCL kernel invocations to
 * an OpenCL context/command queue provided by
 * a ClManager, to which this kernel is attached.
 *
 * The kernel is loaded from an external .cl file
 * located in the ofDataPath.
 *
 */

#ifndef OFX_CL_KERNEL_H
#define OFX_CL_KERNEL_H

#include "ofxClCommon.h"

class ofxClKernel {
	friend class ofxClScheduler;
public:
	cl_kernel			kernel;
	cl_uint				kernelDims;
	char*				kernelName;
	char*				kernelFile;

	char*			kernelSource;
	size_t				kernelSourceSize;

	cl_program			kernelProgram;

	cl_int				clErr;

	size_t globalOffset		[2];
	size_t globalWorkSize	[2];
	size_t localWorkSize	[2];

	cl_context context;
	cl_command_queue commandQ;
	//----------------------------------------
	ofxClKernel(char* name, size_t _kernelDims);

	bool loadFromFile(char* _kernelFile);
	bool loadFromSource(char* _kernelSource, size_t kernelSourceSize);
	virtual bool setup(const cl_context& _context, const cl_command_queue& _commandQ);
	bool enqueue();

	void destroy();

	~ofxClKernel();

//	void setWorkSize(size_t* _local, size_t* _global = 0, size_t* _offset = 0);

protected:
	//----------------------------------------
	virtual void update() = 0;

	virtual void initKernelAssets() = 0;
	virtual void initKernelArgs() = 0;
	virtual void setKernelArgs() = 0;
};

#endif
