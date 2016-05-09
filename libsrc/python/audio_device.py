#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file
# distributed with this source distribution.
#
# This file is part of Frontend Interfaces X.
#
# Frontend Interfaces X is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# Frontend Interfaces X is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.
#
#
from ossie.cf import CF, CF__POA
from ossie.utils import uuid
from omniORB import any, CORBA

from ossie.device import Device
from ossie.properties import simple_property
from ossie.properties import struct_property
from ossie.properties import structseq_property
from ossie.utils import model

import threading
from bulkio.bulkioInterfaces import BULKIO
from redhawk.frontendInterfaces import FRONTEND
from fex_types import *

''' validateRequestVsSRI is a helper function to check that the input data stream and the
     device can support the allocation request. True is returned upon success, otherwise
     FRONTEND.BadParameterException is thrown.
'''
def validateRequestVsSRI(request,upstream_sri):

    # Check if the upstream sample rate falls within the requested tolerable sample rate
    upstream_sr = 1/upstream_sri.xdelta
    upstream_sr /= (request.full_bandwidth_channels + request.low_frequency_effect_channels)
    min_requested_sample_rate = request.sample_rate
    max_requested_sample_rate = request.sample_rate+request.sample_rate * request.sample_rate_tolerance/100.0

    if not validateRequest(min_requested_sample_rate,max_requested_sample_rate,upstream_sr):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- input data stream cannot support sr request")

    return True

''' validateRequestVsDevice is a helper function to check that the input data stream and the
      device can support an allocation request. True is returned upon success, otherwise 
      FRONTEND.BadParameterException is thrown.
'''
def validateRequestVsDevice(request, upstream_sri, max_device_sample_rate):

    # check if request can be satisfied using the available upstream data
    if not validateRequestVsSRI(request,upstream_sri):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- falls outside of input data stream")

    # check device constraints

    # check vs. device sample rate capability (ensure 0 <= request <= max device capability)
    if not validateRequest(0,max_device_sample_rate,request.sample_rate):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- device capabilities cannot support sr request")

    return True

''' validateRequestVsDevice is a helper function to check that the analog capabilities and the
     device can support the allocation request. True is returned upon success, otherwise 
     FRONTEND.BadParameterException is thrown.
'''
def validateRequestVsDevice(request, max_device_sample_rate):

    # check device constraints

    # check vs. device sample rate capability (ensure 0 <= request <= max device capability)
    if not validateRequest(0,max_device_sample_rate,request.sample_rate):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- device capabilities cannot support sr request")

    return True

def createAudioAllocation(audio_type='CAPTURE',allocation_id=None,full_bandwidth_channels=0,low_frequency_effect_channels=0,sample_rate=1.0,sample_rate_tolerance=10.0,
        device_control=True,returnDict=True):
    if returnDict:
        retval = {'FRONTEND::audio_allocation':{'FRONTEND::audio_allocation::audio_type':audio_type,'FRONTEND::audio_allocation::allocation_id':allocation_id,
        'FRONTEND::audio_allocation::full_bandwidth_channels':full_bandwidth_channels,'FRONTEND::audio_allocation::low_frequency_effect_channels':low_frequency_effect_channels,
        'FRONTEND::audio_allocation::sample_rate':sample_rate,'FRONTEND::audio_allocation::sample_rate_tolerance':sample_rate_tolerance,
        'FRONTEND::audio_allocation::device_control':device_control}}
        if allocation_id == None:
            retval['FRONTEND::audio_allocation']['FRONTEND::audio_allocation::allocation_id']=model._uuidgen()
    else:
        alloc=[]
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::audio_type',value=any.to_any(audio_type)))
        if allocation_id == None:
            alloc.append(CF.DataType(id='FRONTEND::audio_allocation::allocation_id',value=any.to_any(model._uuidgen())))
        else:
            alloc.append(CF.DataType(id='FRONTEND::audio_allocation::allocation_id',value=any.to_any(allocation_id)))
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::full_bandwidth_channels',value=any.to_any(full_bandwidth_channels)))
        alloc[-1].value._t = CORBA.TC_ushort
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::low_frequency_effect_channels',value=any.to_any(low_frequency_effect_channels)))
        alloc[-1].value._t = CORBA.TC_ushort
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::sample_rate',value=any.to_any(sample_rate)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::sample_rate_tolerance',value=any.to_any(sample_rate_tolerance)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::device_control',value=any.to_any(device_control)))
        retval = CF.DataType(id='FRONTEND::audio_allocation',value=CORBA.Any(CF._tc_Properties,alloc))
    return retval

def createAudioGenericListenerAllocation(audio_type='CAPTURE',allocation_id=None,full_bandwidth_channels=0,low_frequency_effect_channels=0,sample_rate=1.0,sample_rate_tolerance=10.0,
        device_control=False,returnDict=True):
    if returnDict:
        retval = {'FRONTEND::audio_allocation':{'FRONTEND::audio_allocation::audio_type':audio_type,'FRONTEND::audio_allocation::allocation_id':allocation_id,
        'FRONTEND::audio_allocation::full_bandwidth_channels':full_bandwidth_channels,'FRONTEND::audio_allocation::low_frequency_effect_channels':low_frequency_effect_channels,
        'FRONTEND::audio_allocation::sample_rate':sample_rate,'FRONTEND::audio_allocation::sample_rate_tolerance':sample_rate_tolerance,
        'FRONTEND::audio_allocation::device_control':device_control}}
        if allocation_id == None:
            retval['FRONTEND::audio_allocation']['FRONTEND::audio_allocation::allocation_id']=model._uuidgen()
    else:
        alloc=[]
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::audio_type',value=any.to_any(audio_type)))
        if allocation_id == None:
            alloc.append(CF.DataType(id='FRONTEND::audio_allocation::allocation_id',value=any.to_any(model._uuidgen())))
        else:
            alloc.append(CF.DataType(id='FRONTEND::audio_allocation::allocation_id',value=any.to_any(allocation_id)))
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::full_bandwidth_channels',value=any.to_any(full_bandwidth_channels)))
        alloc[-1].value._t = CORBA.TC_ushort
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::low_frequency_effect_channels',value=any.to_any(low_frequency_effect_channels)))
        alloc[-1].value._t = CORBA.TC_ushort
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::sample_rate',value=any.to_any(sample_rate)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::sample_rate_tolerance',value=any.to_any(sample_rate_tolerance)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::audio_allocation::device_control',value=any.to_any(device_control)))
        retval = CF.DataType(id='FRONTEND::audio_allocation',value=CORBA.Any(CF._tc_Properties,alloc))
    return retval

def createAudioListenerAllocation(existing_allocation_id,listener_allocation_id=None,returnDict=True):
    return createTunerListenerAllocation(existing_allocation_id,listener_allocation_id=None,returnDict=True)

def audioTune(audio_type='CAPTURE',allocation_id=None,full_bandwidth_channels=0,low_frequency_effect_channels=0,sample_rate=1.0,sample_rate_tolerance=10.0,
        device_control=False,returnDict=True):
    numAudioDevices = len(device.frontend_audio_status)
    newAllocation = False
    #No audio devices found on device
    if numAudioDevices == 0:
        print "No Available Audio Devices"
    else:
        if numAudioDevices >= 1:
            for index, key in enumerate(device.frontend_audio_status):
                id_csv = device.frontend_audio_status[index].allocation_id_csv
                if allocation_id != None and allocation_id in id_csv:
                    break
                if id_csv == '':
                    if sample_rate == None:
                        print "audioTune(): audio tune did not occur, must set sample_rate"
                        return None
                    else:
                        alloc=createAudioAllocation(audio_type,allocation_id,full_bandwidth_channels,
low_frequency_effect_channels,sample_rate,sample_rate_tolerance,device_control,returnDict)
                        alloc_results = device.allocateCapacity(alloc)
                        if alloc_results == True:
                            newAllocation = True
                            allocation_id = alloc['FRONTEND::audio_allocation']['FRONTEND::audio_allocation::allocation_id']
                            break

        if allocation_id == None and not newAllocation and numAudioDevices >= 1:
            print "audioTune(): All audio devices (", len(device.frontend_audio_status), ") have been allocated.  Specify an allocation_id to change audio tuning properties"

        elif not newAllocation:
            print "audioTune(): Audio tuning an existing allocation is not yet supported"

            return allocation_status.allocation_id_csv
    
    return None

def deallocateAudio(device,allocation_id=None,allAudioDevices=False):
    deallocated = False
    if len(device.frontend_audio_status) == 1:
        if len(device.frontend_audio_status[0].allocation_id_csv) > 0:
            allocProps = createAudioAllocation(allocation_id=device.frontend_audio_status[0].allocation_id_csv.split(",")[0])
            device.deallocateCapacity(allocProps)
            deallocated = True
    else:
        if allocation_id == None and allAudioDevices == False:
            print "deallocate(): no audio device deallocated because no allocation_id specified and allAudioDevices set to False"
        else:
            if allAudioDevices == True:
                for i in range(len(device.frontend_audio_status)):
                    if len(device.frontend_audio_status[i].allocation_id_csv) > 0:
                        allocProps = createAudioAllocation(allocation_id=device.frontend_audio_status[i].allocation_id_csv.split(",")[0])
                        device.deallocateCapacity(allocProps)
                        deallocated = True
            else:
                for i in range(len(device.frontend_audio_status)):
                    if allocation_id in device.frontend_audio_status[i].allocation_id_csv:
                        allocProps = createAudioAllocation(allocation_id=allocation_id)
                        device.deallocateCapacity(allocProps)
                        deallocated = True
    return deallocated

class FrontendAudioDevice(Device):

    def __init__(self, devmgr, uuid, label, softwareProfile, compositeDevice, execparams):
        Device.__init__(self, devmgr, uuid, label, softwareProfile, compositeDevice, execparams)
        
        # audio_allocation_ids is exclusively paired with property frontend_audio_status.
        # audio_allocation_ids tracks allocation ids while frontend_audio_status provides audio device information.
        self.audio_allocation_ids = []
    
        # Provides mapping from unique allocation ID to internal audio device number
        self.allocation_id_to_audio_device_id = {}
        self.allocation_id_mapping_lock = threading.Lock()

    def deallocateCapacity(self, properties):
        """
        Takes the list of properties and turns it into a dictionary.  If the 
        device has a deallocateCapacities(propDict) method it is invoked.
        
        Input:
            <properties>    A list of CF.DataType properties to allocate
            
        Output:
            None
        """
        self._log.debug("deallocateCapacity(%s)", properties)
        # Validate
        self._validateAllocProps(properties)
        # Consume
        propdict = {}
        for prop in properties:
            propdef = self._props.getPropDef(prop.id)
            # if it is a minimal audio_allocation property (from the IDE), then add None for all the other values
            if prop.id == 'FRONTEND::audio_allocation' and len(prop.value._v) == 1 and prop.value._v[0].id == 'FRONTEND::audio_allocation::allocation_id':
                prop.value._v += [CF.DataType(id='FRONTEND::audio_allocation::audio_type',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::audio_allocation::full_bandwidth_channels',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::audio_allocation::low_frequency_effect_channels',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::audio_allocation::sample_rate',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::audio_allocation::sample_rate_tolerance',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::audio_allocation::device_control',value=any.to_any(None))]
            propdict[prop.id] = propdef._fromAny(prop.value)

        self._capacityLock.acquire()
        try:
            self._deallocateCapacities(propdict)
        finally:
            self._capacityLock.release()

        # Update usage state
        self._usageState = self.updateUsageState()

        self._log.debug("deallocateCapacity() -->")
        
    def createAllocationIdCsv(self, audio_device_id):
        alloc_ids = []
        # ensure control allocation_id is first in list
        if self.audio_allocation_ids[audio_device_id].control_allocation_id:
            alloc_ids = [self.audio_allocation_ids[audio_device_id].control_allocation_id]
        # now add the rest
        for allocID,audioDeviceID in self.allocation_id_to_audio_device_id.items():
            if audioDeviceID == audio_device_id and allocID not in alloc_ids:
                alloc_ids.append(allocID)

        return ','.join(alloc_ids)

    def updateUsageState(self):
        """
        This is called automatically after allocateCapacity or deallocateCapacity are called.
        Your implementation should determine the current state of the device:
           self._usageState = CF.Device.IDLE   # not in use
           self._usageState = CF.Device.ACTIVE # in use, with capacity remaining for allocation
           self._usageState = CF.Device.BUSY   # in use, with no capacity remaining for allocation
        """
        audioDeviceAllocated = 0
        for audioDevice in self.audio_allocation_ids:
            if audioDevice.control_allocation_id:
                audioDeviceAllocated+=1

        # If no audio devices are allocated, device is idle
        if audioDeviceAllocated == 0:
            return CF.Device.IDLE
        # If all audio devices are allocated, device is busy
        if audioDeviceAllocated == len(self.audio_allocation_ids):
            return CF.Device.BUSY
        # Else, device is active
        return CF.Device.ACTIVE

    # This sets the number of entries in the frontend_audio_status struct sequence property
    # as well as the audio_allocation_ids vector. Call this function during initialization
    def setNumChannels(self,num,audio_type='CAPTURE'):
        self.frontend_audio_status = []
        for ii in range(num):
            audio_status = self.frontend_audio_status_struct_struct()
            audio_status.audio_type = audio_type
            audio_status.enabled = False
            self.frontend_audio_status.append(audio_status)
        self.audio_allocation_ids = []

    """ Allocation handlers """
    def allocate_frontend_audio_allocation(self, frontend_audio_allocation):
        try:
            # Check allocation_id
            if not frontend_audio_allocation.allocation_id:
                self._log.info("allocate_frontend_audio_allocation: MISSING ALLOCATION_ID")
                raise CF.Device.InvalidCapacity("MISSING ALLOCATION_ID", frontend_audio_allocation)
            
            # Check if allocation ID has already been used
            if  self.getAudioDeviceMapping(frontend_audio_allocation.allocation_id) >= 0:
                self._log.info("allocate_frontend_audio_allocation: ALLOCATION_ID "+frontend_audio_allocation.allocation_id+" ALREADY IN USE")
                raise CF.Device.InvalidCapacity("ALLOCATION_ID "+frontend_audio_allocation.allocation_id+" ALREADY IN USE", frontend_audio_allocation)
            if len(self.audio_allocation_ids) != len(self.frontend_audio_status):
                for idx in range(len(self.frontend_audio_status)-len(self.audio_allocation_ids)):
                    self.audio_allocation_ids.append(audio_allocation_ids_struct())

            # Next, try to allocate a new audio device
            try:
                for audio_device_id in range(len(self.audio_allocation_ids)):
                    if self.frontend_audio_status[audio_device_id].audio_type != frontend_audio_allocation.audio_type:
                        self._log.debug("allocate_frontend_audio_allocation: Requested audio type '" + str(frontend_audio_allocation.audio_type) + "' does not match audioDevice[" + str(audio_device_id) + "].audio_type ("+str(self.frontend_audio_status[audio_device_id].audio_type)+")")
                        continue

                    if frontend_audio_allocation.device_control:
                        # device control
                        full_bandwidth_channels = self.frontend_audio_status[audio_device_id].full_bandwidth_channels
                        low_frequency_effect_channels = self.frontend_audio_status[audio_device_id].low_frequency_effect_channels
                        sample_rate = self.frontend_audio_status[audio_device_id].sample_rate
                        self.frontend_audio_status[audio_device_id].full_bandwidth_channels = frontend_audio_allocation.full_bandwidth_channels
                        self.frontend_audio_status[audio_device_id].low_frequency_effect_channels = frontend_audio_allocation.low_frequency_effect_channels
                        self.frontend_audio_status[audio_device_id].sample_rate = frontend_audio_allocation.sample_rate
                        if len(self.audio_allocation_ids[audio_device_id].control_allocation_id)>0 or \
                           not self.audioDeviceSetTuning(frontend_audio_allocation, self.frontend_audio_status[audio_device_id], audio_device_id):
                            # either not available or didn't succeed setting tuning, try next audio device
                            self._log.debug("allocate_frontend_audio_allocation: audioDevice["+str(audio_device_id)+"] is either not available or didn't succeed while setting tuning ")
                            continue

                        if full_bandwidth_channels == self.frontend_audio_status[audio_device_id].full_bandwidth_channels and \
                           low_frequency_effect_channels == self.frontend_audio_status[audio_device_id].low_frequency_effect_channels and \
                           sample_rate == self.frontend_audio_status[audio_device_id].sample_rate:
                            self.frontend_audio_status[audio_device_id].full_bandwidth_channels = frontend_audio_allocation.full_bandwidth_channels
                            self.frontend_audio_status[audio_device_id].low_frequency_effect_channels = frontend_audio_allocation.low_frequency_effect_channels
                            self.frontend_audio_status[audio_device_id].sample_rate = frontend_audio_allocation.sample_rate
                           
                        self.audio_allocation_ids[audio_device_id].control_allocation_id = frontend_audio_allocation.allocation_id
                        self.allocation_id_to_audio_device_id[frontend_audio_allocation.allocation_id] =  audio_device_id
                        self.frontend_audio_status[audio_device_id].allocation_id_csv = self.createAllocationIdCsv(audio_device_id)
                    else:
                        # listener
                        if len(self.audio_allocation_ids[audio_device_id].control_allocation_id) == 0 or not listenerRequestValidation(frontend_audio_allocation, audio_device_id):
                            # either not allocated or can't support listener request
                            self._log.debug("allocate_frontend_audio_allocation: audioDevice["+str(audio_device_id)+"] is either not available or can not support listener request ")
                            continue
                        self.audio_allocation_ids[audio_device_id].listener_allocation_ids.append(frontend_audio_allocation.allocation_id)
                        self.allocation_id_to_audio_device_id[frontend_audio_allocation.allocation_id] = audio_device_id
                        self.frontend_audio_status[audio_device_id].allocation_id_csv = self.createAllocationIdCsv(audio_device_id)
                        self.assignListener(frontend_audio_allocation.allocation_id,self.audio_allocation_ids[audio_device_id].control_allocation_id)

                    # if we've reached here, we found an eligible audio device with correct parameters

                    # check tolerances
                    # only check when sample_rate was not set to don't care
                    self._log.debug(" allocate_frontend_audio_allocation - SR requested: " + str(frontend_audio_allocation.sample_rate) + "  SR got: " +str(self.frontend_audio_status[audio_device_id].sample_rate))
                    if  floatingPointCompare(frontend_audio_allocation.sample_rate,0)!=0 and  \
                        (floatingPointCompare(self.frontend_audio_status[audio_device_id].sample_rate,frontend_audio_allocation.sample_rate)<0 or
                         floatingPointCompare(self.frontend_audio_status[audio_device_id].sample_rate,frontend_audio_allocation.sample_rate+frontend_audio_allocation.sample_rate * frontend_audio_allocation.sample_rate_tolerance/100.0)>0 ):
                        eout = "allocate_frontend_audio_allocation(" + str(int(audio_device_id)) +"): returned sr "+str(self.frontend_audio_status[audio_device_id].sample_rate)+" does not meet tolerance criteria of "+str(frontend_audio_allocation.sample_rate_tolerance)+" percent"
                        self._log.info(eout)
                        raise RuntimeError(eout)

                    if frontend_audio_allocation.device_control:
                        # enable audio device after successful allocation
                        try:
                            self.enableAudioDevice(audio_device_id,True)
                        except Exception, e:
                            self._log.info('The following error occurred on allocation:',e)
                            eout = "allocate_frontend_audio_allocation: Failed to enable audio device after allocation"
                            self._log.info(eout)
                            raise RuntimeError(eout)

                    self._usageState = self.updateUsageState()
                    return True
            
            except:
                # if we made it here, we failed to find an available audio device
                eout = "allocate_frontend_audio_allocation: NO AVAILABLE AUDIO DEVICE. Make sure that the device has an initialized frontend_audio_status"
                self._log.info(eout)
                raise RuntimeError(eout)
                    
        except RuntimeError, e:
            self.deallocate_frontend_audio_allocation(frontend_audio_allocation)
            return False

        except AllocationAlreadyExists, e:
            # Don't call deallocateCapacity if the allocationId already exists
            #   - Would end up deallocating a valid audio device/listener
            raise CF.Device.InvalidCapacity(e)
        
        except CF.Device.InvalidCapacity, e:
            raise e
        
        except FRONTEND.BadParameterException, e:
            #self.deallocateCapacity([frontend_audio_allocation.getProp()])
            return False
        
        except Exception, e:
            self._log.info('The following error occurred on allocation:',e)
            #self.deallocateCapacity([frontend_audio_allocation.getProp()])
            raise e

        return False

    def deallocate_frontend_audio_allocation(self, frontend_audio_allocation):
        # Try to remove control of the device
        audio_device_id = self.getAudioDeviceMapping(frontend_audio_allocation.allocation_id)
        if audio_device_id < 0:
            self._log.debug("deallocate_frontend_audio_allocation: ALLOCATION_ID NOT FOUND: [" + str(frontend_audio_allocation.allocation_id) + "]")
            raise CF.Device.InvalidCapacity("ALLOCATION_ID NOT FOUND: [" + str(frontend_audio_allocation.allocation_id) + "]",frontend_audio_allocation)
        if self.audio_allocation_ids[audio_device_id].control_allocation_id == frontend_audio_allocation.allocation_id:
            self.enableAudioDevice(audio_device_id,False)
            self.removeAudioDeviceMappingByAllocationId(frontend_audio_allocation.allocation_id)
            self.frontend_audio_status[audio_device_id].allocation_id_csv = ''
            self.removeAudioDevice(audio_device_id)
        else:
            self.removeAudioDeviceMappingByAllocationId(frontend_audio_allocation.allocation_id)
            self.frontend_audio_status[audio_device_id].allocation_id_csv = ''
        
        #self.frontend_audio_status[audio_device_id].allocation_id_csv = self.create_allocation_id_csv(audio_device_id, frontend_audio_allocation.allocation_id)
        self.frontend_audio_status[audio_device_id].allocation_id_csv = ''

    def allocate_frontend_listener_allocation(self, frontend_listener_allocation):
        try:
            # Check validity of allocation_id's
            if not frontend_listener_allocation.existing_allocation_id:
                self._log.info("allocate_frontend_listener_allocation: MISSING EXISTING ALLOCATION ID")
                raise CF.Device.InvalidCapacity("MISSING EXISTING ALLOCATION ID", frontend_listener_allocation)
            
            if not frontend_listener_allocation.listener_allocation_id:
                self._log.info("allocate_frontend_listener_allocation: MISSING LISTENER ALLOCATION ID")
                raise CF.Device.InvalidCapacity("MISSING LISTENER ALLOCATION ID", frontend_listener_allocation)

            # Check if listener allocation ID has already been used
            if self.getAudioDeviceMapping(frontend_listener_allocation.listener_allocation_id) >= 0:
                self._log.info("allocate_frontend_listener_allocation: LISTENER ALLOCATION ID ALREADY IN USE")
                raise CF.Device.InvalidCapacity("LISTENER ALLOCATION ID ALREADY IN USE", frontend_listener_allocation)
            
            #self.audio_allocation_ids[audio_device_id].lock.acquire()
            # Check if listener allocation ID has already been used
            if self.getAudioDeviceMapping(frontend_listener_allocation.listener_allocation_id) >= 0:
                self._log.info("allocate_frontend_listener_allocation: LISTENER ALLOCATION ID ALREADY IN USE: [" + str(frontend_listener_allocation.listener_allocation_id << "]"))
                raise AllocationAlreadyExists("LISTENER ALLOCATION ID ALREADY IN USE", frontend_listener_allocation)

            # Do not allocate if existing allocation ID does not exist
            audio_device_id = self.getAudioDeviceMapping(frontend_listener_allocation.existing_allocation_id)
            if audio_device_id < 0:
                self._log.info("allocate_frontend_listener_allocation: UNKNOWN CONTROL ALLOCATION ID: [" + str(frontend_listener_allocation.existing_allocation_id)+"]")
                raise FRONTEND.BadParameterException("UNKNOWN CONTROL ALLOCATION ID")

            # listener allocations are not permitted for playback
            if self.frontend_audio_status[audio_device_id].audio_type == "PLAYBACK":
                eout = "allocate_frontend_listener_allocation: listener allocations are not permitted for " + str(self.frontend_audio_status[audio_device_id].audio_type) + " audio type"
                self._log.debug(eout)
                raise CF.Device.InvalidCapacity(eout, frontend_listener_allocation)

            self.audio_allocation_ids[audio_device_id].listener_allocation_ids.append(frontend_listener_allocation.listener_allocation_id)
            self.allocation_id_to_audio_device_id[frontend_listener_allocation.listener_allocation_id] = audio_device_id
            self.frontend_audio_status[audio_device_id].allocation_id_csv = self.createAllocationIdCsv(audio_device_id)
            self.assignListener(frontend_listener_allocation.listener_allocation_id,frontend_listener_allocation.existing_allocation_id)
            return True
            #self.audio_allocation_ids[audio_device_id].lock.release()

                
        except RuntimeError, e:
            return False

        except AllocationAlreadyExists, e:
            # Don't call deallocateCapacity if the allocationId already exists
            #   - Would end up deallocating a valid audio/listener
            raise CF.Device.InvalidCapacity(e)
        
        except CF.Device.InvalidCapacity, e:
            raise e
        
        except FRONTEND.BadParameterException, e:
            return False
        
        except Exception, e:
            self._log.info('The following error occurred on allocation:',e)
            raise e

        return False

    def removeListenerId(self, audio_device_id, allocation_id):
        split_id = self.frontend_audio_status[audio_device_id].allocation_id_csv.split(',')
        for idx in range(len(split_id)):
            if split_id[idx] == allocation_id:
                split_id.pop(idx)
                break
        self.frontend_audio_status[audio_device_id].allocation_id_csv = ','.join(split_id)
        
    def deallocate_frontend_listener_allocation(self, frontend_listener_allocation):
        audio_device_id = self.getAudioDeviceMapping(frontend_listener_allocation.listener_allocation_id)
        if audio_device_id < 0:
            self._log.debug("ALLOCATION_ID NOT FOUND: [" + str(frontend_listener_allocation.listener_allocation_id) + "]")
            raise CF.Device.InvalidCapacity("ALLOCATION_ID NOT FOUND", frontend_listener_allocation)
        # send EOS to listener connection only
        self.removeAudioDeviceMappingByAllocationId(frontend_listener_allocation.listener_allocation_id)
        self.removeListenerId(audio_device_id, frontend_listener_allocation.listener_allocation_id)

    #*****************************************************************
    #* Audio Device Configurations                                   *
    #*****************************************************************

    # Configure audio - gets called during allocation
    def enableAudioDevice(self, audio_device_id, enable):
        prev_enabled = self.frontend_audio_status[audio_device_id].enabled
        # If going from disabled to enabled
        if not prev_enabled and enable:
            self.audioDeviceEnable(self.frontend_audio_status[audio_device_id], audio_device_id)

        # If going from enabled to disabled
        if prev_enabled and not enable:
            self.audioDeviceDisable(self.frontend_audio_status[audio_device_id], audio_device_id)

        return True

    def listenerRequestValidation(self, request, audio_device_id):
        self._log.trace("listenerRequestValidation(): request " + str(request) + " ,audio_device_id " + str(audio_device_id))

        # ensure requested values are non-negative
        if floatingPointCompare(request.sample_rate,0)<0 or \
            floatingPointCompare(request.sample_rate_tolerance,0)<0:
            return False

        # ensure audio sample rate meets requested tolerance
        if floatingPointCompare(request.sample_rate,self.frontend_audio_status[audio_device_id].sample_rate) > 0:
            return False

        if floatingPointCompare(request.sample_rate,0)!=0 and floatingPointCompare((request.sample_rate+(request.sample_rate*request.sample_rate_tolerance/100)),self.frontend_audio_status[audio_device_id].sample_rate) < 0:
            return False

        return True

    #//////////////////////////
    #        MAPPING         //
    #//////////////////////////

    def getAudioDeviceMapping(self, _allocation_id):
        NO_VALID_AUDIO_DEVICE = -1
        for key in self.allocation_id_to_audio_device_id:
            if key == _allocation_id:
                return self.allocation_id_to_audio_device_id[_allocation_id]
        return NO_VALID_AUDIO_DEVICE

    def removeAudioDeviceMappingByAllocationId(self, allocation_id):
        self._log.trace("removeAudioDeviceMapping(allocation_id) allocation_id " + str(allocation_id))
        self.removeListener(allocation_id)
        self.allocation_id_mapping_lock.acquire()
        try:
            if allocation_id in self.allocation_id_to_audio_device_id:
                del self.allocation_id_to_audio_device_id[allocation_id]
                return True
            return False
        finally:
            self.allocation_id_mapping_lock.release()
    
    def removeAudioDeviceMappingByAudioDeviceId(self, audio_device_id):
        self._log.trace("removeAudioDeviceMapping(audio_device_id) audio_device_id " + str(audio_device_id))
        audioDeviceDeleteTuning(self.frontend_audio_status[audio_device_id], audio_device_id)
        removeAllocationIdRouting(audio_device_id)

        self.allocation_id_mapping_lock.acquire()
        try:
            cnt = 0
            for k,v in self.allocation_id_to_audio_device_id.items():
                if v == audio_device_id:
                    del self.allocation_id_to_audio_device_id[k]
                    cnt+=1
            return cnt > 0
        finally:
            self.allocation_id_mapping_lock.release()

    def removeAudioDevice(self, audio_device_id):
        self.enableAudioDevice(audio_device_id, False)
        self.audio_allocation_ids[audio_device_id].reset()
        return True

    def assignListener(self, listen_alloc_id, alloc_id):
        pass

    def removeListener(self, listen_alloc_id):
        pass

    ############################
    ## Other helper functions ##
    ############################

    def addModifyKeyword(self, sri, id, myValue, addOnly=False):
        if not addOnly:
            for keyword in sri.keywords:
                if keyword.id == id:
                    keyword.value = any.to_any(myValue)
                    return True
        sri.keywords.append(CF.DataType(id=id, value=any.to_any(myValue)))
        return True

    def configureAudioDeviceSRI(self, sri, full_bandwidth_channels, low_frequency_effect_channels, sample_rate):
        if sri == None:
            return

        #Create new streamID
        streamID = "audio_sample_rate_" + str(sample_rate) + "_SPS_" + str(uuid.uuid4())
        
        sri.mode = mode
        sri.hversion = 0
        sri.xstart = 0.0
        sri.xdelta = 1.0 / (sample_rate * (full_bandwidth_channels + low_frequency_effect_channels))
        sri.subsize = 0 # 1-dimensional data
        sri.xunits = 1
        sri.ystart = 0
        sri.ydelta = 0.001
        sri.yunits = 1
        sri.streamID = streamID
        sri.blocking = False

    # This is not currently used but is available as a debugging tool
    def printSRI(self, sri, strHeader = "DEBUG SRI"):
        print strHeader
        print "\thversion:",  sri.hversion
        print "\txstart:",  sri.xstart
        print "\txdelta:",  sri.xdelta
        print "\txunits:",  sri.xunits
        print "\tsubsize:",  sri.subsize
        print "\tystart:",  sri.ystart
        print "\tydelta:",  sri.ydelta
        print "\tyunits:",  sri.yunits
        print "\tmode:",  sri.mode
        print "\tstreamID:",  sri.streamID
        for keyword in sri.keywords:
            print "\t KEYWORD KEY/VAL ::",  keywords.id << ":",  any.from_any(keywords.value)
            
    ######################################################################
    # PROPERTIES
    # 
    # DO NOT ADD NEW PROPERTIES HERE.  You can add properties in your derived class, in the PRF xml file
    # or by using the IDE.
    device_kind = simple_property(id_="DCE:cdc5ee18-7ceb-4ae6-bf4c-31f983179b4d",
                                  name="device_kind",
                                  type_="string",
                                  defvalue="FRONTEND::AUDIO",
                                  mode="readonly",
                                  action="eq",
                                  kinds=("allocation","configure"),
                                  description="""This specifies the device kind"""
                                  )
    device_model = simple_property(id_="DCE:0f99b2e4-9903-4631-9846-ff349d18ecfb",
                                   name="device_model",
                                   type_="string",
                                   defvalue="",
                                   mode="readonly",
                                   action="eq",
                                   kinds=("allocation","configure"),
                                   description=""" This specifies the specific device"""
                                   )
    frontend_audio_allocation = struct_property(id_="FRONTEND::audio_allocation",
                                                name="frontend_audio_allocation",
                                                structdef=frontend_audio_allocation,
                                                configurationkind=("allocation",),
                                                mode="readwrite",
                                                description="""Frontend Interfaces v2.0 main allocation structure"""
                                                )
    frontend_listener_allocation = struct_property(id_="FRONTEND::listener_allocation",
                                                   name="frontend_listener_allocation",
                                                   structdef=frontend_listener_allocation,
                                                   configurationkind=("allocation",),
                                                   mode="readwrite",
                                                   description="""Allocates a listener (subscriber) based off a previous allocation """
                                                   )
    frontend_audio_status = structseq_property(id_="FRONTEND::audio_status",
                                               name="frontend_audio_status",
                                               structdef=default_frontend_audio_status_struct_struct,
                                               defvalue=[],
                                               configurationkind=("configure",),
                                               mode="readonly",
                                               description="""Frontend Interfaces v2.0 status structure. One element for every frontend resource (microphone, speaker) configured on this hardware"""
                                               )


    
