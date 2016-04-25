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
import FRONTEND.BadParameterException;
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

public abstract class FrontendVideoDevice<VideoStatusStructType extends frontendX.FEXTypes.default_frontend_video_status_struct_struct> extends ThreadedDevice {

    /* validateRequestVsSRI is a helper function to check that the input data stream can support
     * the allocation request. True is returned upon success, otherwise 
     * FRONTEND.BadParameterException is thrown.
     */
    bool validateRequestVsSRI(final frontend_video_allocation_struct request, final BULKIO.StreamSRI upstream_sri){

        // Check if the upstream sample rate falls within the requested tolerable frame rate
        double upstream_fps = 1.0/upstream_sri.xdelta;
        double upstream_frame_rate = upstream_fps / (request.frame_height.getValue() * request.frame_width.getValue() * request.channels.getValue());
        double min_requested_frame_rate = request.fps;
        double max_requested_frame_rate = request.fps+request.fps * request.fps_tolerance/100.0;

        // check vs. upstream frame rate (ensure min request <= upstream <= max request)
        if ( !validateRequest(min_requested_frame_rate, max_requested_frame_rate, upstream_frame_rate) ){
            throw FRONTEND.BadParameterException("INVALID REQUEST -- upstream fr does not agree with fr request");
        }

        return true;
    }

    /* validateRequestVsDevice is a helper function to check that the input data stream and the
     * device can support an allocation request. True is returned upon success, otherwise 
     * FRONTEND.BadParameterException is thrown.
     */
    bool validateRequestVsDevice(final frontend_video_allocation_struct request, final BULKIO.StreamSRI upstream_sri, double max_device_frame_rate){

        // check if request can be satisfied using the available upstream data
        if( !validateRequestVsSRI(request,upstream_sri) ){
            throw FRONTEND.BadParameterException("INVALID REQUEST -- falls outside of input data stream");
        }

        // check device constraints

        // check vs. device frame rate capability (ensure 0 <= request <= max device capability)
        if ( !validateRequest(0,max_device_frame_rate,request.fps) ){
            throw FRONTEND.BadParameterException("INVALID REQUEST -- device capabilities cannot support fr request");
        }

        return true;
    }

    /* validateRequestVsDevice is a helper function to check that the analog capabilities and the
     * device can support the allocation request. True is returned upon success, otherwise 
     * FRONTEND.BadParameterException is thrown.
     */
    bool validateRequestVsDevice(final frontend_video_allocation_struct request, double max_device_frame_rate){

        // check device constraints

        // check vs. device frame rate capability (ensure 0 <= request <= max device capability)
        if ( !validateRequest(0,max_device_frame_rate,request.fps) ){
            throw FRONTEND.BadParameterException("INVALID REQUEST -- device capabilities cannot support fr request");
        }

        return true;
    }

    public FrontendVideoDevice() {
        super();
        construct();
    }
 
    private void construct() {
        loadProperties();
        allocation_id_to_video_device_id = new HashMap<String,Integer>();
        frontend_video_allocation.setAllocator(new Allocator<frontendX.FEXTypes.frontend_video_allocation_struct>() {
            public boolean allocate(frontendX.FEXTypes.frontend_video_allocation_struct capacity){
                boolean status = false;
                try{
                    status = allocateVideo(capacity);
                }catch(CF.DevicePackage.InvalidCapacity e){

                }catch(Exception e){

                }
                return status;
            }
            public void deallocate(frontendX.FEXTypes.frontend_video_allocation_struct capacity){
                deallocateVideo(capacity);
            }
        });
        frontend_listener_allocation.setAllocator(new Allocator<frontend.FETypes.frontend_listener_allocation_struct>() {
            public boolean allocate(frontend.FETypes.frontend_listener_allocation_struct capacity) {
                boolean status = false;
                try{
                    status = allocateListener(capacity);
                }catch(CF.DevicePackage.InvalidCapacity e){

                }catch(Exception e){

                }
                return status;
            }
            public void deallocate(frontend.FETypes.frontend_listener_allocation_struct capacity){
                deallocateListener(capacity);
            }
        });
    }
    
    // this is implemented in the generated base class once all properties are known
    public void loadProperties(){
        addProperty(device_kind);
        addProperty(device_model);
        addProperty(frontend_video_allocation);
        addProperty(frontend_listener_allocation);
        addProperty(frontend_video_status);
    }

    protected String createAllocationIdCsv(int video_device_id){
        StringBuilder alloc_id_csv = new StringBuilder();
        // ensure control allocation_id is first in list
        if (video_allocation_ids.get(video_device_id).control_allocation_id != null && !video_allocation_ids.get(video_device_id).control_allocation_id.isEmpty())
            alloc_id_csv.append(video_allocation_ids.get(video_device_id).control_allocation_id + ",");
        // now add the rest
        Iterator<String> iter = video_allocation_ids.get(video_device_id).listener_allocation_ids.iterator();
        while (iter.hasNext()) {
            alloc_id_csv.append(iter.next() + ",");
        }
        // and get rid of the trailing comma
        if(alloc_id_csv.length() > 0){
            alloc_id_csv.setLength(alloc_id_csv.length()-1);
        }
        return alloc_id_csv.toString();
    }

    protected String getControlAllocationId(int video_device_id){
        return video_allocation_ids.get(video_device_id).control_allocation_id;
    }

    protected List<String> getListenerAllocationIds(int video_device_id){
        return video_allocation_ids.get(video_device_id).listener_allocation_ids;
    }

    /*****************************************************************/
    /* Allocation/Deallocation of Capacity                           */
    /*****************************************************************/
    // updateUsageState is not defined to return UsageType in Device base class
    public void updateUsageState() {
        getUsageState();
    }

    public CF.DevicePackage.UsageType getUsageState() {
        int videoAllocated = 0;
        for (int video_device_id = 0; video_device_id < video_allocation_ids.size(); video_device_id++) {
            if (!video_allocation_ids.get(video_device_id).control_allocation_id.isEmpty())
                videoAllocated++;
        }
        // If no video devices are allocated, device is idle
        if (videoAllocated == 0)
            return CF.DevicePackage.UsageType.IDLE;
        // If all video devices are allocated, device is busy
        if (videoAllocated == video_allocation_ids.size())
            return CF.DevicePackage.UsageType.BUSY;
        // Else, device is active
        return CF.DevicePackage.UsageType.ACTIVE;
    }

    public boolean allocateVideo(frontendX.FEXTypes.frontend_video_allocation_struct frontend_video_allocation) throws CF.DevicePackage.InvalidCapacity, Exception {
        try{
            // Check allocation_id
            if (frontend_video_allocation.allocation_id != null &&
                frontend_video_allocation.allocation_id.getValue().isEmpty()) {
                logger.info("allocateVideo: MISSING ALLOCATION_ID");
                throw new CF.DevicePackage.InvalidCapacity("MISSING ALLOCATION ID", new CF.DataType[]{new DataType("frontend_video_allocation", frontend_video_allocation.toAny())});
            }
            // Check if allocation ID has already been used
            if(this.getVideoDeviceMapping(frontend_video_allocation.allocation_id.getValue()) >= 0){
                logger.info("allocateVideo: ALLOCATION_ID ALREADY IN USE: [" + frontend_video_allocation.allocation_id.getValue() + "]");
                throw new InvalidCapacity("ALLOCATION_ID ALREADY IN USE", new CF.DataType[]{new DataType("frontend_video_allocation", frontend_video_allocation.toAny())});
            }
            // Check if available video device
            //synchronized(allocation_id_mapping_lock){
                // Next, try to allocate a new video
                for (int video_device_id = 0; video_device_id < this.video_allocation_ids.size(); video_device_id++) {
                    if(!frontend_video_status.getValue().get(video_device_id).video_type.getValue().equals(frontend_video_allocation.video_type.getValue())) {
                        logger.debug("allocateVideo: Requested video type '"+frontend_video_allocation.video_type.getValue() +"' does not match videoDevice[" + video_device_id + "].video_type (" + frontend_video_status.getValue().get(video_device_id).video_type.getValue()+")");
                        continue;
                    }
 
                    if(frontend_video_allocation.device_control.getValue()){
                        // device control
                        long orig_chan = frontend_video_status.getValue().get(video_device_id).channels.getValue();
                        long orig_fh = frontend_video_status.getValue().get(video_device_id).frame_height.getValue();
                        double orig_fr = frontend_video_status.getValue().get(video_device_id).fps.getValue();
                        long orig_fw = frontend_video_status.getValue().get(video_device_id).frame_width.getValue();
                        frontend_video_status.getValue().get(video_device_id).channels.setValue(frontend_video_allocation.channels.getValue());
                        frontend_video_status.getValue().get(video_device_id).frame_height.setValue(frontend_video_allocation.frame_height.getValue());
                        frontend_video_status.getValue().get(video_device_id).fps.setValue(frontend_video_allocation.fps.getValue());
                        frontend_video_status.getValue().get(video_device_id).frame_width.setValue(frontend_video_allocation.frame_width.getValue());
                        if(video_allocation_ids.get(video_device_id).control_allocation_id != null &&
                           (!video_allocation_ids.get(video_device_id).control_allocation_id.isEmpty() || 
                            !videoDeviceSetTuning(frontend_video_allocation, frontend_video_status.getValue().get(video_device_id), video_device_id))){
                            // either not available or didn't succeed setting tuning, try next video
                            if (frontend_video_status.getValue().get(video_device_id).channels.getValue().equals(frontend_video_allocation.channels.getValue()))
                                frontend_video_status.getValue().get(video_device_id).channels.setValue(orig_chan);
                            if (frontend_video_status.getValue().get(video_device_id).frame_height.getValue().equals(frontend_video_allocation.frame_height.getValue()))
                                frontend_video_status.getValue().get(video_device_id).frame_height.setValue(orig_fh);
                            if (frontend_video_status.getValue().get(video_device_id).fps.getValue().equals(frontend_video_allocation.fps.getValue()))
                                frontend_video_status.getValue().get(video_device_id).fps.setValue(orig_fr);
                            if (frontend_video_status.getValue().get(video_device_id).frame_width.getValue().equals(frontend_video_allocation.frame_width.getValue()))
                                frontend_video_status.getValue().get(video_device_id).frame_width.setValue(orig_fw);
                            logger.debug("allocateVideo: videoDevice["+video_device_id+"] is either not available or didn't succeed while setting tuning ");
                            continue;
                        }
                        video_allocation_ids.get(video_device_id).control_allocation_id = frontend_video_allocation.allocation_id.getValue();
                        allocation_id_to_video_device_id.put(frontend_video_allocation.allocation_id.getValue(), video_device_id);
                        frontend_video_status.getValue().get(video_device_id).allocation_id_csv.setValue(createAllocationIdCsv(video_device_id));
                    } else {
                        // listener
                        if(video_allocation_ids.get(video_device_id).control_allocation_id.isEmpty() || !listenerRequestValidation(frontend_video_allocation, video_device_id)){
                            // either not allocated or can't support listener request
                            logger.debug("allocateVideo: videoDevice["+video_device_id+"] is either not available or can not support listener request ");
                            continue;
                        }
                        video_allocation_ids.get(video_device_id).listener_allocation_ids.add(frontend_video_allocation.allocation_id.getValue());
                        allocation_id_to_video_device_id.put(frontend_video_allocation.allocation_id.getValue(), video_device_id);
                        frontend_video_status.getValue().get(video_device_id).allocation_id_csv.setValue(createAllocationIdCsv(video_device_id));
                        this.assignListener(frontend_video_allocation.allocation_id.getValue(),video_allocation_ids.get(video_device_id).control_allocation_id);
                    }
                    // if we've reached here, we found an eligible video device

                    // check tolerances
                    // only check when fps was not set to don't care)
                    logger.debug(" allocateVideo - FR requested: " + frontend_video_allocation.fps.getValue() + "  FR got: " + frontend_video_status.getValue().get(video_device_id).fps.getValue());
                    if( (floatingPointCompare(frontend_video_allocation.fps.getValue(),0)!=0) &&
                        (floatingPointCompare(frontend_video_status.getValue().get(video_device_id).fps.getValue(),frontend_video_allocation.fps.getValue())<0 ||
                        floatingPointCompare(frontend_video_status.getValue().get(video_device_id).fps.getValue(),frontend_video_allocation.fps.getValue()+frontend_video_allocation.fps.getValue() * frontend_video_allocation.fps_tolerance.getValue()/100.0)>0 )){
                        String eout = "allocateVideo(" + video_device_id + "): returned fr " + frontend_video_status.getValue().get(video_device_id).fps.getValue()+" does not meet tolerance criteria of " + frontend_video_allocation.fps_tolerance.getValue()+" percent";
                        logger.info(eout);
                        throw new RuntimeException(eout);
                    }

                    if(frontend_video_allocation.device_control.getValue()){
                        // enable video after successful allocation
                        try {
                            enableVideoDevice(video_device_id,true);
                        } catch(Exception e){
                            String eout = "allocateVideo: Failed to enable video after allocation";
                            logger.info(eout);
                            throw new RuntimeException(eout);
                        }
                    }
                    usageState = getUsageState();
                    return true;
                }
                // if we made it here, we failed to find an available video device
                String eout = "allocateVideo: NO AVAILABLE VIDEO DEVICE. Make sure that the device has an initialized frontend_video_status";
                logger.info(eout);
                throw new RuntimeException(eout);
            //}
        } catch(RuntimeException e) {
            //deallocateVideo(frontend_video_allocation);
            return false;
        } catch(CF.DevicePackage.InvalidCapacity e) {
            // without the following check, a valid allocation could be deallocated due to an attempt to alloc w/ an existing alloc id
            String exceptionMessage = e.getMessage();
            if (exceptionMessage != null && 
               exceptionMessage.indexOf("ALLOCATION_ID ALREADY IN USE") == -1){
               //deallocateVideo(frontend_video_allocation);
            }
            throw e;
        } catch(Exception e){
            //deallocateVideo(frontend_video_allocation);
            throw e; 
        }
    }

    public void deallocateVideo(frontendX.FEXTypes.frontend_video_allocation_struct frontend_video_deallocation){
        try{
            //logger.debug("deallocateVideo()");
            // Try to remove control of the device
            int video_device_id = this.getVideoDeviceMapping(frontend_video_deallocation.allocation_id.getValue());
            if (video_device_id < 0){
                logger.debug("ALLOCATION_ID NOT FOUND: [" + frontend_video_deallocation.allocation_id.getValue() +"]");
                throw new CF.DevicePackage.InvalidCapacity("ALLOCATION_ID NOT FOUND", new CF.DataType[]{new DataType("frontend_video_deallocation", frontend_video_deallocation.toAny())});
            }
            //logger.debug("deallocateVideo() video_device_id = " + video_device_id);
            if(video_allocation_ids.get(video_device_id).control_allocation_id.equals(frontend_video_deallocation.allocation_id.getValue())){
                //logger.debug("deallocateVideo() deallocating control for video_device_id = " + video_device_id);
                enableVideoDevice(video_device_id, false);
                frontend_video_status.getValue().get(video_device_id).allocation_id_csv.setValue("");
                removeVideoDeviceMapping(video_device_id);
                video_allocation_ids.get(video_device_id).control_allocation_id = "";
            }else{
                // send EOS to listener connection only
                removeVideoDeviceMapping(video_device_id,frontend_video_deallocation.allocation_id.getValue());
                frontend_video_status.getValue().get(video_device_id).allocation_id_csv.setValue(createAllocationIdCsv(video_device_id));
            }
        } catch (Exception e){
            System.out.println("deallocateVideo: ERROR WHEN DEALLOCATING.  SKIPPING...");
        }
        usageState = getUsageState();
    }

    public boolean allocateListener(frontend.FETypes.frontend_listener_allocation_struct frontend_listener_allocation) throws CF.DevicePackage.InvalidCapacity, Exception {
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
                if(getVideoDeviceMapping(frontend_listener_allocation.listener_allocation_id.getValue()) >= 0){
                    logger.error("allocateListener: LISTENER ALLOCATION_ID ALREADY IN USE");
                    throw new InvalidCapacity("LISTENER ALLOCATION_ID ALREADY IN USE", new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
                }

                // Do not allocate if existing allocation ID does not exist
                int video_device_id = getVideoDeviceMapping(frontend_listener_allocation.existing_allocation_id.getValue());
                if (video_device_id < 0){
                    logger.info("allocateListener: UNKNOWN CONTROL ALLOCATION ID: ["+ frontend_listener_allocation.existing_allocation_id.getValue() +"]");
                    throw new FRONTEND.BadParameterException("UNKNOWN CONTROL ALLOCATION ID");
                }

                // listener allocations are not permitted for playback
                if(frontend_video_status.getValue().get(video_device_id).video_type.getValue().equals("PLAYBACK")){
                    String eout = "allocateListener: listener allocations are not permitted for " + frontend_video_status.getValue().get(video_device_id).video_type.getValue() + " video type";
                    logger.debug(eout);
                    throw new CF.DevicePackage.InvalidCapacity(eout, new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
                }

                video_allocation_ids.get(video_device_id).listener_allocation_ids.add(frontend_listener_allocation.listener_allocation_id.getValue());
                allocation_id_to_video_device_id.put(frontend_listener_allocation.listener_allocation_id.getValue(), video_device_id);
                frontend_video_status.getValue().get(video_device_id).allocation_id_csv.setValue(createAllocationIdCsv(video_device_id));
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
        } catch (FRONTEND.BadParameterException e){
            return false;
        } catch (Exception e){
            throw e;
        }
    }

    public void deallocateListener(frontend.FETypes.frontend_listener_allocation_struct frontend_listener_allocation){
        try{
            int video_device_id = getVideoDeviceMapping(frontend_listener_allocation.listener_allocation_id.getValue());
            if (video_device_id < 0){
                logger.debug("ALLOCATION_ID NOT FOUND: [" + frontend_listener_allocation.listener_allocation_id.getValue() + "]");
                throw new CF.DevicePackage.InvalidCapacity("ALLOCATION_ID NOT FOUND", new CF.DataType[]{new DataType("frontend_listener_allocation", frontend_listener_allocation.toAny())});
            }
            // send EOS to listener connection only
            removeVideoDeviceMapping(video_device_id, frontend_listener_allocation.listener_allocation_id.getValue());
            frontend_video_status.getValue().get(video_device_id).allocation_id_csv.setValue(removeListenerId(video_device_id, frontend_listener_allocation.listener_allocation_id.getValue()));
        } catch (Exception e){
            logger.debug("deallocateListener: ERROR WHEN DEALLOCATING.  SKIPPING...");
        }
    }
    
    public String removeListenerId(final int video_device_id, final String allocation_id) {
        String[] split_id = frontend_video_status.getValue().get(video_device_id).allocation_id_csv.getValue().split(",");
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
    /* Video Configurations                                          */
    /*****************************************************************/
    // assumes collector RF and channel RF are the same if not true, override function 
    protected boolean enableVideoDevice(int video_device_id, boolean enable){
        boolean prev_enabled = frontend_video_status.getValue().get(video_device_id).enabled.getValue();
        // If going from disabled to enabled
        if (!prev_enabled && enable) {
            videoDeviceEnable(frontend_video_status.getValue().get(video_device_id), video_device_id);
        }

        // If going from enabled to disabled
        if (prev_enabled && !enable) {

            videoDeviceDisable(frontend_video_status.getValue().get(video_device_id), video_device_id);
        }

        return true;
    }

    protected boolean listenerRequestValidation(frontendX.FEXTypes.frontend_video_allocation_struct request, int video_device_id){
        logger.trace("listenerRequestValidation() video_device_id " + video_device_id);
        // ensure requested values are non-negative
        if(floatingPointCompare(request.fps.getValue(),0)<0 || floatingPointCompare(request.fps_tolerance.getValue(),0)<0)
            return false;

        // ensure video frame rate meets requested tolerance
        if( floatingPointCompare(request.fps.getValue(),frontend_video_status.getValue().get(video_device_id).fps.getValue()) > 0 )
            return false;

        if(floatingPointCompare(request.fps.getValue(),0)!=0 && floatingPointCompare((request.fps.getValue()+(request.fps.getValue()*request.fps_tolerance.getValue()/100)),frontend_video_status.getValue().get(video_device_id).fps.getValue()) < 0 )
            return false;

        return true;
    }

    ////////////////////////////
    //        MAPPING         //
    ////////////////////////////

    protected int getVideoDeviceMapping(String allocation_id){
        //logger.trace("getVideoDeviceMapping() allocation_id " + allocation_id);
        int NO_VALID_VIDEO_DEVICE = -1;

        if (allocation_id_to_video_device_id.containsKey(allocation_id)){
            return allocation_id_to_video_device_id.get(allocation_id);
        } 
        return NO_VALID_VIDEO_DEVICE;
    }

    protected boolean removeVideoDeviceMapping(int video_device_id, String allocation_id){
        logger.trace("removeVideoDeviceMapping() video_device_id " + video_device_id + ", allocation_id " + allocation_id);
        removeListener(allocation_id);
        Iterator<String> iter = video_allocation_ids.get(video_device_id).listener_allocation_ids.iterator();
        while(iter.hasNext()){
            String nextString = iter.next();
            if (nextString.equals(allocation_id)){
                iter.remove();
            }
        }
        //synchronized(allocation_id_mapping_lock){
            if (allocation_id_to_video_device_id.containsKey(allocation_id)){
                allocation_id_to_video_device_id.remove(allocation_id);
                return true;
            }
            return false;
        //}
    }

    protected boolean removeVideoDeviceMapping(int video_device_id){
        logger.trace("removeVideoDeviceMapping() video_device_id " + video_device_id);
        videoDeviceDeleteTuning(frontend_video_status.getValue().get(video_device_id),video_device_id);
        removeAllocationIdRouting(video_device_id);

        int cnt = 0;
        //synchronized(allocation_id_mapping_lock){
            if (allocation_id_to_video_device_id.containsValue(video_device_id)){
                Iterator<Map.Entry<String,Integer>> iter = allocation_id_to_video_device_id.entrySet().iterator();
                while (iter.hasNext()) {
                    Map.Entry<String,Integer> entry = iter.next();
                    if(video_device_id == entry.getValue()){
                        this.removeListener(entry.getKey());
                        iter.remove();
                        cnt++;
                    }
                }
            }
            video_allocation_ids.get(video_device_id).reset();
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

    /* Video Allocation IDs struct. This structure contains allocation tracking data.
     *
     */

    public class videoAllocationIdsStruct{
        public String control_allocation_id;
        public List<String> listener_allocation_ids;
        public videoAllocationIdsStruct(){
            control_allocation_id = new String();
            listener_allocation_ids = new ArrayList<String>();
            reset();
        }
        public void reset(){
            control_allocation_id = "";
            listener_allocation_ids.clear();
        }
    }

    // video_allocation_ids is exclusively paired with property frontend_video_status.
    // video_allocation_ids tracks allocation ids while frontend_video_status provides video information.
    protected List<videoAllocationIdsStruct> video_allocation_ids;

    protected StringProperty device_kind =
        new StringProperty(
            "DCE:cdc5ee18-7ceb-4ae6-bf4c-31f983179b4d", //id
            "device_kind", //name
            "FRONTEND.VIDEO", //default value
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

    protected StructProperty<frontendX.FEXTypes.frontend_video_allocation_struct> frontend_video_allocation =
        new StructProperty<frontendX.FEXTypes.frontend_video_allocation_struct>(
            "FRONTEND.video_allocation", //id
            "frontend_video_allocation", //name
            frontendX.FEXTypes.frontend_video_allocation_struct.class, //type
            new frontendX.FEXTypes.frontend_video_allocation_struct(), //default value
            Mode.READWRITE, //mode
            new Kind[] {Kind.ALLOCATION} //kind
            );

    protected StructProperty<frontend.FETypes.frontend_listener_allocation_struct> frontend_listener_allocation =
        new StructProperty<frontend.FETypes.frontend_listener_allocation_struct>(
            "FRONTEND.listener_allocation", //id
            "frontend_listener_allocation", //name
            frontend.FETypes.frontend_listener_allocation_struct.class, //type
            new frontend.FETypes.frontend_listener_allocation_struct(), //default value
            Mode.READWRITE, //mode
            new Kind[] {Kind.ALLOCATION} //kind
            );

    private Class<VideoStatusStructType> frontend_video_status_class_type;

    protected StructSequenceProperty<VideoStatusStructType> frontend_video_status =
        new StructSequenceProperty<VideoStatusStructType> (
            "FRONTEND.video_status", //id
            "frontend_video_status", //name
            frontend_video_status_class_type, //type
            new ArrayList<VideoStatusStructType>(),
            Mode.READONLY, //mode
            new Kind[] { Kind.CONFIGURE } //kind
        );

    protected Map<String, Integer> allocation_id_to_video_device_id;

    protected Object allocation_id_mapping_lock;

    ///////////////////////////////
    // Device specific functions // -- to be implemented by device developer
    ///////////////////////////////
    protected abstract void videoDeviceEnable(VideoStatusStructType fts, int video_device_id);
    protected abstract void videoDeviceDisable(VideoStatusStructType fts, int video_device_id);
    protected abstract boolean videoDeviceSetTuning(final frontendX.FEXTypes.frontend_video_allocation_struct request, VideoStatusStructType fts, int video_device_id);
    protected abstract boolean videoDeviceDeleteTuning(VideoStatusStructType fts, int video_device_id);

    ///////////////////////////////
    // Mapping and translation helpers. External string identifiers to internal numerical identifiers
    ///////////////////////////////
    protected abstract void removeAllocationIdRouting(final int video_device_id);

    ////////////////////////////
    // Other helper functions //
    ////////////////////////////
    protected BULKIO.StreamSRI create(String stream_id, VideoStatusStructType frontend_status) {
        return create(stream_id, frontend_status, -1.0);
    }

    protected BULKIO.StreamSRI create(String stream_id, VideoStatusStructType frontend_status, double collector_frequency) {
        BULKIO.StreamSRI sri = new BULKIO.StreamSRI();
        sri.hversion = 1;
        sri.xstart = 0.0;
        if ( frontend_status.fps.getValue() <= 0.0 )
            sri.xdelta =  1.0;
        else
            sri.xdelta = 1/(frontend_status.fps.getValue() * (frontend_status.frame_height.getValue() * frontend_status.frame_width.getValue() * frontend_status.channels.getValue()));
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
