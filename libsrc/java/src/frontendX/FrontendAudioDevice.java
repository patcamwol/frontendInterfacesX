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
package frontendX;

import BULKIO.StreamSRI;
import BULKIO.UNITS_TIME;
import BULKIO.UNITS_NONE;
import CF.AggregateDevice;
import CF.DataType;
import CF.DeviceManager;
import CF.DevicePackage.UsageType.*;
import CF.DevicePackage.InvalidCapacity;
import CF.DevicePackage.InvalidCapacityHelper;
import CF.DevicePackage.InvalidState;
import CF.DevicePackage.UsageType;
import CF.InvalidObjectReference;
import frontend.BadParameterException;
import java.lang.Math.*;
import java.lang.reflect.*;
import java.text.*;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.concurrent.*;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;
import java.util.UUID.*;
import org.omg.CORBA.Any;
import org.omg.CORBA.ORB;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAPackage.ServantNotActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;
import org.ossie.component.ThreadedDevice;
import org.ossie.properties.Action;
import org.ossie.properties.Allocator;
import org.ossie.properties.AnyUtils;
import org.ossie.properties.Kind;
import org.ossie.properties.Mode;
import org.ossie.properties.StringProperty;
import org.ossie.properties.StructProperty;
import org.ossie.properties.StructSequenceProperty;

public abstract class FrontendAudioDevice<audioStatusStructType extends frontendX.FEXTypes.default_frontend_audio_status_struct_struct> extends ThreadedDevice {

    /* validateRequestVsSRI is a helper function to check that the input data stream can support
     * the allocation request. True is returned upon success, otherwise
     * Frontend.BadParameterException is thrown.
     */
    public boolean validateRequestVsSRI(final frontendX.FEXTypes.frontend_audio_allocation_struct request, final BULKIO.StreamSRI upstream_sri) throws Frontend.BadParameterException {

        // check sample rate
        double upstream_sr = 1/upstream_sri.xdelta;
        upstream_sr /= (request.full_bandwidth_channels + request.low_frequency_effect_channels);
        double min_requested_sample_rate = request.sample_rate;
        double max_requested_sample_rate = request.sample_rate+request.sample_rate * request.sample_rate_tolerance/100.0;

        // check vs. upstream sample rate (ensure min request <= upstream <= max request)
        if ( !validateRequest(min_requested_sample_rate, max_requested_sample_rate, upstream_sample_rate) ){
            throw Frontend.BadParameterException("INVALID REQUEST -- upstream sr does not agree with sr request");
        }

        return true;
    }

    /* validateRequestVsDevice is a helper function to check that the input data stream and the
     * device can support an allocation request. True is returned upon success, otherwise 
     * Frontend.BadParameterException is thrown.
     */
    bool validateRequestVsDevice(final frontend_audio_allocation_struct request, final BULKIO.StreamSRI upstream_sri,
            double max_device_sample_rate){

        // check if request can be satisfied using the available upstream data
        if( !validateRequestVsSRI(request,upstream_sri) ){
            throw Frontend.BadParameterException("INVALID REQUEST -- falls outside of input data stream");
        }

        // check device constraints

        // check vs. device sample rate capability (ensure 0 <= request <= max device capability)
        if ( !validateRequest(0,max_device_sample_rate,request.sample_rate) ){
            throw Frontend.BadParameterException("INVALID REQUEST -- device capabilities cannot support sr request");
        }

        return true;
    }

    /* validateRequestVsDevice is a helper function to check that the analog capabilities and the
     * device can support the allocation request. True is returned upon success, otherwise 
     * Frontend.BadParameterException is thrown.
     */
    bool validateRequestVsDevice(final frontend_audio_allocation_struct request, double max_device_sample_rate){

        // check device constraints

        // check vs. device sample rate capability (ensure 0 <= request <= max device capability)
        if ( !validateRequest(0,max_device_sample_rate,request.sample_rate) ){
            throw Frontend.BadParameterException("INVALID REQUEST -- device capabilities cannot support sr request");
        }

        return true;
    }

    public FrontendAudioDevice() {
        super();
        construct();
    }
 
    private void construct() {
        loadProperties();
        allocation_id_to_audio_device_id = new HashMap<String,Integer>();
        frontend_audio_allocation.setAllocator(new Allocator<frontendX.FEXTypes.frontend_audio_allocation_struct>() {
            public boolean allocate(frontendX.FEXTypes.frontend_audio_allocation_struct capacity){
                boolean status = false;
                try{
                    status = allocateAudioDevice(capacity);
                }catch(CF.DevicePackage.InvalidCapacity e){

                }catch(Exception e){

                }
                return status;
            }
            public void deallocate(frontendX.FEXTypes.frontend_audio_allocation_struct capacity){
                deallocateAudioDevice(capacity);
            }
        });
        frontend_listener_allocation.setAllocator(new Allocator<FRONTEND.FETypes.frontend_listener_allocation_struct>() {
            public boolean allocate(FRONTEND.FETypes.frontend_listener_allocation_struct capacity) {
                boolean status = false;
                try{
                    status = allocateListener(capacity);
                }catch(CF.DevicePackage.InvalidCapacity e){

                }catch(Exception e){

                }
                return status;
            }
            public void deallocate(FRONTEND.FETypes.frontend_listener_allocation_struct capacity){
                deallocateListener(capacity);
            }
        });
    }
    
    // this is implemented in the generated base class once all properties are known
    public void loadProperties(){
        addProperty(device_kind);
        addProperty(device_model);
        addProperty(frontend_audio_allocation);
        addProperty(frontend_listener_allocation);
        addProperty(frontend_audio_status);
    }

    protected String createAllocationIdCsv(int audio_device_id){
        StringBuilder alloc_id_csv = new StringBuilder();
        // ensure control allocation_id is first in list
        if (audio_allocation_ids.get(audio_device_id).control_allocation_id != null && !audio_allocation_ids.get(audio_device_id).control_allocation_id.isEmpty())
            alloc_id_csv.append(audio_allocation_ids.get(audio_device_id).control_allocation_id + ",");
        // now add the rest
        Iterator<String> iter = audio_allocation_ids.get(audio_device_id).listener_allocation_ids.iterator();
        while (iter.hasNext()) {
            alloc_id_csv.append(iter.next() + ",");
        }
        // and get rid of the trailing comma
        if(alloc_id_csv.length() > 0){
            alloc_id_csv.setLength(alloc_id_csv.length()-1);
        }
        return alloc_id_csv.toString();
    }

    protected String getControlAllocationId(int audio_device_id){
        return audio_allocation_ids.get(audio_device_id).control_allocation_id;
    }

    protected List<String> getListenerAllocationIds(int audio_device_id){
        return audio_allocation_ids.get(audio_device_id).listener_allocation_ids;
    }

    /*****************************************************************/
    /* Allocation/Deallocation of Capacity                           */
    /*****************************************************************/
    // updateUsageState is not defined to return UsageType in Device base class
    public void updateUsageState() {
        getUsageState();
    }

    public CF.DevicePackage.UsageType getUsageState() {
        int audioDeviceAllocated = 0;
        for (int audio_device_id = 0; audio_device_id < audio_allocation_ids.size(); audio_device_id++) {
            if (!audio_allocation_ids.get(audio_device_id).control_allocation_id.isEmpty())
                audioDeviceAllocated++;
        }
        // If no audio devices are allocated, device is idle
        if (audioDeviceAllocated == 0)
            return CF.DevicePackage.UsageType.IDLE;
        // If all audio devices are allocated, device is busy
        if (audioDeviceAllocated == audio_allocation_ids.size())
            return CF.DevicePackage.UsageType.BUSY;
        // Else, device is active
        return CF.DevicePackage.UsageType.ACTIVE;
    }

    public boolean allocateAudioDevice(frontendX.FEXTypes.frontend_audio_allocation_struct frontend_audio_allocation) throws CF.DevicePackage.InvalidCapacity, Exception {
        try{
            // Check allocation_id
            if (frontend_audio_allocation.allocation_id != null &&
                frontend_audio_allocation.allocation_id.getValue().isEmpty()) {
                logger.info("allocateAudioDevice: MISSING ALLOCATION_ID");
                throw new CF.DevicePackage.InvalidCapacity("MISSING ALLOCATION ID", new CF.DataType[]{new DataType("frontend_audio_allocation", frontend_audio_allocation.toAny())});
            }
            // Check if allocation ID has already been used
            if(this.getAudioDeviceMapping(frontend_audio_allocation.allocation_id.getValue()) >= 0){
                logger.info("allocateAudioDevice: ALLOCATION_ID ALREADY IN USE: [" + frontend_audio_allocation.allocation_id.getValue() + "]");
                throw new InvalidCapacity("ALLOCATION_ID ALREADY IN USE", new CF.DataType[]{new DataType("frontend_audio_allocation", frontend_audio_allocation.toAny())});
            }
            // Check if available audio device
            //synchronized(allocation_id_mapping_lock){
                // Next, try to allocate a new audio
                for (int audio_device_id = 0; audio_device_id < this.audio_allocation_ids.size(); audio_device_id++) {
                    if(!frontend_audio_status.getValue().get(audio_device_id).audio_type.getValue().equals(frontend_audio_allocation.audio_type.getValue())) {
                        logger.debug("allocateAudioDevice: Requested audio type '"+frontend_audio_allocation.audio_type.getValue() +"' does not match audioDevice[" + audio_device_id + "].audio_type (" + frontend_audio_status.getValue().get(audio_device_id).audio_type.getValue()+")");
                        continue;
                    }
 
                    if(frontend_audio_allocation.device_control.getValue()){
                        // device control
                            short orig_fbc = frontend_audio_status.getValue().get(audio_device_id).full_bandwidth_channels.getValue();
                            short orig_lfec = frontend_audio_status.getValue().get(audio_device_id).low_frequency_effect_channels.getValue();
                        double orig_sr = frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue();
                        frontend_audio_status.getValue().get(audio_device_id).full_bandwidth_channels.setValue(frontend_audio_allocation.full_bandwidth_channels.getValue());
                        frontend_audio_status.getValue().get(audio_device_id).low_frequency_effect_channels.setValue(frontend_audio_allocation.low_frequency_effect_channels.getValue());
                        frontend_audio_status.getValue().get(audio_device_id).sample_rate.setValue(frontend_audio_allocation.sample_rate.getValue());
                        if(audio_allocation_ids.get(audio_device_id).control_allocation_id != null &&
                           (!audio_allocation_ids.get(audio_device_id).control_allocation_id.isEmpty() || 
                            !audioDeviceSetTuning(frontend_audio_allocation, frontend_audio_status.getValue().get(audio_device_id), audio_device_id))){
                            // either not available or didn't succeed setting tuning, try next audio device
                            if (frontend_audio_status.getValue().get(audio_device_id).full_bandwidth_channels.getValue().equals(frontend_audio_allocation.full_bandwidth_channels.getValue()))
                                frontend_audio_status.getValue().get(audio_device_id).full_bandwidth_channels.setValue(orig_fbc);
                            if (frontend_audio_status.getValue().get(audio_device_id).low_frequency_effect_channels.getValue().equals(frontend_audio_allocation.low_frequency_effect_channels.getValue()))
                                frontend_audio_status.getValue().get(audio_device_id).low_frequency_effect_channels.setValue(orig_lfec);
                            if (frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue().equals(frontend_audio_allocation.sample_rate.getValue()))
                                frontend_audio_status.getValue().get(audio_device_id).sample_rate.setValue(orig_sr);
                            logger.debug("allocateAudioDevice: audioDevice["+audio_device_id+"] is either not available or didn't succeed while setting tuning ");
                            continue;
                        }
                        audio_allocation_ids.get(audio_device_id).control_allocation_id = frontend_audio_allocation.allocation_id.getValue();
                        allocation_id_to_audio_device_id.put(frontend_audio_allocation.allocation_id.getValue(), audio_device_id);
                        frontend_audio_status.getValue().get(audio_device_id).allocation_id_csv.setValue(createAllocationIdCsv(audio_device_id));
                    } else {
                        // listener
                        if(audio_allocation_ids.get(audio_device_id).control_allocation_id.isEmpty() || !listenerRequestValidation(frontend_audio_allocation, audio_device_id)){
                            // either not allocated or can't support listener request
                            logger.debug("allocateAudioDevice: audioDevice"+audio_device_id+"] is either not available or can not support listener request ");
                            continue;
                        }
                        audio_allocation_ids.get(audio_device_id).listener_allocation_ids.add(frontend_audio_allocation.allocation_id.getValue());
                        allocation_id_to_audio_device_id.put(frontend_audio_allocation.allocation_id.getValue(), audio_device_id);
                        frontend_audio_status.getValue().get(audio_device_id).allocation_id_csv.setValue(createAllocationIdCsv(audio_device_id));
                        this.assignListener(frontend_audio_allocation.allocation_id.getValue(),audio_allocation_ids.get(audio_device_id).control_allocation_id);
                    }
                    // if we've reached here, we found an eligible audio with correct sample_rate

                    // check tolerances
                    // only check when sample_rate was not set to don't care)
                    logger.debug(" allocateAudioDevice - SR requested: " + frontend_audio_allocation.sample_rate.getValue() + "  SR got: " + frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue());
                    if( (floatingPointCompare(frontend_audio_allocation.sample_rate.getValue(),0)!=0) &&
                        (floatingPointCompare(frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue(),frontend_audio_allocation.sample_rate.getValue())<0 ||
                        floatingPointCompare(frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue(),frontend_audio_allocation.sample_rate.getValue()+frontend_audio_allocation.sample_rate.getValue() * frontend_audio_allocation.sample_rate_tolerance.getValue()/100.0)>0 )){
                        String eout = "allocateAudioDevice(" + audio_device_id + "): returned sr " + frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue()+" does not meet tolerance criteria of " + frontend_audio_allocation.sample_rate_tolerance.getValue()+" percent";
                        logger.info(eout);
                        throw new RuntimeException(eout);
                    }

                    if(frontend_audio_allocation.device_control.getValue()){
                        // enable audio device after successful allocation
                        try {
                            enableAudioDevice(audio_device_id,true);
                        } catch(Exception e){
                            String eout = "allocateAudioDevice: Failed to enable audio after allocation";
                            logger.info(eout);
                            throw new RuntimeException(eout);
                        }
                    }
                    usageState = getUsageState();
                    return true;
                }
                // if we made it here, we failed to find an available audio device
                String eout = "allocateAudioDevice: NO AVAILABLE AUDIO DEVICE. Make sure that the device has an initialized frontend_audio_status";
                logger.info(eout);
                throw new RuntimeException(eout);
            //}
        } catch(RuntimeException e) {
            //deallocateAudioDevice(frontend_audio_allocation);
            return false;
        } catch(CF.DevicePackage.InvalidCapacity e) {
            // without the following check, a valid allocation could be deallocated due to an attempt to alloc w/ an existing alloc id
            String exceptionMessage = e.getMessage();
            if (exceptionMessage != null && 
               exceptionMessage.indexOf("ALLOCATION_ID ALREADY IN USE") == -1){
               //deallocateAudioDevice(frontend_audio_allocation);
            }
            throw e;
        } catch(Exception e){
            //deallocateAudioDevice(frontend_audio_allocation);
            throw e; 
        }
    }

    public void deallocateAudioDevice(frontendX.FEXTypes.frontend_audio_allocation_struct frontend_audio_deallocation){
        try{
            //logger.debug("deallocateAudioDevice()");
            // Try to remove control of the device
            int audio_device_id = this.getAudioDeviceMapping(frontend_audio_deallocation.allocation_id.getValue());
            if (audio_device_id < 0){
                logger.debug("ALLOCATION_ID NOT FOUND: [" + frontend_audio_deallocation.allocation_id.getValue() +"]");
                throw new CF.DevicePackage.InvalidCapacity("ALLOCATION_ID NOT FOUND", new CF.DataType[]{new DataType("frontend_audio_deallocation", frontend_audio_deallocation.toAny())});
            }
            //logger.debug("deallocateAudioDevice() audio_device_id = " + audio_device_id);
            if(audio_allocation_ids.get(audio_device_id).control_allocation_id.equals(frontend_audio_deallocation.allocation_id.getValue())){
                //logger.debug("deallocateAudioDevice() deallocating control for audio_device_id = " + audio_device_id);
                enableAudioDevice(audio_device_id, false);
                frontend_audio_status.getValue().get(audio_device_id).allocation_id_csv.setValue("");
                removeAudioDeviceMapping(audio_device_id);
                audio_allocation_ids.get(audio_device_id).control_allocation_id = "";
            }else{
                // send EOS to listener connection only
                removeAudioDeviceMapping(audio_device_id,frontend_audio_deallocation.allocation_id.getValue());
                frontend_audio_status.getValue().get(audio_device_id).allocation_id_csv.setValue(createAllocationIdCsv(audio_device_id));
            }
        } catch (Exception e){
            System.out.println("deallocateAudioDevice: ERROR WHEN DEALLOCATING.  SKIPPING..");
        }
        usageState = getUsageState();
    }

    public boolean allocateListener(FRONTEND.FETypes.frontend_listener_allocation_struct frontend_listener_allocation) throws CF.DevicePackage.InvalidCapacity, Exception {
        try{
            // Check validity of allocation_id's
            if (frontend_listener_allocation.existing_allocation_id == null || 
                frontend_listener_allocation.existing_allocation_id.getValue().isEmpty()){
                logger.info("allocateListener: MISSING EXISTING ALLOCATION ID");
                throw new CF.DevicePackage.InvalidCapacity("MISSING EXISTING ALLOCATION ID", new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
            }
            if (frontend_listener_allocation.listener_allocation_id == null ||
                frontend_listener_allocation.listener_allocation_id.getValue().isEmpty()){
                logger.info("allocateListener: MISSING LISTENER ALLOCATION ID");
                throw new CF.DevicePackage.InvalidCapacity("MISSING LISTENER ALLOCATION ID", new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
            }

            //synchronized(allocation_id_mapping_lock){
                // Check if listener allocation ID has already been used
                if(getAudioDeviceMapping(frontend_listener_allocation.listener_allocation_id.getValue()) >= 0){
                    logger.error("allocateListener: LISTENER ALLOCATION_ID ALREADY IN USE");
                    throw new InvalidCapacity("LISTENER ALLOCATION_ID ALREADY IN USE", new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
                }

                // Do not allocate if existing allocation ID does not exist
                int audio_device_id = getAudioDeviceMapping(frontend_listener_allocation.existing_allocation_id.getValue());
                if (audio_device_id < 0){
                    logger.info("allocateListener: UNKNOWN CONTROL ALLOCATION ID: ["+ frontend_listener_allocation.existing_allocation_id.getValue() +"]");
                    throw new Frontend.BadParameterException("UNKNOWN CONTROL ALLOCATION ID");
                }

                // listener allocations are not permitted for PLAYBACK
                if(frontend_audio_status.getValue().get(audio_device_id).audio_type.getValue().equals("PLAYBACK")){
                    String eout = "allocateListener: listener allocations are not permitted for " + frontend_audio_status.getValue().get(audio_device_id).audio_type.getValue() + " audio type";
                    logger.debug(eout);
                    throw new CF.DevicePackage.InvalidCapacity(eout, new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
                }

                audio_allocation_ids.get(audio_device_id).listener_allocation_ids.add(frontend_listener_allocation.listener_allocation_id.getValue());
                allocation_id_to_audio_device_id.put(frontend_listener_allocation.listener_allocation_id.getValue(), audio_device_id);
                frontend_audio_status.getValue().get(audio_device_id).allocation_id_csv.setValue(createAllocationIdCsv(audio_device_id));
                this.assignListener(frontend_listener_allocation.listener_allocation_id.getValue(),frontend_listener_allocation.existing_allocation_id.getValue());
                return true;
            //}
        }catch(CF.DevicePackage.InvalidCapacity e){
            String exceptionMessage = e.getMessage();
            // without the following check, a valid allocation could be deallocated due to an attempt to alloc w/ an existing alloc id
            if (exceptionMessage != null && 
               exceptionMessage.indexOf("ALLOCATION_ID ALREADY IN USE") == -1){
            }
            throw e;
        } catch (Frontend.BadParameterException e){
            return false;
        } catch (Exception e){
            throw e;
        }
    }

    public void deallocateListener(FRONTEND.FETypes.frontend_listener_allocation_struct frontend_listener_allocation){
        try{
            int audio_device_id = getAudioDeviceMapping(frontend_listener_allocation.listener_allocation_id.getValue());
            if (audio_device_id < 0){
                logger.debug("ALLOCATION_ID NOT FOUND: [" + frontend_listener_allocation.listener_allocation_id.getValue() + "]");
                throw new CF.DevicePackage.InvalidCapacity("ALLOCATION_ID NOT FOUND", new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
            }
            // send EOS to listener connection only
            removeAudioDeviceMapping(audio_device_id, frontend_listener_allocation.listener_allocation_id.getValue());
            frontend_audio_status.getValue().get(audio_device_id).allocation_id_csv.setValue(removeListenerId(audio_device_id, frontend_listener_allocation.listener_allocation_id.getValue()));
        } catch (Exception e){
            logger.debug("deallocateListener: ERROR WHEN DEALLOCATING.  SKIPPING..");
        }
    }
    
    public String removeListenerId(final int audio_device_id, final String allocation_id) {
        String[] split_id = frontend_audio_status.getValue().get(audio_device_id).allocation_id_csv.getValue().split(",");
        int idx;
        for (idx=0; idx<split_id.length; idx++) {
            if (split_id[idx].equals(allocation_id)) {
                break;
            }
        }
        String cleanedString = "";
        for (int idx_i=0; idx_i<split_id.length; idx_i++) {
            if (idx == idx_i) {
                continue;
            }
            cleanedString += split_id[idx_i] + ",";
        }
        cleanedString = cleanedString.replaceAll(",$","");
        return cleanedString;
    }

    /*****************************************************************/
    /* Audio Configurations                                          */
    /*****************************************************************/

    protected boolean enableAudioDevice(int audio_device_id, boolean enable){
        boolean prev_enabled = frontend_audio_status.getValue().get(audio_device_id).enabled.getValue();
        // If going from disabled to enabled
        if (!prev_enabled && enable) {
            audioDeviceEnable(frontend_audio_status.getValue().get(audio_device_id), audio_device_id);
        }

        // If going from enabled to disabled
        if (prev_enabled && !enable) {

            audioDeviceDisable(frontend_audio_status.getValue().get(audio_device_id), audio_device_id);
        }

        return true;
    }

    protected boolean listenerRequestValidation(frontendX.FEXTypes.frontend_audio_allocation_struct request, int audio_device_id){
        logger.trace("listenerRequestValidation() audio_device_id " + audio_device_id);

        // ensure requested values are non-negative
        if(floatingPointCompare(request.sample_rate.getValue(),0)<0 || floatingPointCompare(request.sample_rate_tolerance.getValue(),0)<0)
            return false;

        // ensure audio sample rate meets requested tolerance
        if( floatingPointCompare(request.sample_rate.getValue(),frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue()) > 0 )
            return false;

        if(floatingPointCompare(request.sample_rate.getValue(),0)!=0 && floatingPointCompare((request.sample_rate.getValue()+(request.sample_rate.getValue()*request.sample_rate_tolerance.getValue()/100)),frontend_audio_status.getValue().get(audio_device_id).sample_rate.getValue()) < 0 )
            return false;

        return true;
    }

    ////////////////////////////
    //        MAPPING         //
    ////////////////////////////

    protected int getAudioDeviceMapping(String allocation_id){
        //logger.trace("getAudioDeviceMapping() allocation_id " + allocation_id);
        int NO_VALID_AUDIO_DEVICE = -1;

        if (allocation_id_to_audio_device_id.containsKey(allocation_id)){
            return allocation_id_to_audio_device_id.get(allocation_id);
        } 
        return NO_VALID_AUDIO_DEVICE;
    }

    protected boolean removeAudioDeviceMapping(int audio_device_id, String allocation_id){
        logger.trace("removeAudioDeviceMapping() audio_device_id " + audio_device_id + ", allocation_id " + allocation_id);
        removeListener(allocation_id);
        Iterator<String> iter = audio_allocation_ids.get(audio_device_id).listener_allocation_ids.iterator();
        while(iter.hasNext()){
            String nextString = iter.next();
            if (nextString.equals(allocation_id)){
                iter.remove();
            }
        }
        //synchronized(allocation_id_mapping_lock){
            if (allocation_id_to_audio_device_id.containsKey(allocation_id)){
                allocation_id_to_audio_device_id.remove(allocation_id);
                return true;
            }
            return false;
        //}
    }

    protected boolean removeAudioDeviceMapping(int audio_device_id){
        logger.trace("removeAudioDeviceMapping() audio_device_id " + audio_device_id);
        audioDeviceDeleteTuning(frontend_audio_status.getValue().get(audio_device_id),audio_device_id);
        removeAllocationIdRouting(audio_device_id);

        int cnt = 0;
        //synchronized(allocation_id_mapping_lock){
            if (allocation_id_to_audio_device_id.containsValue(audio_device_id)){
                Iterator<Map.Entry<String,Integer>> iter = allocation_id_to_audio_device_id.entrySet().iterator();
                while (iter.hasNext()) {
                    Map.Entry<String,Integer> entry = iter.next();
                    if(audio_device_id == entry.getValue()){
                        this.removeListener(entry.getKey());
                        iter.remove();
                        cnt++;
                    }
                }
            }
            audio_allocation_ids.get(audio_device_id).reset();
            return cnt > 0;
        //}
    }

    protected void assignListener(final String listen_alloc_id, final String alloc_id){
    }

    protected void removeListener(final String listen_alloc_id){
    }
   

    /* floatingPointCompare is a helper function to handle floating point comparison
     * Return values:
     *   if lhs == rhs: 0.0
     *   if lhs >  rhs: 1.0 or greater
     *   if lhs <  rhs: -1.0 or less
     * Recommended usage is to convert a comparison such as: (lhs OP rhs)
     * to (floatingPointCompare(lhs,rhs) OP 0), where OP is a comparison operator
     * (==, <, >, <=, >=, !=).
     * "places" is used to specify precision. The default is 1, which
     * uses a single decimal place of precision.
     */
    public double floatingPointCompare(double lhs, double rhs){
        return floatingPointCompare(lhs, rhs, 1);
    }

    public double floatingPointCompare(double lhs, double rhs, int places){
        return java.lang.Math.rint((lhs-rhs)*java.lang.Math.pow(10.0,(double)places));
    }

    /* validateRequest is a helper function to verify a value is within a range, returning
     * true if the value requested_val falls within the range [available_min:available_max]
     * False is returned if min > max
     */
    public boolean validateRequest(double available_min, double available_max, double requested_val){
        if(floatingPointCompare(requested_val,available_min) < 0) return false;
        if(floatingPointCompare(requested_val,available_max) > 0) return false;
        if(floatingPointCompare(available_min,available_max) > 0) return false;
        return true;
    }

    /* validateRequest is a helper function to compare two ranges, returning true if the range
     * [requested_min:requested_max] falls within the range [available_min:available_max]
     * False is returned if min > max for either available for requested values
     */
    public boolean validateRequest(double available_min, double available_max, double requested_min, double requested_max){
        if(floatingPointCompare(requested_min,available_min) < 0) return false;
        if(floatingPointCompare(requested_max,available_max) > 0) return false;
        if(floatingPointCompare(available_min,available_max) > 0) return false;
        if(floatingPointCompare(requested_min,requested_max) > 0) return false;
        return true;
    }

    /* audio Allocation IDs struct. This structure contains allocation tracking data.
     *
     */

    public class audioAllocationIdsStruct{
        public String control_allocation_id;
        public List<String> listener_allocation_ids;
        public audioAllocationIdsStruct(){
            control_allocation_id = new String();
            listener_allocation_ids = new ArrayList<String>();
            reset();
        }
        public void reset(){
            control_allocation_id = "";
            listener_allocation_ids.clear();
        }
    }

    // audio_allocation_ids is exclusively paired with property frontend_audio_status.
    // audio_allocation_ids tracks allocation ids while frontend_audio_status provides audio information.
    protected List<audioAllocationIdsStruct> audio_allocation_ids;

    protected StringProperty device_kind =
        new StringProperty(
            "DCE:cdc5ee18-7ceb-4ae6-bf4c-31f983179b4d", //id
            "device_kind", //name
            "FRONTEND::AUDIO", //default value
            Mode.READONLY, //mode
            Action.EQ, //action
            new Kind[] {Kind.ALLOCATION,Kind.CONFIGURE} //kind
            );

    protected StringProperty device_model =
        new StringProperty(
            "DCE:0f99b2e4-9903-4631-9846-ff349d18ecfb", //id
            "device_model", //name
            "", //default value
            Mode.READONLY, //mode
            Action.EQ, //action
            new Kind[] {Kind.ALLOCATION,Kind.CONFIGURE} //kind
            );

    protected StructProperty<frontendX.FEXTypes.frontend_audio_allocation_struct> frontend_audio_allocation =
        new StructProperty<frontendX.FEXTypes.frontend_audio_allocation_struct>(
            "FRONTEND::audio_allocation", //id
            "frontend_audio_allocation", //name
            frontendX.FEXTypes.frontend_audio_allocation_struct.class, //type
            new frontend.FETypes.frontend_audio_allocation_struct(), //default value
            Mode.READWRITE, //mode
            new Kind[] {Kind.ALLOCATION} //kind
            );

    protected StructProperty<frontend.FETypes.frontend_listener_allocation_struct> frontend_listener_allocation =
        new StructProperty<frontend.FETypes.frontend_listener_allocation_struct>(
            "FRONTEND::listener_allocation", //id
            "frontend_listener_allocation", //name
            FRONTEND.FETypes.frontend_listener_allocation_struct.class, //type
            new FRONTEND.FETypes.frontend_listener_allocation_struct(), //default value
            Mode.READWRITE, //mode
            new Kind[] {Kind.ALLOCATION} //kind
            );

    private Class<audioStatusStructType> frontend_audio_status_class_type;

    protected StructSequenceProperty<audioStatusStructType> frontend_audio_status =
        new StructSequenceProperty<audioStatusStructType> (
            "FRONTEND::audio_status", //id
            "frontend_audio_status", //name
            frontend_audio_status_class_type, //type
            new ArrayList<audioStatusStructType>(),
            Mode.READONLY, //mode
            new Kind[] { Kind.CONFIGURE } //kind
        );

    protected Map<String, Integer> allocation_id_to_audio_device_id;

    protected Object allocation_id_mapping_lock;

    ///////////////////////////////
    // Device specific functions // -- to be implemented by device developer
    ///////////////////////////////
    protected abstract void audioDeviceEnable(audioStatusStructType fts, int audio_device_id);
    protected abstract void audioDeviceDisable(audioStatusStructType fts, int audio_device_id);
    protected abstract boolean audioDeviceSetTuning(final frontendX.FEXTypes.frontend_audio_allocation_struct request, audioStatusStructType fts, int audio_device_id);
    protected abstract boolean audioDeviceDeleteTuning(audioStatusStructType fts, int audio_device_id);

    ///////////////////////////////
    // Mapping and translation helpers. External string identifiers to internal numerical identifiers
    ///////////////////////////////
    protected abstract void removeAllocationIdRouting(final int audio_device_id);

    ////////////////////////////
    // Other helper functions //
    ////////////////////////////
    protected BULKIO.StreamSRI create(String stream_id, audioStatusStructType frontend_status) {
        return create(stream_id, frontend_status, -1.0);
    }

    protected BULKIO.StreamSRI create(String stream_id, audioStatusStructType frontend_status, double collector_frequency) {
        BULKIO.StreamSRI sri = new BULKIO.StreamSRI();
        sri.xstart = 0.0;
        if ( frontend_status.sample_rate.getValue() <= 0.0 )
            sri.xdelta =  1.0;
        else
            sri.xdelta = 1/(frontend_status.sample_rate.getValue() * (frontend_status.full_bandwidth_channels.getValue() + frontend_status.low_frequency_effect_channels.getValue()));
        sri.xunits = BULKIO.UNITS_TIME.value;
        sri.subsize = 0;
        sri.ystart = 0.0;
        sri.ydelta = 0.0;
        sri.yunits = BULKIO.UNITS_NONE.value;
        sri.mode = 0;
        sri.blocking=false;
        sri.streamID = stream_id;
        return sri;
    }

    protected boolean addModifyKeyword(BULKIO.StreamSRI sri, String id, Any value) {
        return addModifyKeyword(sri, id, value, false);
    }

    protected boolean addModifyKeyword(BULKIO.StreamSRI sri, String id, Any value, boolean addOnly) {
        if (!addOnly && sri.keywords != null) {
            // find and replace
            for(int ii=0; ii<sri.keywords.length; ii++){
                if (sri.keywords[ii].id == id){
                    sri.keywords[ii].value = value;
                    return true;
                }
            }
        }
        CF.DataType[] updatedKeywords;
        if (sri.keywords != null){
            updatedKeywords = new CF.DataType[sri.keywords.length+1];
            for (int ii=0; ii<sri.keywords.length; ii++){
                updatedKeywords[ii] = sri.keywords[ii];
            }
            updatedKeywords[sri.keywords.length] = new DataType(id,value);
        }else{
            updatedKeywords = new CF.DataType[]{new DataType(id,value)};
        }
        sri.keywords = updatedKeywords;
        return true;
    }

    protected void printSRI(BULKIO.StreamSRI sri){
        this.printSRI(sri, "DEBUG SRI");
    }

    protected void printSRI(BULKIO.StreamSRI sri, String strHeader){
        System.out.println(strHeader);
        System.out.println("\thversion: " + sri.hversion);
        System.out.println("\txstart: " + sri.xstart);
        System.out.println("\txdelta: " + sri.xdelta);
        System.out.println("\txunits: " + sri.xunits);
        System.out.println("\tsubsize: " + sri.subsize);
        System.out.println("\tystart: " + sri.ystart);
        System.out.println("\tydelta: " + sri.ydelta);
        System.out.println("\tyunits: " + sri.yunits);
        System.out.println("\tmode: " + sri.mode);
        System.out.println("\tstreamID: " + sri.streamID);
        for (int ii=0; ii<sri.keywords.length; ii++){
            System.out.println("\tKEYWORD KEY:VAL : " + sri.keywords[ii].id + ":" + String.valueOf(AnyUtils.convertAny(sri.keywords[ii].value)));
        }
    }
}
