/*
 * ofxClManager.h
 * openFrameworks
 *
 *
 */

#ifndef OFX_CL_SCHEDULER_H
#define OFX_CL_SCHEDULER_H

#include "ofxClCommon.h"
#include "ofxClKernel.h"

class ofxClScheduler {

public:
//	vector <cl_device_id>		knownDevices;
	cl_context			context;
	cl_device_id		device;

	cl_command_queue	globalQ;
	cl_int				clErr;

	//----------------------------------------
	ofxClScheduler();
	~ofxClScheduler();

	//----------------------------------------
	bool initKernel(ofxClKernel& krnl);
	void enqueueKernel(ofxClKernel& krnl);

	void printDeviceInfo(int device_nr = -1);

	cl_device_id getMaxFlopsDev(cl_context ctx) const;

	//	cl_device_id* findAllDevices();
//	cl_device_id findDeviceById(short _id);
//	cl_device_id findDeviceByMaxFlops();
};

#endif
