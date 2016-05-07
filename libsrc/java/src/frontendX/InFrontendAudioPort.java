/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK frontendInterfaces.
 *
 * REDHAWK frontendInterfaces is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK frontendInterfaces is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
package frontendX;

import FRONTEND.FrontendException;
import FRONTEND.BadParameterException;
import FRONTEND.NotSupportedException;
import frontendX.FrontendAudioDelegate;
import FRONTENDX.FrontendAudioHelper;
import org.ossie.component.PortBase;

public class InFrontendAudioPort extends FRONTENDX.FrontendAudioPOA implements PortBase {

    protected String name;

    protected Object portAccess = null;

    protected FrontendAudioDelegate delegate = null;

    public InFrontendAudioPort( String portName) {
        this(portName, null);
    }

    public InFrontendAudioPort( String portName,
                                FrontendAudioDelegate d) {
        this.name = portName;
        this.delegate = d;
        this.portAccess = new Object();
    }


    public String getAudioType(String id) throws BadParameterException, FrontendException, NotSupportedException{
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getAudioType(id);
                } else {
                    throw new FrontendException("InFrontendAudioPort getAudioType(String id) callback delegate not defined");
                }
            }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public boolean getAudioDeviceControl(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getAudioDeviceControl(id);
                } else {
                    throw new FrontendException("InFrontendAudioPort getAudioDeviceControl(String id) callback delegate not defined");
                }
            }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public short getFullBandwidthChannels(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getFullBandwidthChannels(id);
                } else {
                    throw new FrontendException("InFrontendAudioPort getFullBandwidthChannels(String id) callback delegate not defined");
                }
            }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public short getLowFrequencyEffectChannels(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getLowFrequencyEffectChannels(id);
                } else {
                    throw new FrontendException("InFrontendAudioPort getLowFrequencyEffectChannels(String id) callback delegate not defined");
                }
            }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public void setAudioEnable(String id, boolean enable) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    delegate.setAudioEnable(id, enable);
                } else {
                    throw new FrontendException("InFrontendAudioPort setAudioEnable(String id, boolean enable) callback delegate not defined");
                }
            }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public boolean getAudioEnable(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getAudioEnable(id);
                } else {
                    throw new FrontendException("InFrontendAudioPort getAudioEnable(String id) callback delegate not defined");
                }
            }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public void setAudioOutputSampleRate(String id, double sr) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    delegate.setAudioOutputSampleRate(id, sr);
                } else {
                    throw new FrontendException("InDigitalAudioPort setAudioOutputSampleRate(String id, double sr) callback delegate not defined");
                }
           }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public double getAudioOutputSampleRate(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getAudioOutputSampleRate(id);
                } else {
                    throw new FrontendException("InDigitalAudioPort getAudioOutputSampleRate(String id) callback delegate not defined");
                }
           }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public CF.DataType[] getAudioStatus(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getAudioStatus(id);
                } else {
                    throw new FrontendException("InFrontendAudioPort getAudioStatus(String id) callback delegate not defined");
                }
            }catch(BadParameterException e){
                throw e;
            }catch(FrontendException e){
                throw e;
            }catch(NotSupportedException e){
                throw e;
            }
        }
    }

    public void setDelegate( FrontendAudioDelegate d ) {
        delegate = d;
    }

    public String getRepid() {
        return FrontendAudioHelper.id();
    }

    public String getDirection() {
        return "Provides";
    }
}
