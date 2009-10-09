#pragma once

#include "ofxClCommon.h"
#include <sys/time.h>
#include <string>

//--------------------------------------------------------------
bool checkOpenClError(cl_int clErr,
					  char*	logNotice,
					  char*	successMsg,
					  char*	failureMsg,
					  bool	verbose) {
	bool wasSuccessful = (clErr == CL_SUCCESS);
	
	if (verbose)
		printf("%s... %s.\n", logNotice, (wasSuccessful? successMsg : failureMsg) );
	
	return wasSuccessful;
}

//--------------------------------------------------------------
char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength) {
    // locals
    FILE* pFileStream = NULL;
    size_t szSourceLength;
	
    // open the OpenCL source code file
#ifdef _WIN32   // Windows version
	if(fopen_s(&pFileStream, cFilename, "rb") != 0)
	{
		return NULL;
	}
#else           // Linux version
	pFileStream = fopen(cFilename, "rb");
	if(pFileStream == 0)
	{
		return NULL;
	}
#endif
	
    size_t szPreambleLength = strlen(cPreamble);
	
    // get the length of the source code
    fseek(pFileStream, 0, SEEK_END);
    szSourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET);
	
    // allocate a buffer for the source code string and read it in
    char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1);
    memcpy(cSourceString, cPreamble, szPreambleLength);
    if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
    {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }
	
    // close the file and return the total length of the combined (preamble + source) string
    fclose(pFileStream);
    if(szFinalLength != 0)
    {
        *szFinalLength = szSourceLength + szPreambleLength;
    }
    cSourceString[szSourceLength + szPreambleLength] = '\0';

    return cSourceString;
}

//--------------------------------------------------------------
cl_device_id oclGetFirstDev(cl_context cxGPUContext) {
    size_t szParmDataBytes;
    cl_device_id* cdDevices;
	
    // get the list of GPU devices associated with context
    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);
    cdDevices = (cl_device_id*) malloc(szParmDataBytes);
	
    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);
	
    cl_device_id first = cdDevices[0];
    free(cdDevices);
	
    return first;
}

//--------------------------------------------------------------
cl_device_id oclGetMaxFlopsDev(cl_context cxGPUContext) {
    size_t szParmDataBytes;
    cl_device_id* cdDevices;
	
    // get the list of GPU devices associated with context
    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);
    cdDevices = (cl_device_id*) malloc(szParmDataBytes);
    size_t device_count = szParmDataBytes / sizeof(cl_device_id);
	
    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);
	
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

//--------------------------------------------------------------
cl_device_id oclGetDev(cl_context cxGPUContext, unsigned int nr) {
    size_t szParmDataBytes;
    cl_device_id* cdDevices;
	
    // get the list of GPU devices associated with context
    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);
    
    if( szParmDataBytes / sizeof(cl_device_id) < nr ) {
		return (cl_device_id)-1;
    }
    
    cdDevices = (cl_device_id*) malloc(szParmDataBytes);
	
    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);
    
    cl_device_id device = cdDevices[nr];
    free(cdDevices);

    return device;
}


//--------------------------------------------------------------
void oclPrintDevInfo(int iLogMode, cl_device_id device) {
    char device_string[1024];
	
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
    printf(" Device %s:\n", device_string);
	
    // CL_DEVICE_INFO
    cl_device_type type;
    clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
    if( type & CL_DEVICE_TYPE_CPU )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_CPU");
    if( type & CL_DEVICE_TYPE_GPU )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_GPU");
    if( type & CL_DEVICE_TYPE_ACCELERATOR )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_ACCELERATOR");
    if( type & CL_DEVICE_TYPE_DEFAULT )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_DEFAULT");
    
    // CL_DEVICE_MAX_COMPUTE_UNITS
    cl_uint compute_units;
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
    printf("  CL_DEVICE_MAX_COMPUTE_UNITS:\t\t%d\n", compute_units);
	
    // CL_DEVICE_MAX_WORK_GROUP_SIZE
    size_t workitem_size[3];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
    printf("  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t%d / %d / %d \n",
		   (int)workitem_size[0], (int)workitem_size[1], (int)workitem_size[2]);
    
    // CL_DEVICE_MAX_WORK_GROUP_SIZE
    size_t workgroup_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
    printf("  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t%d\n", (int)workgroup_size);
	
    // CL_DEVICE_MAX_CLOCK_FREQUENCY
    cl_uint clock_frequency;
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
    printf("  CL_DEVICE_MAX_CLOCK_FREQUENCY:\t%d MHz\n", clock_frequency / 1000);
	
    // CL_DEVICE_IMAGE_SUPPORT
    cl_bool image_support;
    clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
    printf("  CL_DEVICE_IMAGE_SUPPORT:\t\t%d\n", image_support);
	
    // CL_DEVICE_GLOBAL_MEM_SIZE
    cl_ulong mem_size;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_GLOBAL_MEM_SIZE:\t\t%d MByte\n", (unsigned int)(mem_size / (1024 * 1024)));
	
    // CL_DEVICE_LOCAL_MEM_SIZE
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_LOCAL_MEM_SIZE:\t\t%d KByte\n", (unsigned int)(mem_size / (1024)));
	
    // CL_DEVICE_QUEUE_PROPERTIES
    cl_command_queue_properties queue_properties;
    clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL);
    if( queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE )
        printf("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");    
    if( queue_properties & CL_QUEUE_PROFILING_ENABLE )
        printf("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_PROFILING_ENABLE");
    
    // CL_DEVICE_EXTENSIONS: get device extensions, and if any then parse & log the string onto separate lines
    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(device_string), &device_string, NULL);
    if (device_string != 0) 
    {
        printf("  CL_DEVICE_EXTENSIONS:\n");
		std::string stdDevString;
        stdDevString = std::string(device_string);
        size_t szOldPos = 0;
        size_t szSpacePos = 0;
        while (szSpacePos != stdDevString.npos)
        {
            szSpacePos = stdDevString.find(' ', szOldPos);  // extensions string is space delimited
            printf("\t\t\t\t\t%s\n", stdDevString.substr(szOldPos, szSpacePos - szOldPos).c_str());
            szOldPos = szSpacePos + 1;
        }
        printf("\n");
    }
    else 
    {
        printf("  CL_DEVICE_EXTENSIONS: None\n");
    }
}

//--------------------------------------------------------------
double shrDeltaT(int iCounterID) {
    // local var for computation of microseconds since last call
    double DeltaT;
	
#ifdef _WIN32 // Windows version of precision host timer
	
	// Variables that need to retain state between calls
	static LARGE_INTEGER liOldCount0 = {0, 0};
	static LARGE_INTEGER liOldCount1 = {0, 0};
	static LARGE_INTEGER liOldCount2 = {0, 0};
	
	// locals for new count, new freq and new time delta
	LARGE_INTEGER liNewCount, liFreq;
	if (QueryPerformanceFrequency(&liFreq))
	{
		// Get new counter reading
		QueryPerformanceCounter(&liNewCount);
		
		// Update the requested timer
		switch (iCounterID)
		{
			case 0:
			{
				// Calculate time difference for timer 0.  (zero when called the first time)
				DeltaT = liOldCount0.LowPart ? (((double)liNewCount.QuadPart - (double)liOldCount0.QuadPart) / (double)liFreq.QuadPart) : 0.0;
				
				// Reset old count to new
				liOldCount0 = liNewCount;
				
				break;
			}
			case 1:
			{
				// Calculate time difference for timer 1.  (zero when called the first time)
				DeltaT = liOldCount1.LowPart ? (((double)liNewCount.QuadPart - (double)liOldCount1.QuadPart) / (double)liFreq.QuadPart) : 0.0;
				
				// Reset old count to new
				liOldCount1 = liNewCount;
				
				break;
			}
			case 2:
			{
				// Calculate time difference for timer 2.  (zero when called the first time)
				DeltaT = liOldCount2.LowPart ? (((double)liNewCount.QuadPart - (double)liOldCount2.QuadPart) / (double)liFreq.QuadPart) : 0.0;
				
				// Reset old count to new
				liOldCount2 = liNewCount;
				
				break;
			}
			default:
			{
				// Requested counter ID out of range
				return -9999.0;
			}
		}
		
		// Returns time difference in seconds sunce the last call
		return DeltaT;
	}
	else
	{
		// No high resolution performance counter
		return -9999.0;
	}
#else // Linux version of precision host timer. See http://www.informit.com/articles/article.aspx?p=23618&seqNum=8
	static struct timeval _NewTime;  // new wall clock time (struct representation in seconds and microseconds)
	static struct timeval _OldTime0; // old wall clock time 0(struct representation in seconds and microseconds)
	static struct timeval _OldTime1; // old wall clock time 1(struct representation in seconds and microseconds)
	static struct timeval _OldTime2; // old wall clock time 2(struct representation in seconds and microseconds)
	
	// Get new counter reading
	gettimeofday(&_NewTime, NULL);
	
	switch (iCounterID)
	{
		case 0:
		{
			// Calculate time difference for timer 0.  (zero when called the first time)
			DeltaT =  ((double)_NewTime.tv_sec + 1.0e-6 * (double)_NewTime.tv_usec) - ((double)_OldTime0.tv_sec + 1.0e-6 * (double)_OldTime0.tv_usec);
			
			// Reset old time 0 to new
			_OldTime0.tv_sec = _NewTime.tv_sec;
			_OldTime0.tv_usec = _NewTime.tv_usec;
			
			break;
		}
		case 1:
		{
			// Calculate time difference for timer 1.  (zero when called the first time)
			DeltaT =  ((double)_NewTime.tv_sec + 1.0e-6 * (double)_NewTime.tv_usec) - ((double)_OldTime1.tv_sec + 1.0e-6 * (double)_OldTime1.tv_usec);
			
			// Reset old time 1 to new
			_OldTime1.tv_sec = _NewTime.tv_sec;
			_OldTime1.tv_usec = _NewTime.tv_usec;
			
			break;
		}
		case 2:
		{
			// Calculate time difference for timer 2.  (zero when called the first time)
			DeltaT =  ((double)_NewTime.tv_sec + 1.0e-6 * (double)_NewTime.tv_usec) - ((double)_OldTime2.tv_sec + 1.0e-6 * (double)_OldTime2.tv_usec);
			
			// Reset old time 2 to new
			_OldTime2.tv_sec = _NewTime.tv_sec;
			_OldTime2.tv_usec = _NewTime.tv_usec;
			
			break;
		}
		default:
		{
			// Requested counter ID out of range
			return -9999.0;
		}
	}
	
	// Returns time difference in seconds sunce the last call
	return DeltaT;
#endif
}
