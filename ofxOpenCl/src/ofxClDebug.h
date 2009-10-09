#pragma once

#include <OpenCL/cl.h>

void DEBUG_clSetKernelArg(cl_int clErr) {
	printf("clSetKernelArg error was: ");
	switch(clErr) {
		case CL_SUCCESS:
			printf("none");
			break;
		case CL_INVALID_KERNEL:
			printf("CL_INVALID_KERNEL");
			break;
		case CL_INVALID_ARG_INDEX:
			printf("CL_INVALID_ARG_INDEX");
			break;
		case CL_INVALID_ARG_VALUE:
			printf("CL_INVALID_ARG_VALUE");
			break;
		case CL_INVALID_MEM_OBJECT:
			printf("CL_INVALID_MEM_OBJECT");
			break;
		case CL_INVALID_SAMPLER:
			printf("CL_INVALID_SAMPLER");
			break;
		case CL_INVALID_ARG_SIZE:
			printf("CL_INVALID_ARG_SIZE");
			break;
		default:
			printf("unknown");
			break;
	}
	printf(".\n");
}

void DEBUG__clEnqueueNDRangeKernel(cl_int clErr) {
	printf("clEnqueueNDRangeKernel, error was: ");
	switch(clErr) {
		case CL_SUCCESS:
			printf("none");
			break;
		case CL_INVALID_PROGRAM_EXECUTABLE:
			printf("CL_INVALID_PROGRAM_EXECUTABLE");
			break;
		case CL_INVALID_COMMAND_QUEUE:
			printf("CL_INVALID_COMMAND_QUEUE");
			break;
		case CL_INVALID_KERNEL:
			printf("CL_INVALID_KERNEL");
			break;
		case CL_INVALID_KERNEL_ARGS:
			printf("CL_INVALID_KERNEL_ARGS");
			break;
		case CL_INVALID_WORK_DIMENSION:
			printf("CL_INVALID_WORK_DIMENSION");
			break;
		case CL_INVALID_WORK_GROUP_SIZE:
			printf("CL_INVALID_WORK_GROUP_SIZE");
			break;
		case CL_INVALID_WORK_ITEM_SIZE:
			printf("CL_INVALID_WORK_ITEM_SIZE");
			break;
		case CL_INVALID_GLOBAL_OFFSET:
			printf("CL_INVALID_GLOBAL_OFFSET");
			break;
		case CL_OUT_OF_RESOURCES:
			printf("CL_OUT_OF_RESOURCES");
			break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:
			printf("CL_MEM_OBJECT_ALLOCATION_FAILURE");
			break;
		case CL_INVALID_EVENT_WAIT_LIST:
			printf("CL_INVALID_EVENT_WAIT_LIST");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			printf("CL_OUT_OF_HOST_MEMORY");
			break;
		default:
			printf("unknown");
			break;
	}
	printf(".\n");
}

void DEBUG_clCreateCommandQueue(cl_int clErr) {
	printf("clCreateCommandQueue error was: ");
	switch(clErr) {
		case CL_SUCCESS:
			printf("none");
			break;
		case CL_INVALID_CONTEXT:
			printf("CL_INVALID_CONTEXT");
			break;
		case CL_INVALID_DEVICE:
			printf("CL_INVALID_DEVICE");
			break;
		case CL_INVALID_VALUE:
			printf("CL_INVALID_VALUE");
			break;
		case CL_INVALID_QUEUE_PROPERTIES:
			printf("CL_INVALID_QUEUE_PROPERTIES");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			printf("CL_OUT_OF_HOST_MEMORY");
			break;
		default:
			printf("unknown");
			break;
	}
	printf(".\n");
}

void DEBUG_clGetContextInfo(cl_int clErr) {
	printf("clGetContextInfo error was: ");
	switch(clErr) {
		case CL_SUCCESS:
			printf("none");
			break;
		case CL_INVALID_CONTEXT:
			printf("CL_INVALID_CONTEXT");
			break;
		case CL_INVALID_VALUE:
			printf("CL_INVALID_VALUE");
			break;
		default:
			printf("unknown");
			break;
	}
	printf(".\n");
}

void DEBUG_clCreateFromGLBuffer(cl_int clErr) {
	printf("clCreateFromGLBuffer error was: ");
	switch(clErr) {
		case CL_SUCCESS:
			printf("none");
			break;
		case CL_INVALID_CONTEXT:
			printf("CL_INVALID_CONTEXT");
			break;
		case CL_INVALID_VALUE:
			printf("CL_INVALID_VALUE");
			break;
		case CL_INVALID_GL_OBJECT:
			printf("CL_INVALID_GL_OBJECT");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			printf("CL_OUT_OF_HOST_MEMORY");
			break;
		default:
			printf("unknown");
			break;
	}
	printf(".\n");
}
/*
void DEBUG_clCreateFromGLBuffer(cl_int clErr) {
	printf("clCreateFromGLBuffer error was: ");
	switch(clErr) {
		case CL_SUCCESS:
			printf("none");
			break;
		case CL_INVALID_CONTEXT:
			printf("CL_INVALID_CONTEXT");
			break;
		case CL_INVALID_VALUE:
			printf("CL_INVALID_VALUE");
			break;
		case CL_INVALID_GL_OBJECT:
			printf("CL_INVALID_GL_OBJECT");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			printf("CL_OUT_OF_HOST_MEMORY");
			break;
		default:
			printf("unknown");
			break;
	}
	printf(".\n");
}
*/