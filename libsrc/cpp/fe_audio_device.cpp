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
#include "fe_audio_device.h"
#include <exception>
#include <frontend/fe_tuner_device.h>

namespace frontendX {
    template < typename AudioStatusStructType > 
      PREPARE_ALT_LOGGING(FrontendAudioDevice<AudioStatusStructType>, FrontendAudioDevice );

    /* validateRequestVsSRI is a helper function to check that the input data stream and the
     * device can support an allocation request. True is returned upon success, otherwise
     * FRONTEND::BadParameterException is thrown.
     */
    bool validateRequestVsSRI(const frontend_audio_allocation_struct& request, const BULKIO::StreamSRI& upstream_sri){
        
        // Check if the upstream sample rate falls within the requested tolerable sample rate
        double upstream_sample_rate = 1.0/upstream_sri.xdelta;
        upstream_sample_rate /= (request.full_bandwidth_channels + request.low_frequency_effect_channels);
        double min_requested_sample_rate = request.sample_rate;
        double max_requested_sample_rate = request.sample_rate+request.sample_rate * request.sample_rate_tolerance/100.0;

        // check vs. upstream sample rate (ensure min request <= upstream <= max request)
        if ( !frontend::validateRequest(min_requested_sample_rate, max_requested_sample_rate, upstream_sample_rate) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- upstream sr does not agree with sr request");
        }

        return true;
    }

    /* validateRequestVsDevice is a helper function to check that the input data stream and the
     * device can support an allocation request. True is returned upon success, otherwise 
     * FRONTEND::BadParameterException is thrown.
     */
    bool validateRequestVsDevice(const frontend_audio_allocation_struct& request, const BULKIO::StreamSRI& upstream_sri,
            double max_device_sample_rate){

        // check if request can be satisfied using the available upstream data
        if( !validateRequestVsSRI(request,upstream_sri) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- falls outside of input data stream");
        }

        // check device constraints

        // check vs. device sample rate capability (ensure 0 <= request <= max device capability)
        if ( !frontend::validateRequest(0,max_device_sample_rate,request.sample_rate) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- device capabilities cannot support sr request");
        }

        return true;
    }

    /* validateRequestVsDevice is a helper function to check that the analog capabilities and the
     * device can support the allocation request. True is returned upon success, otherwise 
     * FRONTEND::BadParameterException is thrown.
     */
    bool validateRequestVsDevice(const frontend_audio_allocation_struct& request, double max_device_sample_rate){

        // check device constraints

        // check vs. device sample rate capability (ensure 0 <= request <= max device capability)
        if ( !frontend::validateRequest(0,max_device_sample_rate,request.sample_rate) ){
            throw FRONTEND::BadParameterException("INVALID REQUEST -- device capabilities cannot support sr request");
        }

        return true;
    }

    template < typename AudioStatusStructType >
    FrontendAudioDevice<AudioStatusStructType>::FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl)
    {
        construct();
    }

    template < typename AudioStatusStructType >
    FrontendAudioDevice<AudioStatusStructType>::FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl, compDev)
    {
        construct();
    }

    template < typename AudioStatusStructType >
    FrontendAudioDevice<AudioStatusStructType>::FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities)
    {
        construct();
    }

    template < typename AudioStatusStructType >
    FrontendAudioDevice<AudioStatusStructType>::FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev) :
        Device_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities, compDev)
    {
        construct();
    }

    template < typename AudioStatusStructType >
    void FrontendAudioDevice<AudioStatusStructType>::construct()
    {
        Resource_impl::_started = false;
        loadProperties();
    }

    template < typename AudioStatusStructType >
    FrontendAudioDevice<AudioStatusStructType>::~FrontendAudioDevice()
    {
        audio_allocation_ids.clear();
    }

    /*******************************************************************************************
        Framework-level functions
        These functions are generally called by the framework to perform housekeeping.
    *******************************************************************************************/

    template < typename AudioStatusStructType >
    void FrontendAudioDevice<AudioStatusStructType>::loadProperties()
    {
        addProperty(device_kind,
                    "FRONTEND::AUDIO",
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

        addProperty(frontend_audio_allocation,
                    frontendX::frontend_audio_allocation_struct(),
                    "FRONTEND::audio_allocation",
                    "frontend_audio_allocation",
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

        addProperty(frontend_audio_status,
                    "FRONTEND::audio_status",
                    "frontend_audio_status",
                    "readonly",
                    "",
                    "external",
                    "configure");

    }

    template < typename AudioStatusStructType >
    std::string FrontendAudioDevice<AudioStatusStructType>::createAllocationIdCsv(size_t audio_device_id){
        //LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        std::string alloc_id_csv = "";
        // ensure control allocation_id is first in list
        if (!audio_allocation_ids[audio_device_id].control_allocation_id.empty())
            alloc_id_csv = audio_allocation_ids[audio_device_id].control_allocation_id + ",";
        std::vector<std::string>::iterator it = audio_allocation_ids[audio_device_id].listener_allocation_ids.begin();
        for(; it != audio_allocation_ids[audio_device_id].listener_allocation_ids.end(); it++)
            alloc_id_csv += *it + ",";
        if(!alloc_id_csv.empty())
            alloc_id_csv.erase(alloc_id_csv.size()-1);
        return alloc_id_csv;
    }

    template < typename AudioStatusStructType >
    std::string FrontendAudioDevice<AudioStatusStructType>::getControlAllocationId(size_t audio_device_id){
        return audio_allocation_ids[audio_device_id].control_allocation_id;
    }
    template < typename AudioStatusStructType >
    std::vector<std::string> FrontendAudioDevice<AudioStatusStructType>::getListenerAllocationIds(size_t audio_device_id){
        return audio_allocation_ids[audio_device_id].listener_allocation_ids;
    }

    /*****************************************************************/
    /* Allocation/Deallocation of Capacity                           */
    /*****************************************************************/
    template < typename AudioStatusStructType >
    CF::Device::UsageType FrontendAudioDevice<AudioStatusStructType>::updateUsageState() {
        //LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        size_t audioAllocated = 0;
        for (size_t audio_device_id = 0; audio_device_id < audio_allocation_ids.size(); audio_device_id++) {
            if (!audio_allocation_ids[audio_device_id].control_allocation_id.empty())
                audioAllocated++;
        }
        // If no audio devices are allocated, device is idle
        if (audioAllocated == 0)
            return CF::Device::IDLE;
        // If all audio devices are allocated, device is busy
        if (audioAllocated == audio_allocation_ids.size())
            return CF::Device::BUSY;
        // Else, device is active
        return CF::Device::ACTIVE;
    }

    template < typename AudioStatusStructType >
    CORBA::Boolean FrontendAudioDevice<AudioStatusStructType>::allocateCapacity(const CF::Properties & capacities)
    throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState) {
        if (this->audio_allocation_ids.size() != this->frontend_audio_status.size()) {
            this->audio_allocation_ids.resize(this->frontend_audio_status.size());
        }
        LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        CORBA::ULong ii;
        try{
            for (ii = 0; ii < capacities.length(); ++ii) {
                const std::string id = (const char*) capacities[ii].id;
                if (id != "FRONTEND::audio_allocation" && id != "FRONTEND::listener_allocation"){
                    LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, "UNKNOWN ALLOCATION PROPERTY1");
                    throw CF::Device::InvalidCapacity("UNKNOWN ALLOCATION PROPERTY1", capacities);
                }
                PropertyInterface* property = getPropertyFromId(id);
                if(!property){
                    LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, "UNKNOWN PROPERTY");
                    throw CF::Device::InvalidCapacity("UNKNOWN PROPERTY", capacities);
                }
                try{
                    property->setValue(capacities[ii].value);
                }
                catch(const std::logic_error &e){
                    LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, "COULD NOT PARSE CAPACITY: " << e.what());
                    throw CF::Device::InvalidCapacity("COULD NOT PARSE CAPACITY", capacities);
                };
                if (id == "FRONTEND::audio_allocation"){
                    // Check allocation_id
                    if (frontend_audio_allocation.allocation_id.empty()) {
                        LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"allocateCapacity: MISSING ALLOCATION_ID");
                        throw CF::Device::InvalidCapacity("MISSING ALLOCATION_ID", capacities);
                    }
                    // Check if allocation ID has already been used
                    if(getAudioDeviceMapping(frontend_audio_allocation.allocation_id) >= 0){
                        LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"allocateCapacity: ALLOCATION_ID ALREADY IN USE: [" << frontend_audio_allocation.allocation_id << "]");
                        throw frontend::AllocationAlreadyExists("ALLOCATION_ID ALREADY IN USE", capacities);
                    }

                    // Check if available audio device
                    exclusive_lock lock(allocation_id_mapping_lock);

                    // Next, try to allocate a new audio device
                    for (size_t audio_device_id = 0; audio_device_id < audio_allocation_ids.size(); audio_device_id++) {
                        if(frontend_audio_status[audio_device_id].audio_type != frontend_audio_allocation.audio_type) {
                            LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,
                              "allocateCapacity: Requested audio type '"<<frontend_audio_allocation.audio_type <<"' does not match audioDevice[" << audio_device_id << "].audio_type ("<<frontend_audio_status[audio_device_id].audio_type<<")");
                            continue;
                        }

                        if(frontend_audio_allocation.device_control){
                            unsigned short orig_fbc = frontend_audio_status[audio_device_id].full_bandwidth_channels;
                            unsigned short orig_lfec = frontend_audio_status[audio_device_id].low_frequency_effect_channels;
                            double orig_sr = frontend_audio_status[audio_device_id].sample_rate;
                            // pre-load frontend_audio_status values (just in case the request is filled but the values are not populated)
                            frontend_audio_status[audio_device_id].full_bandwidth_channels = frontend_audio_allocation.full_bandwidth_channels;
                            frontend_audio_status[audio_device_id].low_frequency_effect_channels = frontend_audio_allocation.low_frequency_effect_channels;
                            frontend_audio_status[audio_device_id].sample_rate = frontend_audio_allocation.sample_rate;
                            // device control
                            if(!audio_allocation_ids[audio_device_id].control_allocation_id.empty() || !audioDeviceSetTuning(frontend_audio_allocation, frontend_audio_status[audio_device_id], audio_device_id)){
                                if (frontend_audio_status[audio_device_id].full_bandwidth_channels == frontend_audio_allocation.full_bandwidth_channels)
                                    frontend_audio_status[audio_device_id].full_bandwidth_channels = orig_fbc;
                                if (frontend_audio_status[audio_device_id].low_frequency_effect_channels == frontend_audio_allocation.low_frequency_effect_channels)
                                    frontend_audio_status[audio_device_id].low_frequency_effect_channels = orig_lfec;
                                if (frontend_audio_status[audio_device_id].sample_rate == frontend_audio_allocation.sample_rate)
                                    frontend_audio_status[audio_device_id].sample_rate = orig_sr;
                                // either not available or didn't succeed setting tuning, try next audio device
                                LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,
                                    "allocateCapacity: audioDevice["<<audio_device_id<<"] is either not available or didn't succeed while setting tuning ");
                                continue;
                            }
                            audio_allocation_ids[audio_device_id].control_allocation_id = frontend_audio_allocation.allocation_id;
                            allocation_id_to_audio_device_id.insert(std::pair<std::string, size_t > (frontend_audio_allocation.allocation_id, audio_device_id));
                            frontend_audio_status[audio_device_id].allocation_id_csv = createAllocationIdCsv(audio_device_id);
                        } else {
                            // listener
                            if(audio_allocation_ids[audio_device_id].control_allocation_id.empty() || !listenerRequestValidation(frontend_audio_allocation, audio_device_id)){
                                // either not allocated or can't support listener request
                                LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,
                                    "allocateCapacity: audioDevice["<<audio_device_id<<"] is either not available or can not support listener request ");
                                continue;
                            }
                            audio_allocation_ids[audio_device_id].listener_allocation_ids.push_back(frontend_audio_allocation.allocation_id);
                            allocation_id_to_audio_device_id.insert(std::pair<std::string, size_t > (frontend_audio_allocation.allocation_id, audio_device_id));
                            frontend_audio_status[audio_device_id].allocation_id_csv = createAllocationIdCsv(audio_device_id);
                            this->assignListener(frontend_audio_allocation.allocation_id,audio_allocation_ids[audio_device_id].control_allocation_id);
                        }
                        // if we've reached here, we found an eligible audio device with correct sample rate

                        // check tolerances
                        // only check when sample_rate was not set to don't care)
                        LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, std::fixed << " allocateCapacity - SR requested: " << frontend_audio_allocation.sample_rate
                                                                                                           << "  SR got: " << frontend_audio_status[audio_device_id].sample_rate);
                        if( (frontend::floatingPointCompare(frontend_audio_allocation.sample_rate,0)!=0) &&
                            (frontend::floatingPointCompare(frontend_audio_status[audio_device_id].sample_rate,frontend_audio_allocation.sample_rate)<0 ||
                            frontend::floatingPointCompare(frontend_audio_status[audio_device_id].sample_rate,frontend_audio_allocation.sample_rate+frontend_audio_allocation.sample_rate * frontend_audio_allocation.sample_rate_tolerance/100.0)>0 )){
                            std::ostringstream eout;
                            eout<<std::fixed<<"allocateCapacity("<<int(audio_device_id)<<"): returned sr "<<frontend_audio_status[audio_device_id].sample_rate<<" does not meet tolerance criteria of "<<frontend_audio_allocation.sample_rate_tolerance<<" percent";
                            LOG_INFO(FrontendAudioDevice<AudioStatusStructType>, eout.str());
                            throw std::logic_error(eout.str().c_str());
                        }
                        
                        if(frontend_audio_allocation.device_control){
                            // enable audio device after successful allocation
                            try {
                                enableAudioDevice(audio_device_id,true);
                            } catch(...){
                                std::ostringstream eout;
                                eout<<"allocateCapacity: Failed to enable audio device after allocation";
                                LOG_INFO(FrontendAudioDevice<AudioStatusStructType>, eout.str());
                                throw std::logic_error(eout.str().c_str());
                            }
                        }
                        _usageState = updateUsageState();
                        return true;
                    }
                    // if we made it here, we failed to find an available audio device
                    std::ostringstream eout;
                    eout<<"allocateCapacity: NO AVAILABLE AUDIO DEVICE. Make sure that the device has an initialized frontend_audio_status";
                    LOG_INFO(FrontendAudioDevice<AudioStatusStructType>, eout.str());
                    throw std::logic_error(eout.str().c_str());
                    
                } else if (id == "FRONTEND::listener_allocation") {
                    // Check validity of allocation_id's
                    if (frontend_listener_allocation.existing_allocation_id.empty()){
                        LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"allocateCapacity: MISSING EXISTING ALLOCATION ID");
                        throw CF::Device::InvalidCapacity("MISSING EXISTING ALLOCATION ID", capacities);
                    }
                    if (frontend_listener_allocation.listener_allocation_id.empty()){
                        LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"allocateCapacity: MISSING LISTENER ALLOCATION ID");
                        throw CF::Device::InvalidCapacity("MISSING LISTENER ALLOCATION ID", capacities);
                    }

                    exclusive_lock lock(allocation_id_mapping_lock);

                    // Check if listener allocation ID has already been used
                    if(getAudioDeviceMapping(frontend_listener_allocation.listener_allocation_id) >= 0){
                        LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"allocateCapacity: LISTENER ALLOCATION ID ALREADY IN USE: [" << frontend_listener_allocation.listener_allocation_id << "]");
                        throw frontend::AllocationAlreadyExists("LISTENER ALLOCATION ID ALREADY IN USE", capacities);
                    }
                    // Do not allocate if existing allocation ID does not exist
                    long audio_device_id = getAudioDeviceMapping(frontend_listener_allocation.existing_allocation_id);
                    if (audio_device_id < 0){
                        LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"allocateCapacity: UNKNOWN CONTROL ALLOCATION ID: ["<< frontend_listener_allocation.existing_allocation_id <<"]");
                        throw FRONTEND::BadParameterException("UNKNOWN CONTROL ALLOCATION ID");
                    }

                    // listener allocations are not permitted for PLAYBACK
                    if(frontend_audio_status[audio_device_id].audio_type == "PLAYBACK"){
                        std::ostringstream eout;
                        eout<<"allocateCapacity: listener allocations are not permitted for " << std::string(frontend_audio_status[audio_device_id].audio_type) << " audio type";
                        LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, eout.str());
                        throw CF::Device::InvalidCapacity(eout.str().c_str(), capacities);
                    }

                    audio_allocation_ids[audio_device_id].listener_allocation_ids.push_back(frontend_listener_allocation.listener_allocation_id);
                    allocation_id_to_audio_device_id.insert(std::pair<std::string, size_t > (frontend_listener_allocation.listener_allocation_id, audio_device_id));
                    frontend_audio_status[audio_device_id].allocation_id_csv = createAllocationIdCsv(audio_device_id);
                    this->assignListener(frontend_listener_allocation.listener_allocation_id,frontend_listener_allocation.existing_allocation_id);
                    return true;
                }
                else {
                    LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"allocateCapacity: UNKNOWN ALLOCATION PROPERTY2");
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
            //   - Would end up deallocating a valid audio/listener
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

    template < typename AudioStatusStructType >
    void FrontendAudioDevice<AudioStatusStructType>::deallocateCapacity(const CF::Properties & capacities)
    throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState) {
        LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        for (CORBA::ULong ii = 0; ii < capacities.length(); ++ii) {
            try{
                const std::string id = (const char*) capacities[ii].id;
                if (id != "FRONTEND::audio_allocation" && id != "FRONTEND::listener_allocation"){
                    LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"deallocateCapacity: UNKNOWN ALLOCATION PROPERTY");
                    throw CF::Device::InvalidCapacity("UNKNOWN ALLOCATION PROPERTY", capacities);
                }
                PropertyInterface* property = getPropertyFromId(id);
                if(!property){
                    LOG_INFO(FrontendAudioDevice<AudioStatusStructType>,"deallocateCapacity: UNKNOWN PROPERTY");
                    throw CF::Device::InvalidCapacity("UNKNOWN PROPERTY", capacities);
                }
                try{
                    property->setValue(capacities[ii].value);
                }
                catch(const std::logic_error &e){
                    LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, "COULD NOT PARSE CAPACITY: " << e.what());
                    throw CF::Device::InvalidCapacity("COULD NOT PARSE CAPACITY", capacities);
                };
                if (id == "FRONTEND::audio_allocation"){
                    //LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,std::string(__PRETTY_FUNCTION__)+" audio_allocation");
                    // Try to remove control of the audio device
                    long audio_device_id = getAudioDeviceMapping(frontend_audio_allocation.allocation_id);
                    if (audio_device_id < 0){
                        LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, "ALLOCATION_ID NOT FOUND: [" << frontend_audio_allocation.allocation_id <<"]");
                        throw CF::Device::InvalidCapacity("ALLOCATION_ID NOT FOUND", capacities);
                    }
                    //LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,std::string(__PRETTY_FUNCTION__)+" audio_device_id = " << audio_device_id);
                    if(audio_allocation_ids[audio_device_id].control_allocation_id == frontend_audio_allocation.allocation_id){
                        //LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,std::string(__PRETTY_FUNCTION__)+" deallocating control for audio_device_id = " << audio_device_id);
                        enableAudioDevice(audio_device_id, false);
                        removeAudioDeviceMapping(audio_device_id);
                        frontend_audio_status[audio_device_id].allocation_id_csv = createAllocationIdCsv(audio_device_id);
                    }
                    else {
                        //LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,std::string(__PRETTY_FUNCTION__)+" deallocating listener for audio_device_id = " << audio_device_id);
                        // send EOS to listener connection only
                        removeAudioDeviceMapping(audio_device_id,frontend_audio_allocation.allocation_id);
                        frontend_audio_status[audio_device_id].allocation_id_csv = createAllocationIdCsv(audio_device_id);
                    }
                }
                else if (id == "FRONTEND::listener_allocation") {
                    //LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,std::string(__PRETTY_FUNCTION__)+" listener_allocation");
                    long audio_device_id = getAudioDeviceMapping(frontend_listener_allocation.listener_allocation_id);
                    if (audio_device_id < 0){
                        LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>, "ALLOCATION_ID NOT FOUND: [" << frontend_listener_allocation.listener_allocation_id <<"]");
                        throw CF::Device::InvalidCapacity("ALLOCATION_ID NOT FOUND", capacities);
                    }
                    //LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,std::string(__PRETTY_FUNCTION__)+" audio_device_id = " << audio_device_id);
                    // send EOS to listener connection only
                    removeAudioDeviceMapping(audio_device_id,frontend_listener_allocation.listener_allocation_id);
                    frontend_audio_status[audio_device_id].allocation_id_csv = createAllocationIdCsv(audio_device_id);
                }
                else {
                    LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,"WARNING: UNKNOWN ALLOCATION PROPERTY \""+ std::string(property->name) + "\". IGNORING!");
                }
            }
            catch(...){
                LOG_DEBUG(FrontendAudioDevice<AudioStatusStructType>,"ERROR WHEN DEALLOCATING. SKIPPING...");
            }
        }
        _usageState = updateUsageState();
    }

    /*****************************************************************/
    /* Audio Configurations                                          */
    /*****************************************************************/

    template < typename AudioStatusStructType >
    bool FrontendAudioDevice<AudioStatusStructType>::enableAudioDevice(size_t audio_device_id, bool enable) {
        //LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        
        bool prev_enabled = frontend_audio_status[audio_device_id].enabled;
        
        // If going from disabled to enabled
        if (!prev_enabled && enable) {
            audioDeviceEnable(frontend_audio_status[audio_device_id], audio_device_id);
        }
        
        // If going from enabled to disabled
        if (prev_enabled && !enable) {

            audioDeviceDisable(frontend_audio_status[audio_device_id], audio_device_id);
        }

        return true;
    }

    template < typename AudioStatusStructType >
    bool FrontendAudioDevice<AudioStatusStructType>::listenerRequestValidation(frontend_audio_allocation_struct &request, size_t audio_device_id){
        LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);

        // ensure requested values are non-negative
        if(frontend::floatingPointCompare(request.sample_rate,0)<0 || frontend::floatingPointCompare(request.sample_rate_tolerance,0)<0)
            return false;

        // ensure audio sample rate meets requested tolerance
        //if(request.sample_rate > frontend_audio_status[audio_device_id].sample_rate)
        if( frontend::floatingPointCompare(request.sample_rate,frontend_audio_status[audio_device_id].sample_rate) > 0 )
            return false;

        //if(request.sample_rate != 0 && (request.sample_rate+(request.sample_rate*request.sample_rate_tolerance/100)) < frontend_audio_status[audio_device_id].sample_rate)
        if(frontend::floatingPointCompare(request.sample_rate,0)!=0 && frontend::floatingPointCompare((request.sample_rate+(request.sample_rate*request.sample_rate_tolerance/100)),frontend_audio_status[audio_device_id].sample_rate) < 0 )
            return false;

        return true;
    };

    ////////////////////////////
    //        MAPPING         //
    ////////////////////////////

    template < typename AudioStatusStructType >
    long FrontendAudioDevice<AudioStatusStructType>::getAudioDeviceMapping(std::string allocation_id) {
        //LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        long NO_VALID_AUDIO_DEVICE = -1;

        string_number_mapping::iterator iter = allocation_id_to_audio_device_id.find(allocation_id);
        if (iter != allocation_id_to_audio_device_id.end())
            return iter->second;

        return NO_VALID_AUDIO_DEVICE;
    }
    
    template < typename AudioStatusStructType >
    bool FrontendAudioDevice<AudioStatusStructType>::removeAudioDeviceMapping(size_t audio_device_id, std::string allocation_id) {
        LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        removeListener(allocation_id);
        std::vector<std::string>::iterator it = audio_allocation_ids[audio_device_id].listener_allocation_ids.begin();
        while(it != audio_allocation_ids[audio_device_id].listener_allocation_ids.end()){
            if(*it == allocation_id){
                audio_allocation_ids[audio_device_id].listener_allocation_ids.erase(it);
            } else {
                ++it;
            }
        }
        exclusive_lock lock(allocation_id_mapping_lock);
        if(allocation_id_to_audio_device_id.erase(allocation_id) > 0)
            return true;
        return false;
    }

    template < typename AudioStatusStructType >
    bool FrontendAudioDevice<AudioStatusStructType>::removeAudioDeviceMapping(size_t audio_device_id) {
        LOG_TRACE(FrontendAudioDevice<AudioStatusStructType>,__PRETTY_FUNCTION__);
        audioDeviceDeleteTuning(frontend_audio_status[audio_device_id], audio_device_id);
        removeAllocationIdRouting(audio_device_id);

        long cnt = 0;
        exclusive_lock lock(allocation_id_mapping_lock);
        string_number_mapping::iterator it = allocation_id_to_audio_device_id.begin();
        while(it != allocation_id_to_audio_device_id.end()){
            if(it->second == audio_device_id){
                std::string allocation_id = it->first;
                removeListener(allocation_id);
                allocation_id_to_audio_device_id.erase(it++);
                cnt++;
            } else {
                ++it;
            }
        }
        /*
        for(std::vector<std::string>::iterator it = audio_allocation_ids[audio_device_id].listener_allocation_ids.begin(); it != audio_allocation_ids[audio_device_id].listener_allocation_ids.end();it++){
            removeListener(*it);
            allocation_id_to_audio_device_id.erase(*it);
            cnt++;
        }
        removeListener(audio_allocation_ids[audio_device_id].control_allocation_id);
        allocation_id_to_audio_device_id.erase(audio_allocation_ids[audio_device_id].control_allocation_id);
        */
        audio_allocation_ids[audio_device_id].reset();
        return cnt > 0;
    }

    template < typename AudioStatusStructType >
    void FrontendAudioDevice<AudioStatusStructType>::assignListener(const std::string& listen_alloc_id, const std::string& alloc_id) {
    };

    template < typename AudioStatusStructType >
    void FrontendAudioDevice<AudioStatusStructType>::removeListener(const std::string& listen_alloc_id) {
    };

}; // end frontendX namespace
