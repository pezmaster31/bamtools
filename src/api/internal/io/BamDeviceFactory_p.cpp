// ***************************************************************************
// BamDeviceFactory_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 25 September 2011 (DB)
// ---------------------------------------------------------------------------
// Creates built-in concrete implementations of IBamIODevices
// ***************************************************************************

#include "api/internal/io/BamDeviceFactory_p.h"
#include "api/internal/io/BamFile_p.h"
#include "api/internal/io/BamPipe_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <iostream>
#include <vector>
using namespace std;

static std::vector<CreateBamIODeviceCallback> m_callBacks;

IBamIODevice* BamDeviceFactory::CreateDevice(const string& source) {

    for(int i=0; i<m_callBacks.size(); i++){
        IBamIODevice* device = m_callBacks[i](source);
        if(device) //Callback did something with this source URL
            return device;
    }
    
    // check for requested pipe
    if ( source == "-" || source == "stdin" || source == "stdout" )
        return new BamPipe;

    // otherwise assume a "normal" file
    return new BamFile(source);
}

void BamDeviceFactory::RegisterCreatorCallback(CreateBamIODeviceCallback cb)
{
    m_callBacks.push_back(cb);
}

void IBamIODevice::RegisterCreatorCallback(CreateBamIODeviceCallback cb){
  BamDeviceFactory::RegisterCreatorCallback(cb);
}
