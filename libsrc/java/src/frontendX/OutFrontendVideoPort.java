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

import java.util.HashMap;
import java.util.Map;
import org.ossie.component.QueryableUsesPort;
import FRONTENDX.FrontendVideoHelper;
import FRONTENDX.FrontendVideoOperations;
import FRONTEND.FrontendException;
import FRONTEND.BadParameterException;
import FRONTEND.NotSupportedException;
import org.ossie.component.PortBase;

public class OutFrontendVideoPort extends QueryableUsesPort<FrontendVideoOperations> implements FrontendVideoOperations, PortBase {

    protected String name;

    protected Object updatingPortsLock;

    /**
     * Map of connection Ids to port objects
     */
    protected Map<String, FrontendVideoOperations> outConnections = new HashMap<String, FrontendVideoOperations>();

    public OutFrontendVideoPort(String portName) { 
        super(portName);
        this.name = portName;
        this.outConnections = new HashMap<String, FrontendVideoOperations>();
    }

    protected FrontendVideoOperations narrow(org.omg.CORBA.Object connection) {
        FrontendVideoOperations ops = FrontendVideoHelper.narrow(connection);
        return ops; 
    }

    public void connectPort(final org.omg.CORBA.Object connection, final String connectionId) throws CF.PortPackage.InvalidPort, CF.PortPackage.OccupiedPort {
        try {
            synchronized (this.updatingPortsLock) {
                super.connectPort(connection, connectionId);
                final FrontendVideoOperations port = FrontendVideoHelper.narrow(connection);
                this.outConnections.put(connectionId, port);
                this.active = true;
            }
        } catch (final Throwable t) {
            t.printStackTrace();
        }

    }

    public void disconnectPort(final String connectionId) {
        synchronized (this.updatingPortsLock) {
            super.disconnectPort(connectionId);
            this.outConnections.remove(connectionId);
            this.active = (this.outConnections.size() != 0);
        }
    }

    public String getVideoType(String id) throws FrontendException, BadParameterException, NotSupportedException {
        String retval = "";

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getVideoType(id);
                }
            }
        }
        return retval;
    }
 
    public boolean getVideoDeviceControl(String id) throws FrontendException, BadParameterException, NotSupportedException {
        boolean retval = false;

        synchronized(this.updatingPortsLock) { 
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getVideoDeviceControl(id);
                }
            }
        }
        return retval;
    }
 
    public int getChannels(String id) throws FrontendException, BadParameterException, NotSupportedException {
        int retval = 0;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getChannels(id);
                }
            }
        } 
        return retval;
    }
 
    public int getFrameHeight(String id) throws FrontendException, BadParameterException, NotSupportedException {
        int retval = 0;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getFrameHeight(id);
                }
            }
        } 
        
        return retval;
    }

    public int getFrameWidth(String id) throws FrontendException, BadParameterException, NotSupportedException {
        int retval = 0;

        synchronized(this.updatingPortsLock) {
            if (this.active) {

                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getFrameWidth(id);
                }
            }
        }

        return retval;
    }

    public void setVideoEnable(String id, boolean enable) throws FrontendException, BadParameterException, NotSupportedException {
        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    p.setVideoEnable(id, enable);
                }
            }
        }
    }

    public boolean getVideoEnable(String id) throws FrontendException, BadParameterException, NotSupportedException {
        boolean retval = false;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getVideoEnable(id);
                }
            }
        }
        return retval;
    }

    public void setVideoOutputFrameRate(String id, double fr) throws FrontendException, BadParameterException, NotSupportedException {
        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    p.setVideoOutputFrameRate(id, fr);
                }
            }
        }
    }

    public double getVideoOutputFrameRate(String id) throws FrontendException, BadParameterException, NotSupportedException {
        double retval = 0.0;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getVideoOutputFrameRate(id);
                }
            }
        }
        return retval;
    }

    public CF.DataType[] getVideoStatus(String id) throws FrontendException, BadParameterException, NotSupportedException {
        CF.DataType[] retval = null;

        synchronized(this.updatingPortsLock) { 
            if (this.active) {
                for (FrontendVideoOperations p : this.outConnections.values()) {
                    retval = p.getVideoStatus(id);
                }
            }
        }
        return retval;
    }

    public String getRepid() {
        return FrontendVideoHelper.id();
    }

    public String getDirection() {
        return "Uses";
    }
 }
