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
import FRONTENDX.FrontendAudioHelper;
import FRONTENDX.FrontendAudioOperations;
import FRONTEND.FrontendException;
import FRONTEND.BadParameterException;
import FRONTEND.NotSupportedException;
import org.ossie.component.PortBase;

public class OutFrontendAudioPort extends QueryableUsesPort<FrontendAudioOperations> implements FrontendAudioOperations, PortBase {

    protected String name;

    protected Object updatingPortsLock;

    /**
     * Map of connection Ids to port objects
     */
    protected Map<String, FrontendAudioOperations> outConnections = new HashMap<String, FrontendAudioOperations>();

    public OutFrontendAudioPort(String portName) { 
        super(portName);
        this.name = portName;
        this.outConnections = new HashMap<String, FrontendAudioOperations>();
    }

    protected FrontendAudioOperations narrow(org.omg.CORBA.Object connection) {
        FrontendAudioOperations ops = FrontendAudioHelper.narrow(connection);
        return ops; 
    }

    public void connectPort(final org.omg.CORBA.Object connection, final String connectionId) throws CF.PortPackage.InvalidPort, CF.PortPackage.OccupiedPort {
        try {
            synchronized (this.updatingPortsLock) {
                super.connectPort(connection, connectionId);
                final FrontendAudioOperations port = FrontendAudioHelper.narrow(connection);
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

    public String getAudioType(String id) throws FrontendException, BadParameterException, NotSupportedException {
        String retval = "";

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    retval = p.getAudioType(id);
                }
            }
        }
        return retval;
    }
 
    public boolean getAudioDeviceControl(String id) throws FrontendException, BadParameterException, NotSupportedException {
        boolean retval = false;

        synchronized(this.updatingPortsLock) { 
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    retval = p.getAudioDeviceControl(id);
                }
            }
        }
        return retval;
    }
 
    public short getFullBandwidthChannels(String id) throws FrontendException, BadParameterException, NotSupportedException {
        short retval = 0;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    retval = p.getFullBandwidthChannels(id);
                }
            }
        } 
        return retval;
    }
 
    public short getLowFrequencyEffectChannels(String id) throws FrontendException, BadParameterException, NotSupportedException {
        short retval = 0;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    retval = p.getLowFrequencyEffectChannels(id);
                }
            }
        } 
        
        return retval;
    }

    public void setAudioEnable(String id, boolean enable) throws FrontendException, BadParameterException, NotSupportedException {
        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    p.setAudioEnable(id, enable);
                }
            }
        }
    }

    public boolean getAudioEnable(String id) throws FrontendException, BadParameterException, NotSupportedException {
        boolean retval = false;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    retval = p.getAudioEnable(id);
                }
            }
        }
        return retval;
    }

    public void setAudioOutputSampleRate(String id, double sr) throws FrontendException, BadParameterException, NotSupportedException {
        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    p.setAudioOutputSampleRate(id, sr);
                }
            }
        }
    }

    public double getAudioOutputSampleRate(String id) throws FrontendException, BadParameterException, NotSupportedException {
        double retval = 0.0;

        synchronized(this.updatingPortsLock) {
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    retval = p.getAudioOutputSampleRate(id);
                }
            }
        }
        return retval;
    }

    public CF.DataType[] getAudioStatus(String id) throws FrontendException, BadParameterException, NotSupportedException {
        CF.DataType[] retval = null;

        synchronized(this.updatingPortsLock) { 
            if (this.active) {
                for (FrontendAudioOperations p : this.outConnections.values()) {
                    retval = p.getAudioStatus(id);
                }
            }
        }
        return retval;
    }

    public String getRepid() {
        return FrontendAudioHelper.id();
    }

    public String getDirection() {
        return "Uses";
    }
 }
