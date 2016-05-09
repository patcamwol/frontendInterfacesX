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

''' validateRequestVsSRI is a helper function to check that the input data stream can support
     the allocation request. True is returned upon success, otherwise 
     FRONTEND.BadParameterException is thrown.
'''
def validateRequestVsSRI(request,upstream_sri):

    # check if the upstream sample rate falls within the requested tolerable frame rate
    upstream_sr = 1/upstream_sri.xdelta
    upstream_frame_rate = upstream_sr / (request.frame_height * request.frame_width * request.channels)
    min_requested_sample_rate = request.sample_rate
    max_requested_sample_rate = request.sample_rate+request.sample_rate * request.sample_rate_tolerance/100.0

    if not validateRequest(min_requested_sample_rate,max_requested_sample_rate,upstream_frame_rate):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- input data stream cannot support fr request")

    return True

''' validateRequestVsDevice is a helper function to check that the input data stream and the
      device can support an allocation request. True is returned upon success, otherwise
      FRONTEND.BadParameterException is thrown.
'''
def validateRequestVsDevice(request, upstream_sri, max_device_frame_rate):

    # check if request can be satisfied using the available upstream data
    if not validateRequestVsSRI(request,upstream_sri):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- falls outside of input data stream")

    # check device constraints

    # check vs. device frame rate capability (ensure 0 <= request <= max device capability)
    if not validateRequest(0,max_device_sample_rate,request.fps):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- device capabilities cannot support fr request")

    return True

''' validateRequestVsDevice is a helper function to check that the analog capabilities and the
     device can support the allocation request. True is returned upon success, otherwise
     FRONTEND.BadParameterException is thrown.
'''
def validateRequestVsDevice(request, max_device_frame_rate):

    # check device constraints

    # check vs. device frame rate capability (ensure 0 <= request <= max device capability)
    if not validateRequest(0,max_device_frame_rate,request.fps):
        raise FRONTEND.BadParameterException("INVALID REQUEST -- device capabilities cannot support fr request")

    return True

def createVideoAllocation(video_type='CAPTURE',allocation_id=None,channels=0,frame_height=0,frame_width=0,
                 fps=0.0,fps_tolerance=10.0,device_control=True,returnDict=True):
    if returnDict:
        retval = {'FRONTEND::video_allocation':{'FRONTEND::video_allocation::video_type':video_type,'FRONTEND::video_allocation::allocation_id':allocation_id,
        'FRONTEND::video_allocation::channels':channels,'FRONTEND::video_allocation::frame_height':frame_height,
        'FRONTEND::video_allocation::frame_width':frame_width,'FRONTEND::video_allocation::fps':fps,
        'FRONTEND::video_allocation::fps_tolerance':fps_tolerance,'FRONTEND::video_allocation::device_control':device_control}}
        if allocation_id == None:
            retval['FRONTEND::video_allocation']['FRONTEND::video_allocation::allocation_id']=model._uuidgen()
    else:
        alloc=[]
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::video_type',value=any.to_any(video_type)))
        if allocation_id == None:
            alloc.append(CF.DataType(id='FRONTEND::video_allocation::allocation_id',value=any.to_any(model._uuidgen())))
        else:
            alloc.append(CF.DataType(id='FRONTEND::video_allocation::allocation_id',value=any.to_any(allocation_id)))
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::channels',value=any.to_any(channels)))
        alloc[-1].value._t = CORBA.TC_long
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::frame_height',value=any.to_any(frame_height)))
        alloc[-1].value._t = CORBA.TC_long
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::frame_width',value=any.to_any(frame_width)))
        alloc[-1].value._t = CORBA.TC_long
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::fps',value=any.to_any(fps)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::fps_tolerance',value=any.to_any(fps_tolerance)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::device_control',value=any.to_any(device_control)))
        retval = CF.DataType(id='FRONTEND::video_allocation',value=CORBA.Any(CF._tc_Properties,alloc))
    return retval

def createVideoGenericListenerAllocation(video_type='CAPTURE',allocation_id=None,channels=0,frame_height=0,frame_width=0,
                 fps=0.0,fps_tolerance=10.0,device_control=False,returnDict=True):
    if returnDict:
        retval = {'FRONTEND::video_allocation':{'FRONTEND::video_allocation::video_type':video_type,'FRONTEND::video_allocation::allocation_id':allocation_id,
        'FRONTEND::video_allocation::channels':channels,'FRONTEND::video_allocation::frame_height':frame_height,
        'FRONTEND::video_allocation::frame_width':frame_width,'FRONTEND::video_allocation::fps':fps,
        'FRONTEND::video_allocation::fps_tolerance':fps_tolerance,'FRONTEND::video_allocation::device_control':device_control}}
        if allocation_id == None:
            retval['FRONTEND::video_allocation']['FRONTEND::video_allocation::allocation_id']=model._uuidgen()
    else:
        alloc=[]
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::video_type',value=any.to_any(video_type)))
        if allocation_id == None:
            alloc.append(CF.DataType(id='FRONTEND::video_allocation::allocation_id',value=any.to_any(model._uuidgen())))
        else:
            alloc.append(CF.DataType(id='FRONTEND::video_allocation::allocation_id',value=any.to_any(allocation_id)))
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::channels',value=any.to_any(channels)))
        alloc[-1].value._t = CORBA.TC_long
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::frame_height',value=any.to_any(frame_height)))
        alloc[-1].value._t = CORBA.TC_long
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::frame_width',value=any.to_any(frame_width)))
        alloc[-1].value._t = CORBA.TC_long
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::fps',value=any.to_any(fps)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::fps_tolerance',value=any.to_any(fps_tolerance)))
        alloc[-1].value._t = CORBA.TC_double
        alloc.append(CF.DataType(id='FRONTEND::video_allocation::device_control',value=any.to_any(device_control)))
        retval = CF.DataType(id='FRONTEND::video_allocation',value=CORBA.Any(CF._tc_Properties,alloc))
    return retval

def createVideoListenerAllocation(existing_allocation_id,listener_allocation_id=None,returnDict=True):
    return createTunerListenerAllocation(existing_allocation_id,listener_allocation_id=None,returnDict=True)

def videoTune(video_type='CAPTURE',allocation_id=None,channels=0,frame_height=0,frame_width=0,
                 fps=0.0,fps_tolerance=10.0,device_control=False,returnDict=True):
    numVideoDevices = len(device.frontend_video_status)
    newAllocation = False
    #No video devices found on device
    if numVideoDevices == 0:
        print "No Available Video Devices"
    else:
        if numVideoDevices >= 1:
            for index, key in enumerate(device.frontend_video_status):
                id_csv = device.frontend_video_status[index].allocation_id_csv
                if allocation_id != None and allocation_id in id_csv:
                    break
                if id_csv == '':
                    if sample_rate == None or center_frequency == None:
                        print "tune(): tune did not occur, must set center_frequency and sample_rate"
                        return None
                    else:
                        alloc=createVideoAllocation(video_type,allocation_id,channels,frame_height,frame_width,
fps,fps_tolerance,device_control,returnDict)
                        alloc_results = device.allocateCapacity(alloc)
                        if alloc_results == True:
                            newAllocation = True
                            allocation_id = alloc['FRONTEND::video_allocation']['FRONTEND::video_allocation::allocation_id']
                            break

        if allocation_id == None and not newAllocation and numVideoDevices >= 1:
            print "videoTune(): All video devices (", len(device.frontend_video_status), ") have been allocated.  Specify an allocation_id to change video tuning properties"

        elif not newAllocation:
            print "videoTune(): Video tuning an existing allocation is not yet supported"

            return allocation_status.allocation_id_csv
    
    return None

def deallocateVideo(device,allocation_id=None,allVideoDevices=False):
    deallocated = False
    if len(device.frontend_video_status) == 1:
        if len(device.frontend_video_status[0].allocation_id_csv) > 0:
            allocProps = createVideoAllocation(allocation_id=device.frontend_video_status[0].allocation_id_csv.split(",")[0])
            device.deallocateCapacity(allocProps)
            deallocated = True
    else:
        if allocation_id == None and allVideoDevices == False:
            print "deallocate(): no video device deallocated because no allocation_id specified and allVideoDevices set to False"
        else:
            if allVideoDevices == True:
                for i in range(len(device.frontend_video_status)):
                    if len(device.frontend_video_status[i].allocation_id_csv) > 0:
                        allocProps = createVideoAllocation(allocation_id=device.frontend_video_status[i].allocation_id_csv.split(",")[0])
                        device.deallocateCapacity(allocProps)
                        deallocated = True
            else:
                for i in range(len(device.frontend_video_status)):
                    if allocation_id in device.frontend_video_status[i].allocation_id_csv:
                        allocProps = createVideoAllocation(allocation_id=allocation_id)
                        device.deallocateCapacity(allocProps)
                        deallocated = True
    return deallocated

class FrontendVideoDevice(Device):

    def __init__(self, devmgr, uuid, label, softwareProfile, compositeDevice, execparams):
        Device.__init__(self, devmgr, uuid, label, softwareProfile, compositeDevice, execparams)
        
        # video_allocation_ids is exclusively paired with property frontend_video_status.
        # video_allocation_ids tracks allocation ids while frontend_video_status provides video device information.
        self.video_allocation_ids = []
    
        # Provides mapping from unique allocation ID to internal video (channel) number
        self.allocation_id_to_video_device_id = {}
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
            # if it is a minimal video_allocation property (from the IDE), then add None for all the other values
            if prop.id == 'FRONTEND::video_allocation' and len(prop.value._v) == 1 and prop.value._v[0].id == 'FRONTEND::video_allocation::allocation_id':
                prop.value._v += [CF.DataType(id='FRONTEND::video_allocation::video_type',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::video_allocation::channels',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::video_allocation::frame_height',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::video_allocation::frame_width',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::video_allocation::fps',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::video_allocation::fps_tolerance',value=any.to_any(None))]
                prop.value._v += [CF.DataType(id='FRONTEND::video_allocation::device_control',value=any.to_any(None))]
            propdict[prop.id] = propdef._fromAny(prop.value)

        self._capacityLock.acquire()
        try:
            self._deallocateCapacities(propdict)
        finally:
            self._capacityLock.release()

        # Update usage state
        self._usageState = self.updateUsageState()

        self._log.debug("deallocateCapacity() -->")
        
    def createAllocationIdCsv(self, video_device_id):
        alloc_ids = []
        # ensure control allocation_id is first in list
        if self.video_allocation_ids[video_device_id].control_allocation_id:
            alloc_ids = [self.video_allocation_ids[video_device_id].control_allocation_id]
        # now add the rest
        for allocID,videoID in self.allocation_id_to_video_device_id.items():
            if videoID == video_device_id and allocID not in alloc_ids:
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
        videoDeviceAllocated = 0
        for video in self.video_allocation_ids:
            if video.control_allocation_id:
                videoDeviceAllocated+=1

        # If no video devices are allocated, device is idle
        if videoDeviceAllocated == 0:
            return CF.Device.IDLE
        # If all video devices are allocated, device is busy
        if videoDeviceAllocated == len(self.video_allocation_ids):
            return CF.Device.BUSY
        # Else, device is active
        return CF.Device.ACTIVE

    # This sets the number of entries in the frontend_video_status struct sequence property
    # as well as the video_allocation_ids vector. Call this function during initialization
    def setNumChannels(self,num,video_type='CAPTURE'):
        self.frontend_video_status = []
        for ii in range(num):
            video_status = self.frontend_video_status_struct_struct()
            video_status.video_type = video_type
            video_status.enabled = False
            self.frontend_video_status.append(video_status)
        self.video_allocation_ids = []

    """ Allocation handlers """
    def allocate_frontend_video_allocation(self, frontend_video_allocation):
        try:
            # Check allocation_id
            if not frontend_video_allocation.allocation_id:
                self._log.info("allocate_frontend_video_allocation: MISSING ALLOCATION_ID")
                raise CF.Device.InvalidCapacity("MISSING ALLOCATION_ID", frontend_video_allocation)
            
            # Check if allocation ID has already been used
            if  self.getVideoDeviceMapping(frontend_video_allocation.allocation_id) >= 0:
                self._log.info("allocate_frontend_video_allocation: ALLOCATION_ID "+frontend_video_allocation.allocation_id+" ALREADY IN USE")
                raise CF.Device.InvalidCapacity("ALLOCATION_ID "+frontend_video_allocation.allocation_id+" ALREADY IN USE", frontend_video_allocation)
            if len(self.video_allocation_ids) != len(self.frontend_video_status):
                for idx in range(len(self.frontend_video_status)-len(self.video_allocation_ids)):
                    self.video_allocation_ids.append(video_allocation_ids_struct())

            # Next, try to allocate a new video device
            try:
                for video_device_id in range(len(self.video_allocation_ids)):
                    if self.frontend_video_status[video_device_id].video_type != frontend_video_allocation.video_type:
                        self._log.debug("allocate_frontend_video_allocation: Requested video type '" + str(frontend_video_allocation.video_type) + "' does not match videoDevice[" + str(video_device_id) + "].video_type ("+str(self.frontend_video_status[video_device_id].video_type)+")")
                        continue

                    if frontend_video_allocation.device_control:
                        # device control
                        channels = self.frontend_video_status[video_device_id].channels
                        frame_height = self.frontend_video_status[video_device_id].frame_height
                        frame_width = self.frontend_video_status[video_device_id].frame_width
                        fps = self.frontend_video_status[video_device_id].fps
                        self.frontend_video_status[video_device_id].channels = frontend_video_allocation.channels
                        self.frontend_video_status[video_device_id].frame_height = frontend_video_allocation.frame_height
                        self.frontend_video_status[video_device_id].frame_width = frontend_video_allocation.frame_width
                        self.frontend_video_status[video_device_id].fps = frontend_video_allocation.fps
                        if len(self.video_allocation_ids[video_device_id].control_allocation_id)>0 or \
                           not self.videoDeviceSetTuning(frontend_video_allocation, self.frontend_video_status[video_device_id], video_device_id):
                            # either not available or didn't succeed setting tuning, try next video
                            self._log.debug("allocate_frontend_video_allocation: videoDevice["+str(video_device_id)+"] is either not available or didn't succeed while setting tuning ")
                            continue

                        if channels == self.frontend_video_status[video_device_id].channels and \
                           frame_height == self.frontend_video_status[video_device_id].frame_height and \
                           frame_width == self.frontend_video_status[video_device_id].frame_width and \
                           fps == self.frontend_video_status[video_device_id].fps:
                            self.frontend_video_status[video_device_id].channels = frontend_video_allocation.channels
                            self.frontend_video_status[video_device_id].frame_height = frontend_video_allocation.frame_height
                            self.frontend_video_status[video_device_id].frame_width = frontend_video_allocation.frame_width
                            self.frontend_video_status[video_device_id].fps = frontend_video_allocation.fps
                           
                        self.video_allocation_ids[video_device_id].control_allocation_id = frontend_video_allocation.allocation_id
                        self.allocation_id_to_video_device_id[frontend_video_allocation.allocation_id] =  video_device_id
                        self.frontend_video_status[video_device_id].allocation_id_csv = self.createAllocationIdCsv(video_device_id)
                    else:
                        # listener
                        if len(self.video_allocation_ids[video_device_id].control_allocation_id) == 0 or not listenerRequestValidation(frontend_video_allocation, video_device_id):
                            # either not allocated or can't support listener request
                            self._log.debug("allocate_frontend_video_allocation: videoDevice["+str(video_device_id)+"] is either not available or can not support listener request ")
                            continue
                        self.video_allocation_ids[video_device_id].listener_allocation_ids.append(frontend_video_allocation.allocation_id)
                        self.allocation_id_to_video_device_id[frontend_video_allocation.allocation_id] = video_device_id
                        self.frontend_video_status[video_device_id].allocation_id_csv = self.createAllocationIdCsv(video_device_id)
                        self.assignListener(frontend_video_allocation.allocation_id,self.video_allocation_ids[video_device_id].control_allocation_id)

                    # if we've reached here, we found an eligible video devic with correct parameters

                    # check tolerances
                    # only check when sample_rate was not set to don't care
                    self._log.debug(" allocate_frontend_video_allocation - FR requested: " + str(frontend_video_allocation.fps) + "  FR got: " +str(self.frontend_video_status[video_device_id].fps))
                    if  floatingPointCompare(frontend_video_allocation.fps,0)!=0 and  \
                        (floatingPointCompare(self.frontend_video_status[video_device_id].fps,frontend_video_allocation.fps)<0 or
                         floatingPointCompare(self.frontend_video_status[video_device_id].fps,frontend_video_allocation.fps+frontend_video_allocation.fps * frontend_video_allocation.fps_tolerance/100.0)>0 ):
                        eout = "allocate_frontend_video_allocation(" + str(int(video_device_id)) +"): returned fr "+str(self.frontend_video_status[video_device_id].fps)+" does not meet tolerance criteria of "+str(frontend_video_allocation.fps_tolerance)+" percent"
                        self._log.info(eout)
                        raise RuntimeError(eout)

                    if frontend_video_allocation.device_control:
                        # enable video after successful allocation
                        try:
                            self.enableVideoDevice(video_device_id,True)
                        except Exception, e:
                            self._log.info('The following error occurred on allocation:',e)
                            eout = "allocate_frontend_video_allocation: Failed to enable video device after allocation"
                            self._log.info(eout)
                            raise RuntimeError(eout)

                    self._usageState = self.updateUsageState()
                    return True
            
            except:
                # if we made it here, we failed to find an available video device
                eout = "allocate_frontend_video_allocation: NO AVAILABLE VIDEO DEVICE. Make sure that the device has an initialized frontend_video_status"
                self._log.info(eout)
                raise RuntimeError(eout)
                    
        except RuntimeError, e:
            self.deallocate_frontend_video_allocation(frontend_video_allocation)
            return False

        except AllocationAlreadyExists, e:
            # Don't call deallocateCapacity if the allocationId already exists
            #   - Would end up deallocating a valid video device/listener
            raise CF.Device.InvalidCapacity(e)
        
        except CF.Device.InvalidCapacity, e:
            raise e
        
        except FRONTEND.BadParameterException, e:
            #self.deallocateCapacity([frontend_video_allocation.getProp()])
            return False
        
        except Exception, e:
            self._log.info('The following error occurred on allocation:',e)
            #self.deallocateCapacity([frontend_video_allocation.getProp()])
            raise e

        return False

    def deallocate_frontend_video_allocation(self, frontend_video_allocation):
        # Try to remove control of the device
        video_device_id = self.getVideoDeviceMapping(frontend_video_allocation.allocation_id)
        if video_device_id < 0:
            self._log.debug("deallocate_frontend_video_allocation: ALLOCATION_ID NOT FOUND: [" + str(frontend_video_allocation.allocation_id) + "]")
            raise CF.Device.InvalidCapacity("ALLOCATION_ID NOT FOUND: [" + str(frontend_video_allocation.allocation_id) + "]",frontend_video_allocation)
        if self.video_allocation_ids[video_device_id].control_allocation_id == frontend_video_allocation.allocation_id:
            self.enableVideoDevice(video_device_id,False)
            self.removeVideoDeviceMappingByAllocationId(frontend_video_allocation.allocation_id)
            self.frontend_video_status[video_device_id].allocation_id_csv = ''
            self.removeVideoDevice(video_device_id)
        else:
            self.removeVideoDeviceMappingByAllocationId(frontend_video_allocation.allocation_id)
            self.frontend_video_status[video_device_id].allocation_id_csv = ''
        
        #self.frontend_video_status[video_device_id].allocation_id_csv = self.create_allocation_id_csv(video_device_id, frontend_video_allocation.allocation_id)
        self.frontend_video_status[video_device_id].allocation_id_csv = ''

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
            if self.getVideoDeviceMapping(frontend_listener_allocation.listener_allocation_id) >= 0:
                self._log.info("allocate_frontend_listener_allocation: LISTENER ALLOCATION ID ALREADY IN USE")
                raise CF.Device.InvalidCapacity("LISTENER ALLOCATION ID ALREADY IN USE", frontend_listener_allocation)
            
            #self.video_allocation_ids[video_device_id].lock.acquire()
            # Check if listener allocation ID has already been used
            if self.getVideoDeviceMapping(frontend_listener_allocation.listener_allocation_id) >= 0:
                self._log.info("allocate_frontend_listener_allocation: LISTENER ALLOCATION ID ALREADY IN USE: [" + str(frontend_listener_allocation.listener_allocation_id << "]"))
                raise AllocationAlreadyExists("LISTENER ALLOCATION ID ALREADY IN USE", frontend_listener_allocation)

            # Do not allocate if existing allocation ID does not exist
            video_device_id = self.getVideoDeviceMapping(frontend_listener_allocation.existing_allocation_id)
            if video_device_id < 0:
                self._log.info("allocate_frontend_listener_allocation: UNKNOWN CONTROL ALLOCATION ID: [" + str(frontend_listener_allocation.existing_allocation_id)+"]")
                raise FRONTEND.BadParameterException("UNKNOWN CONTROL ALLOCATION ID")

            # listener allocations are not permitted for playback
            if self.frontend_video_status[video_device_id].video_type == "PLAYBACK":
                eout = "allocate_frontend_listener_allocation: listener allocations are not permitted for " + str(self.frontend_video_status[video_device_id].video_type) + " video type"
                self._log.debug(eout)
                raise CF.Device.InvalidCapacity(eout, frontend_listener_allocation)

            self.video_allocation_ids[video_device_id].listener_allocation_ids.append(frontend_listener_allocation.listener_allocation_id)
            self.allocation_id_to_video_device_id[frontend_listener_allocation.listener_allocation_id] = video_device_id
            self.frontend_video_status[video_device_id].allocation_id_csv = self.createAllocationIdCsv(video_device_id)
            self.assignListener(frontend_listener_allocation.listener_allocation_id,frontend_listener_allocation.existing_allocation_id)
            return True
            #self.video_allocation_ids[video_device_id].lock.release()

                
        except RuntimeError, e:
            return False

        except AllocationAlreadyExists, e:
            # Don't call deallocateCapacity if the allocationId already exists
            #   - Would end up deallocating a valid video/listener
            raise CF.Device.InvalidCapacity(e)
        
        except CF.Device.InvalidCapacity, e:
            raise e
        
        except FRONTEND.BadParameterException, e:
            return False
        
        except Exception, e:
            self._log.info('The following error occurred on allocation:',e)
            raise e

        return False

    def removeListenerId(self, video_device_id, allocation_id):
        split_id = self.frontend_video_status[video_device_id].allocation_id_csv.split(',')
        for idx in range(len(split_id)):
            if split_id[idx] == allocation_id:
                split_id.pop(idx)
                break
        self.frontend_video_status[video_device_id].allocation_id_csv = ','.join(split_id)
        
    def deallocate_frontend_listener_allocation(self, frontend_listener_allocation):
        video_device_id = self.getVideoDeviceMapping(frontend_listener_allocation.listener_allocation_id)
        if video_device_id < 0:
            self._log.debug("ALLOCATION_ID NOT FOUND: [" + str(frontend_listener_allocation.listener_allocation_id) + "]")
            raise CF.Device.InvalidCapacity("ALLOCATION_ID NOT FOUND", frontend_listener_allocation)
        # send EOS to listener connection only
        self.removeVideoDeviceMappingByAllocationId(frontend_listener_allocation.listener_allocation_id)
        self.removeListenerId(video_device_id, frontend_listener_allocation.listener_allocation_id)

    #*****************************************************************
    #* Video Device Configurations                                   *
    #*****************************************************************

    # Configure video - gets called during allocation
    def enableVideoDevice(self, video_device_id, enable):
        prev_enabled = self.frontend_video_status[video_device_id].enabled
        # If going from disabled to enabled
        if not prev_enabled and enable:
            self.videoDeviceEnable(self.frontend_video_status[video_device_id], video_device_id)

        # If going from enabled to disabled
        if prev_enabled and not enable:
            self.videoDeviceDisable(self.frontend_video_status[video_device_id], video_device_id)

        return True

    def listenerRequestValidation(self, request, video_device_id):
        self._log.trace("listenerRequestValidation(): request " + str(request) + " ,video_device_id " + str(video_device_id))

        # ensure requested values are non-negative
        if floatingPointCompare(request.fps,0)<0 or \
            floatingPointCompare(request.fps_tolerance,0)<0:
            return False

        # ensure video frame rate meets requested tolerance
        if floatingPointCompare(request.fps,self.frontend_video_status[video_device_id].fps) > 0:
            return False

        if floatingPointCompare(request.fps,0)!=0 and floatingPointCompare((request.fps+(request.fps*request.fps_tolerance/100)),self.frontend_video_status[video_device_id].fps) < 0:
            return False

        return True

    #//////////////////////////
    #        MAPPING         //
    #//////////////////////////

    def getVideoDeviceMapping(self, _allocation_id):
        NO_VALID_VIDEO_DEVICE = -1
        for key in self.allocation_id_to_video_device_id:
            if key == _allocation_id:
                return self.allocation_id_to_video_device_id[_allocation_id]
        return NO_VALID_VIDEO_DEVICE

    def removeVideoDeviceMappingByAllocationId(self, allocation_id):
        self._log.trace("removeVideoDeviceMapping(allocation_id) allocation_id " + str(allocation_id))
        self.removeListener(allocation_id)
        self.allocation_id_mapping_lock.acquire()
        try:
            if allocation_id in self.allocation_id_to_video_device_id:
                del self.allocation_id_to_video_device_id[allocation_id]
                return True
            return False
        finally:
            self.allocation_id_mapping_lock.release()
    
    def removeVideoDeviceMappingByVideoDeviceId(self, video_device_id):
        self._log.trace("removeVideoDeviceMapping(video_device_id) video_device_id " + str(video_device_id))
        videoDeviceDeleteTuning(self.frontend_video_status[video_device_id], video_device_id)
        removeAllocationIdRouting(video_device_id)

        self.allocation_id_mapping_lock.acquire()
        try:
            cnt = 0
            for k,v in self.allocation_id_to_video_device_id.items():
                if v == video_device_id:
                    del self.allocation_id_to_video_device_id[k]
                    cnt+=1
            return cnt > 0
        finally:
            self.allocation_id_mapping_lock.release()

    def removeVideoDevice(self, video_device_id):
        self.enableVideoDevice(video_device_id, False)
        self.video_allocation_ids[video_device_id].reset()
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

    def configureVideoSRI(self, sri, channels, frame_height, frame_width, fps):
        if sri == None:
            return

        #Create new streamID
        streamID = "video_freq_" + str(fps) + "_FPS_" + str(uuid.uuid4())
        
        sri.mode = mode
        sri.hversion = 0
        sri.xstart = 0.0
        sri.xdelta = 1.0 / (sample_rate / (frame_height * frame_width * channels))
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
                                  defvalue="FRONTEND::VIDEO",
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
    frontend_video_allocation = struct_property(id_="FRONTEND::video_allocation",
                                                name="frontend_video_allocation",
                                                structdef=frontend_video_allocation,
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
    frontend_video_status = structseq_property(id_="FRONTEND::video_status",
                                               name="frontend_video_status",
                                               structdef=default_frontend_video_status_struct_struct,
                                               defvalue=[],
                                               configurationkind=("configure",),
                                               mode="readonly",
                                               description="""Frontend Interfaces v2.0 status structure. One element for every frontend resource (receiver, transmitter) configured on this hardware"""
                                               )


    
