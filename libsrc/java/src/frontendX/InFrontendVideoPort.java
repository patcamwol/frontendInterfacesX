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
import frontendX.FrontendVideoDelegate;
import FRONTENDX.FrontendVideoHelper;
import org.ossie.component.PortBase;

public class InFrontendVideoPort extends FRONTENDX.FrontendVideoPOA implements PortBase {

    protected String name;

    protected Object portAccess = null;

    protected FrontendVideoDelegate delegate = null;

    public InFrontendVideoPort( String portName) {
        this(portName, null);
    }

    public InFrontendVideoPort( String portName,
                                FrontendVideoDelegate d) {
        this.name = portName;
        this.delegate = d;
        this.portAccess = new Object();
    }


    public String getVideoType(String id) throws BadParameterException, FrontendException, NotSupportedException{
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getVideoType(id);
                } else {
                    throw new FrontendException("InFrontendVideoPort getVideoType(String id) callback delegate not defined");
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

    public boolean getVideoDeviceControl(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getVideoDeviceControl(id);
                } else {
                    throw new FrontendException("InFrontendVideoPort getVideoDeviceControl(String id) callback delegate not defined");
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

    public int getChannels(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getChannels(id);
                } else {
                    throw new FrontendException("InFrontendVideoPort getChannels(String id) callback delegate not defined");
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

    public int getFrameHeight(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getFrameHeight(id);
                } else {
                    throw new FrontendException("InFrontendVideoPort getFrameHeight(String id) callback delegate not defined");
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

    public int getFrameWidth(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getFrameWidth(id);
                } else {
                    throw new FrontendException("InFrontendVideoPort getFrameWidth(String id) callback delegate not defined");
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

    public void setVideoEnable(String id, boolean enable) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    delegate.setVideoEnable(id, enable);
                } else {
                    throw new FrontendException("InFrontendVideoPort setVideoEnable(String id, boolean enable) callback delegate not defined");
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

    public boolean getVideoEnable(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getVideoEnable(id);
                } else {
                    throw new FrontendException("InFrontendVideoPort getVideoEnable(String id) callback delegate not defined");
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

    public void setVideoOutputFrameRate(String id, double fr) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    delegate.setVideoOutputFrameRate(id, fr);
                } else {
                    throw new FrontendException("InDigitalVideoPort setVideoOutputFrameRate(String id, double fr) callback delegate not defined");
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

    public double getVideoOutputFrameRate(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getVideoOutputFrameRate(id);
                } else {
                    throw new FrontendException("InDigitalVideoPort getVideoOutputFrameRate(String id) callback delegate not defined");
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

    public CF.DataType[] getVideoStatus(String id) throws BadParameterException, FrontendException, NotSupportedException {
        synchronized(this.portAccess){
            try{
                if ( delegate != null ){
                    return delegate.getVideoStatus(id);
                } else {
                    throw new FrontendException("InFrontendVideoPort getVideoStatus(String id) callback delegate not defined");
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

    public void setDelegate( FrontendVideoDelegate d ) {
        delegate = d;
    }

    public String getRepid() {
        return FrontendVideoHelper.id();
    }

    public String getDirection() {
        return "Provides";
    }
}
