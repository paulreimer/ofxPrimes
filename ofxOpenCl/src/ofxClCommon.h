/*
 * ofxClCommon.h
 * openFrameworks
 *
 *
 */

#ifndef OFX_CL_COMMON_H
#define OFX_CL_COMMON_H

#include <OpenCL/OpenCL.h>

#define LOG_DEBUG true

bool checkOpenClError(cl_int clErr, char* logNotice, char* successMsg = "ok", char* failureMsg = "error", bool verbose=LOG_DEBUG);

//////////////////////////////////////////////////////////////////////////////
//! Loads a Program file and prepends the cPreamble to the code.
//!
//! @return the source string if succeeded, 0 otherwise
//! @param cFilename        program filename
//! @param cPreamble        code that is prepended to the loaded file, typically a set of #defines or a header
//! @param szFinalLength    returned length of the code string
//////////////////////////////////////////////////////////////////////////////
char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength);

//////////////////////////////////////////////////////////////////////////////
//! Gets the id of the first device from the context
//!
//! @return the id
//! @param cxGPUContext         OpenCL context
//////////////////////////////////////////////////////////////////////////////
cl_device_id oclGetFirstDev(cl_context cxGPUContext);

// Helper function to return precision delta time for 3 counters since last call based upon host high performance counter
// *********************************************************************
double shrDeltaT(int iCounterID);

//////////////////////////////////////////////////////////////////////////////
//! Gets the id of device with maximal FLOPS from the context
//!
//! @return the id 
//! @param cxGPUContext         OpenCL context
//////////////////////////////////////////////////////////////////////////////
cl_device_id oclGetMaxFlopsDev(cl_context cxGPUContext);


//////////////////////////////////////////////////////////////////////////////
//! Print info about the device
//!
//! @param iLogMode       enum LOGBOTH, LOGCONSOLE, LOGFILE
//! @param device         OpenCL id of the device
//////////////////////////////////////////////////////////////////////////////
void oclPrintDevInfo(int iLogMode, cl_device_id device);

//////////////////////////////////////////////////////////////////////////////
//! Gets the id of the nth device from the context
//!
//! @return the id or -1 when out of range
//! @param cxGPUContext         OpenCL context
//! @param device_idx            index of the device of interest
//////////////////////////////////////////////////////////////////////////////
cl_device_id oclGetDev(cl_context cxGPUContext, unsigned int nr);
	
#endif
