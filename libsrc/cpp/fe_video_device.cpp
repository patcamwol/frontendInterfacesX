/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of Frontend Interfaces X.
 *
 * Frontend Interfaces X is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Frontend Interfaces X is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
#include "fe_video_device.h"
#include <exception>
#include <frontend/fe_tuner_device.h>

namespace frontendX {
    template < typename VideoStatusStructType > 
      PREPARE_ALT_LOGGING(FrontendVideoDevice<VideoStatusStructType>, FrontendVideoDevice );


    /* frontend::validateRequestVsSRI is a helper function to check that the input data stream can support
     * the allocation request. True is returned upon success, otherwise 
     * FRONTEND::BadParameterException is thrown.
     */
    bool validateRequestVsSRI(const frontend_video_allocation_struct& request, const BULKIO::StreamSRI& upstream_sri){

        // Check if the upstream sample rate falls within the requested tolerable frame rate
        double upstream_sample_rate = 1.0/upstream_sri.xdelta;
        double upstream_frame_rate = upstream_sample_rate / (request.frame_height * request.frame_width * request.channels);
        double min_requested_frame_rate = request.fps;
        double max_requested_frame_rate = request.fps+request.fps * request.fps_tolerance/100.0;

        // check vs. upstream frame rate (ensure min request <= upstream <= max request)
        if ( !frontend::validateRequest(min_requested_frame_rate, max_requested_frame_rate, upstream_frame_rate) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- upstream fr does not agree with fr request");
        }

        return true;
    }

    /* frontend::validateRequestVsDevice is a helper function to check that the input data stream and the
     * device can support an allocation request. True is returned upon success, otherwise 
     * FRONTEND::BadParameterException is thrown.
     */
    bool validateRequestVsDevice(const frontend_video_allocation_struct& request, const BULKIO::StreamSRI& upstream_sri, double max_device_frame_rate){

        // check if request can be satisfied using the available upstream data
        if( !validateRequestVsSRI(request,upstream_sri) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- falls outside of input data stream");
        }

        // check device constraints

        // check vs. device frame rate capability (ensure 0 <= request <= max device capability)
        if ( !frontend::validateRequest(0,max_device_frame_rate,request.fps) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- device capabilities cannot support fr request");
        }

        return true;
    }

    /* frontend::validateRequestVsDevice is a helper function to check that the analog capabilities and the
     * device can support the allocation request. True is returned upon success, otherwise 
     * FRONTEND::BadParameterException is thrown.
     */
    bool validateRequestVsDevice(const frontend_video_allocation_struct& request, double max_device_frame_rate){

        // check device constraints

        // check vs. device frame rate capability (ensure 0 <= request <= max device capability)
        if ( !frontend::validateRequest(0,max_device_frame_rate,request.fps) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- device capabilities cannot support fr request");
        }

        return true;
    }

    template < typename VideoStatusStructType >
    FrontendVideoDevice<VideoStatusStructType>::FrontendVideoDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl)
    {
        construct();
    }

    template < typename VideoStatusStructType >
    FrontendVideoDevice<VideoStatusStructType>::FrontendVideoDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl, compDev)
    {
        construct();
    }

    template < typename VideoStatusStructType >
    FrontendVideoDevice<VideoStatusStructType>::FrontendVideoDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities)
    {
        construct();
    }

    template < typename VideoStatusStructType >
    FrontendVideoDevice<VideoStatusStructType>::FrontendVideoDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities, compDev)
    {
        construct();
    }

    template < typename VideoStatusStructType >
    void FrontendVideoDevice<VideoStatusStructType>::construct()
    {
        Resource_impl::_started = false;
        loadProperties();
    }

    template < typename VideoStatusStructType >
    FrontendVideoDevice<VideoStatusStructType>::~FrontendVideoDevice()
    {
        video_allocation_ids.clear();
    }

    /*******************************************************************************************
        Framework-level functions
        These functions are generally called by the framework to perform housekeeping.
    *******************************************************************************************/

    template < typename VideoStatusStructType >
    void FrontendVideoDevice<VideoStatusStructType>::loadProperties()
    {
        addProperty(device_kind,
                    "FRONTEND::VIDEO",
                    "DCE:cdc5ee18-7ceb-4ae6-bf4c-31f983179b4d",
                    "device_kind",
                    "readonly",
                    "",
                    "eq",
                    "allocation,configure");

        addProperty(device_model,
                    "",
                    "DCE:0f99b2e4-9903-4631-9846-ff349d18ecfb",
                    "device_model",
                    "readonly",
                    "",
                    "eq",
                    "allocation,configure");

        addProperty(frontend_video_allocation,
                    frontendX::frontend_video_allocation_struct(),
                    "FRONTEND::video_allocation",
                    "frontend_video_allocation",
                    "readwrite",
                    "",
                    "external",
                    "allocation");

        addProperty(frontend_listener_allocation,
                    frontend::frontend_listener_allocation_struct(),
                    "FRONTEND::listener_allocation",
                    "frontend_listener_allocation",
                    "readwrite",
                    "",
                    "external",
                    "allocation");

        addProperty(frontend_video_status,
                    "FRONTEND::video_status",
                    "frontend_video_status",
                    "readonly",
                    "",
                    "external",
                    "configure");

    }

    template < typename VideoStatusStructType >
    std::string FrontendVideoDevice<VideoStatusStructType>::createAllocationIdCsv(size_t video_device_id){
        //LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        std::string alloc_id_csv = "";
        // ensure control allocation_id is first in list
        if (!video_allocation_ids[video_device_id].control_allocation_id.empty())
            alloc_id_csv = video_allocation_ids[video_device_id].control_allocation_id + ",";
        std::vector<std::string>::iterator it = video_allocation_ids[video_device_id].listener_allocation_ids.begin();
        for(; it != video_allocation_ids[video_device_id].listener_allocation_ids.end(); it++)
            alloc_id_csv += *it + ",";
        if(!alloc_id_csv.empty())
            alloc_id_csv.erase(alloc_id_csv.size()-1);
        return alloc_id_csv;
    }

    template < typename VideoStatusStructType >
    std::string FrontendVideoDevice<VideoStatusStructType>::getControlAllocationId(size_t video_device_id){
        return video_allocation_ids[video_device_id].control_allocation_id;
    }
    template < typename VideoStatusStructType >
    std::vector<std::string> FrontendVideoDevice<VideoStatusStructType>::getListenerAllocationIds(size_t video_device_id){
        return video_allocation_ids[video_device_id].listener_allocation_ids;
    }

    /*****************************************************************/
    /* Allocation/Deallocation of Capacity                           */
    /*****************************************************************/
    template < typename VideoStatusStructType >
    CF::Device::UsageType FrontendVideoDevice<VideoStatusStructType>::updateUsageState() {
        //LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        size_t videoAllocated = 0;
        for (size_t video_device_id = 0; video_device_id < video_allocation_ids.size(); video_device_id++) {
            if (!video_allocation_ids[video_device_id].control_allocation_id.empty())
                videoAllocated++;
        }
        // If no video devices are allocated, device is idle
        if (videoAllocated == 0)
            return CF::Device::IDLE;
        // If all video devices are allocated, device is busy
        if (videoAllocated == video_allocation_ids.size())
            return CF::Device::BUSY;
        // Else, device is active
        return CF::Device::ACTIVE;
    }

    template < typename VideoStatusStructType >
    CORBA::Boolean FrontendVideoDevice<VideoStatusStructType>::allocateCapacity(const CF::Properties & capacities)
    throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState) {
        if (this->video_allocation_ids.size() != this->frontend_video_status.size()) {
            this->video_allocation_ids.resize(this->frontend_video_status.size());
        }
        LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        CORBA::ULong ii;
        try{
            for (ii = 0; ii < capacities.length(); ++ii) {
                const std::string id = (const char*) capacities[ii].id;
                if (id != "FRONTEND::video_allocation" && id != "FRONTEND::listener_allocation"){
                    LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, "UNKNOWN ALLOCATION PROPERTY1");
                    throw CF::Device::InvalidCapacity("UNKNOWN ALLOCATION PROPERTY1", capacities);
                }
                PropertyInterface* property = getPropertyFromId(id);
                if(!property){
                    LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, "UNKNOWN PROPERTY");
                    throw CF::Device::InvalidCapacity("UNKNOWN PROPERTY", capacities);
                }
                try{
                    property->setValue(capacities[ii].value);
                }
                catch(const std::logic_error &e){
                    LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, "COULD NOT PARSE CAPACITY: " << e.what());
                    throw CF::Device::InvalidCapacity("COULD NOT PARSE CAPACITY", capacities);
                };
                if (id == "FRONTEND::video_allocation"){
                    // Check allocation_id
                    if (frontend_video_allocation.allocation_id.empty()) {
                        LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"allocateCapacity: MISSING ALLOCATION_ID");
                        throw CF::Device::InvalidCapacity("MISSING ALLOCATION_ID", capacities);
                    }
                    // Check if allocation ID has already been used
                    if(getVideoDeviceMapping(frontend_video_allocation.allocation_id) >= 0){
                        LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"allocateCapacity: ALLOCATION_ID ALREADY IN USE: [" << frontend_video_allocation.allocation_id << "]");
                        throw frontend::AllocationAlreadyExists("ALLOCATION_ID ALREADY IN USE", capacities);
                    }

                    // Check if available video device
                    exclusive_lock lock(allocation_id_mapping_lock);

                    // Next, try to allocate a new video device
                    for (size_t video_device_id = 0; video_device_id < video_allocation_ids.size(); video_device_id++) {
                        if(frontend_video_status[video_device_id].video_type != frontend_video_allocation.video_type) {
                            LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,
                              "allocateCapacity: Requested video type '"<<frontend_video_allocation.video_type <<"' does not match video[" << video_device_id << "].video_type ("<<frontend_video_status[video_device_id].video_type<<")");
                            continue;
                        }

                        if(frontend_video_allocation.device_control){
                            CORBA::Long orig_chan = frontend_video_status[video_device_id].channels;
                            CORBA::Long orig_fh = frontend_video_status[video_device_id].frame_height;
                            double orig_fr = frontend_video_status[video_device_id].fps;
                            CORBA::Long orig_fw = frontend_video_status[video_device_id].frame_width;
                            // pre-load frontend_video_status values (just in case the request is filled but the values are not populated)
                            frontend_video_status[video_device_id].orig_chan = frontend_video_allocation.channels;
                            frontend_video_status[video_device_id].frame_height = frontend_video_allocation.frame_height;
                            frontend_video_status[video_device_id].fps = frontend_video_allocation.fps;
                            frontend_video_status[video_device_id].frame_width = frontend_video_allocation.frame_width;
                            // device control
                            if(!video_allocation_ids[video_device_id].control_allocation_id.empty() || !videoDeviceSetTuning(frontend_video_allocation, frontend_video_status[video_device_id], video_device_id)){
                                if (frontend_video_status[video_device_id].channels == frontend_video_allocation.channels)
                                    frontend_video_status[video_device_id].channels = orig_chan;
                                if (frontend_video_status[video_device_id].frame_height == frontend_video_allocation.frame_height)
                                    frontend_video_status[video_device_id].frame_height = orig_fh;
                                if (frontend_video_status[video_device_id].fps == frontend_video_allocation.fps)
                                    frontend_video_status[video_device_id].fps = orig_fr;
                                if (frontend_video_status[video_device_id].frame_width == frontend_video_allocation.frame_width)
                                    frontend_video_status[video_device_id].frame_width = orig_fw;
                                // either not available or didn't succeed setting tuning, try next video
                                LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,
                                    "allocateCapacity: videoDevice["<<video_device_id<<"] is either not available or didn't succeed while setting tuning ");
                                continue;
                            }
                            video_allocation_ids[video_device_id].control_allocation_id = frontend_video_allocation.allocation_id;
                            allocation_id_to_video_device_id.insert(std::pair<std::string, size_t > (frontend_video_allocation.allocation_id, video_device_id));
                            frontend_video_status[video_device_id].allocation_id_csv = createAllocationIdCsv(video_device_id);
                        } else {
                            // listener
                            if(video_allocation_ids[video_device_id].control_allocation_id.empty() || !listenerRequestValidation(frontend_video_allocation, video_device_id)){
                                // either not allocated or can't support listener request
                                LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,
                                    "allocateCapacity: videoDevice["<<video_device_id<<"] is either not available or can not support listener request ");
                                continue;
                            }
                            video_allocation_ids[video_device_id].listener_allocation_ids.push_back(frontend_video_allocation.allocation_id);
                            allocation_id_to_video_device_id.insert(std::pair<std::string, size_t > (frontend_video_allocation.allocation_id, video_device_id));
                            frontend_video_status[video_device_id].allocation_id_csv = createAllocationIdCsv(video_device_id);
                            this->assignListener(frontend_video_allocation.allocation_id,video_allocation_ids[video_device_id].control_allocation_id);
                        }
                        // if we've reached here, we found an eligible video device

                        // check tolerances
                        // only check when fps was not set to don't care
                        LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, std::fixed << " allocateCapacity - FR requested: " << frontend_video_allocation.fps
                                                                                                           << "  FR got: " << frontend_video_status[video_device_id].fps);
                        if( (frontend::floatingPointCompare(frontend_video_allocation.fps,0)!=0) &&
                            (frontend::floatingPointCompare(frontend_video_status[video_device_id].fps,frontend_video_allocation.fps)<0 ||
                            frontend::floatingPointCompare(frontend_video_status[video_device_id].fps,frontend_video_allocation.fps+frontend_video_allocation.fps * frontend_video_allocation.fps_tolerance/100.0)>0 )){
                            std::ostringstream eout;
                            eout<<std::fixed<<"allocateCapacity("<<int(video_device_id)<<"): returned fr "<<frontend_video_status[video_device_id].fps<<" does not meet tolerance criteria of "<<frontend_video_allocation.fps_tolerance<<" percent";
                            LOG_INFO(FrontendVideoDevice<VideoStatusStructType>, eout.str());
                            throw std::logic_error(eout.str().c_str());
                        }

                        if(frontend_video_allocation.device_control){
                            // enable video after successful allocation
                            try {
                                enableVideoDevice(video_device_id,true);
                            } catch(...){
                                std::ostringstream eout;
                                eout<<"allocateCapacity: Failed to enable video device after allocation";
                                LOG_INFO(FrontendVideoDevice<VideoStatusStructType>, eout.str());
                                throw std::logic_error(eout.str().c_str());
                            }
                        }
                        _usageState = updateUsageState();
                        return true;
                    }
                    // if we made it here, we failed to find an available video device
                    std::ostringstream eout;
                    eout<<"allocateCapacity: NO AVAILABLE VIDEO DEVICE. Make sure that the device has an initialized frontend_video_status";
                    LOG_INFO(FrontendVideoDevice<VideoStatusStructType>, eout.str());
                    throw std::logic_error(eout.str().c_str());
                    
                } else if (id == "FRONTEND::listener_allocation") {
                    // Check validity of allocation_id's
                    if (frontend_listener_allocation.existing_allocation_id.empty()){
                        LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"allocateCapacity: MISSING EXISTING ALLOCATION ID");
                        throw CF::Device::InvalidCapacity("MISSING EXISTING ALLOCATION ID", capacities);
                    }
                    if (frontend_listener_allocation.listener_allocation_id.empty()){
                        LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"allocateCapacity: MISSING LISTENER ALLOCATION ID");
                        throw CF::Device::InvalidCapacity("MISSING LISTENER ALLOCATION ID", capacities);
                    }

                    exclusive_lock lock(allocation_id_mapping_lock);

                    // Check if listener allocation ID has already been used
                    if(getVideoDeviceMapping(frontend_listener_allocation.listener_allocation_id) >= 0){
                        LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"allocateCapacity: LISTENER ALLOCATION ID ALREADY IN USE: [" << frontend_listener_allocation.listener_allocation_id << "]");
                        throw frontend::AllocationAlreadyExists("LISTENER ALLOCATION ID ALREADY IN USE", capacities);
                    }
                    // Do not allocate if existing allocation ID does not exist
                    long video_device_id = getVideoDeviceMapping(frontend_listener_allocation.existing_allocation_id);
                    if (video_device_id < 0){
                        LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"allocateCapacity: UNKNOWN CONTROL ALLOCATION ID: ["<< frontend_listener_allocation.existing_allocation_id <<"]");
                        throw FRONTEND::BadParameterException("UNKNOWN CONTROL ALLOCATION ID");
                    }

                    // listener allocations are not permitted for playback
                    if(frontend_video_status[video_device_id].video_type == "PLAYBACK"){
                        std::ostringstream eout;
                        eout<<"allocateCapacity: listener allocations are not permitted for " << std::string(frontend_video_status[video_device_id].video_type) << " video type";
                        LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, eout.str());
                        throw CF::Device::InvalidCapacity(eout.str().c_str(), capacities);
                    }

                    video_allocation_ids[video_device_id].listener_allocation_ids.push_back(frontend_listener_allocation.listener_allocation_id);
                    allocation_id_to_video_device_id.insert(std::pair<std::string, size_t > (frontend_listener_allocation.listener_allocation_id, video_device_id));
                    frontend_video_status[video_device_id].allocation_id_csv = createAllocationIdCsv(video_device_id);
                    this->assignListener(frontend_listener_allocation.listener_allocation_id,frontend_listener_allocation.existing_allocation_id);
                    return true;
                }
                else {
                    LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"allocateCapacity: UNKNOWN ALLOCATION PROPERTY2");
                    throw CF::Device::InvalidCapacity("UNKNOWN ALLOCATION PROPERTY2", capacities);
                }
            }
        }
        catch(const std::logic_error &e) {
            deallocateCapacity(capacities);
            return false;
        }
        catch(frontend::AllocationAlreadyExists &e) {
            // Don't call deallocateCapacity if the allocationId already exists
            //   - Would end up deallocating a valid video/listener
            throw static_cast<CF::Device::InvalidCapacity>(e); 
        }
        catch(CF::Device::InvalidCapacity &e) {
            deallocateCapacity(capacities);
            throw e;
        }
        catch(FRONTEND::BadParameterException &e) {
            deallocateCapacity(capacities);
            return false;
        }
        catch(...){
            deallocateCapacity(capacities);
            throw;
        };
        
        return true;
    }

    template < typename VideoStatusStructType >
    void FrontendVideoDevice<VideoStatusStructType>::deallocateCapacity(const CF::Properties & capacities)
    throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState) {
        LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        for (CORBA::ULong ii = 0; ii < capacities.length(); ++ii) {
            try{
                const std::string id = (const char*) capacities[ii].id;
                if (id != "FRONTEND::video_allocation" && id != "FRONTEND::listener_allocation"){
                    LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"deallocateCapacity: UNKNOWN ALLOCATION PROPERTY");
                    throw CF::Device::InvalidCapacity("UNKNOWN ALLOCATION PROPERTY", capacities);
                }
                PropertyInterface* property = getPropertyFromId(id);
                if(!property){
                    LOG_INFO(FrontendVideoDevice<VideoStatusStructType>,"deallocateCapacity: UNKNOWN PROPERTY");
                    throw CF::Device::InvalidCapacity("UNKNOWN PROPERTY", capacities);
                }
                try{
                    property->setValue(capacities[ii].value);
                }
                catch(const std::logic_error &e){
                    LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, "COULD NOT PARSE CAPACITY: " << e.what());
                    throw CF::Device::InvalidCapacity("COULD NOT PARSE CAPACITY", capacities);
                };
                if (id == "FRONTEND::video_allocation"){
                    //LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,std::string(__PRETTY_FUNCTION__)+" video_allocation");
                    // Try to remove control of the device
                    long video_device_id = getVideoDeviceMapping(frontend_video_allocation.allocation_id);
                    if (video_device_id < 0){
                        LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, "ALLOCATION_ID NOT FOUND: [" << frontend_video_allocation.allocation_id <<"]");
                        throw CF::Device::InvalidCapacity("ALLOCATION_ID NOT FOUND", capacities);
                    }
                    //LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,std::string(__PRETTY_FUNCTION__)+" video_device_id = " << video_device_id);
                    if(video_allocation_ids[video_device_id].control_allocation_id == frontend_video_allocation.allocation_id){
                        //LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,std::string(__PRETTY_FUNCTION__)+" deallocating control for video_device_id = " << video_device_id);
                        enableVideoDevice(video_device_id, false);
                        removeVideoDeviceMapping(video_device_id);
                        frontend_video_status[video_device_id].allocation_id_csv = createAllocationIdCsv(video_device_id);
                    }
                    else {
                        //LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,std::string(__PRETTY_FUNCTION__)+" deallocating listener for video_device_id = " << video_device_id);
                        // send EOS to listener connection only
                        removeVideoDeviceMapping(video_device_id,frontend_video_allocation.allocation_id);
                        frontend_video_status[video_device_id].allocation_id_csv = createAllocationIdCsv(video_device_id);
                    }
                }
                else if (id == "FRONTEND::listener_allocation") {
                    //LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,std::string(__PRETTY_FUNCTION__)+" listener_allocation");
                    long video_device_id = getVideoDeviceMapping(frontend_listener_allocation.listener_allocation_id);
                    if (video_device_id < 0){
                        LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>, "ALLOCATION_ID NOT FOUND: [" << frontend_listener_allocation.listener_allocation_id <<"]");
                        throw CF::Device::InvalidCapacity("ALLOCATION_ID NOT FOUND", capacities);
                    }
                    //LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,std::string(__PRETTY_FUNCTION__)+" video_device_id = " << video_device_id);
                    // send EOS to listener connection only
                    removeVideoDeviceMapping(video_device_id,frontend_listener_allocation.listener_allocation_id);
                    frontend_video_status[video_device_id].allocation_id_csv = createAllocationIdCsv(video_device_id);
                }
                else {
                    LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,"WARNING: UNKNOWN ALLOCATION PROPERTY \""+ std::string(property->name) + "\". IGNORING!");
                }
            }
            catch(...){
                LOG_DEBUG(FrontendVideoDevice<VideoStatusStructType>,"ERROR WHEN DEALLOCATING. SKIPPING...");
            }
        }
        _usageState = updateUsageState();
    }

    /*****************************************************************/
    /* Video Configurations                                          */
    /*****************************************************************/

    template < typename VideoStatusStructType >
    bool FrontendVideoDevice<VideoStatusStructType>::enableVideoDevice(size_t video_device_id, bool enable) {
        //LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        
        bool prev_enabled = frontend_video_status[video_device_id].enabled;
        
        // If going from disabled to enabled
        if (!prev_enabled && enable) {
            videoDeviceEnable(frontend_video_status[video_device_id], video_device_id);
        }
        
        // If going from enabled to disabled
        if (prev_enabled && !enable) {

            videoDeviceDisable(frontend_video_status[video_device_id], video_device_id);
        }

        return true;
    }

    template < typename VideoStatusStructType >
    bool FrontendVideoDevice<VideoStatusStructType>::listenerRequestValidation(frontend_video_allocation_struct &request, size_t video_device_id){
        LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);

        // ensure requested values are non-negative
        if(frontend::floatingPointCompare(request.fps,0)<0 || frontend::floatingPointCompare(request.fps_tolerance,0)<0)
            return false;

        // ensure video frame rate meets requested tolerance
        //if(request.fps > frontend_video_status[video_device_id].fps)
        if( frontend::floatingPointCompare(request.fps,frontend_video_status[video_device_id].fps) > 0 )
            return false;

        //if(request.fps != 0 && (request.fps+(request.fps*request.fps_tolerance/100)) < frontend_video_status[video_device_id].fps)
        if(frontend::floatingPointCompare(request.fps,0)!=0 && frontend::floatingPointCompare((request.fps+(request.fps*request.fps_tolerance/100)),frontend_video_status[video_device_id].fps) < 0 )
            return false;

        return true;
    };

    ////////////////////////////
    //        MAPPING         //
    ////////////////////////////

    template < typename VideoStatusStructType >
    long FrontendVideoDevice<VideoStatusStructType>::getVideoDeviceMapping(std::string allocation_id) {
        //LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        long NO_VALID_VIDEO_DEVICE = -1;

        string_number_mapping::iterator iter = allocation_id_to_video_device_id.find(allocation_id);
        if (iter != allocation_id_to_video_device_id.end())
            return iter->second;

        return NO_VALID_VIDEO_DEVICE;
    }
    
    template < typename VideoStatusStructType >
    bool FrontendVideoDevice<VideoStatusStructType>::removeVideoDeviceMapping(size_t video_device_id, std::string allocation_id) {
        LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        removeListener(allocation_id);
        std::vector<std::string>::iterator it = video_allocation_ids[video_device_id].listener_allocation_ids.begin();
        while(it != video_allocation_ids[video_device_id].listener_allocation_ids.end()){
            if(*it == allocation_id){
                video_allocation_ids[video_device_id].listener_allocation_ids.erase(it);
            } else {
                ++it;
            }
        }
        exclusive_lock lock(allocation_id_mapping_lock);
        if(allocation_id_to_video_device_id.erase(allocation_id) > 0)
            return true;
        return false;
    }

    template < typename VideoStatusStructType >
    bool FrontendVideoDevice<VideoStatusStructType>::removeVideoDeviceMapping(size_t video_device_id) {
        LOG_TRACE(FrontendVideoDevice<VideoStatusStructType>,__PRETTY_FUNCTION__);
        videoDeviceDeleteTuning(frontend_video_status[video_device_id], video_device_id);
        removeAllocationIdRouting(video_device_id);

        long cnt = 0;
        exclusive_lock lock(allocation_id_mapping_lock);
        string_number_mapping::iterator it = allocation_id_to_video_device_id.begin();
        while(it != allocation_id_to_video_device_id.end()){
            if(it->second == video_device_id){
                std::string allocation_id = it->first;
                removeListener(allocation_id);
                allocation_id_to_video_device_id.erase(it++);
                cnt++;
            } else {
                ++it;
            }
        }
        /*
        for(std::vector<std::string>::iterator it = video_allocation_ids[video_device_id].listener_allocation_ids.begin(); it != video_allocation_ids[video_device_id].listener_allocation_ids.end();it++){
            removeListener(*it);
            allocation_id_to_video_device_id.erase(*it);
            cnt++;
        }
        removeListener(video_allocation_ids[video_device_id].control_allocation_id);
        allocation_id_to_video_device_id.erase(video_allocation_ids[video_device_id].control_allocation_id);
        */
        video_allocation_ids[video_device_id].reset();
        return cnt > 0;
    }

    template < typename VideoStatusStructType >
    void FrontendVideoDevice<VideoStatusStructType>::assignListener(const std::string& listen_alloc_id, const std::string& alloc_id) {
    };

    template < typename VideoStatusStructType >
    void FrontendVideoDevice<VideoStatusStructType>::removeListener(const std::string& listen_alloc_id) {
    };

}; // end frontendX namespace
