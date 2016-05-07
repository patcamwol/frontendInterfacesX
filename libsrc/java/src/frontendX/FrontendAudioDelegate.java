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

public interface FrontendAudioDelegate {

    public String getAudioType(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public boolean getAudioDeviceControl(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public short getFullBandwidthChannels(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public short getLowFrequencyEffectChannels(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public void setAudioEnable(String id, boolean enable) throws FrontendException, BadParameterException, NotSupportedException;

    public boolean getAudioEnable(String id) throws FrontendException, BadParameterException, NotSupportedException;

    void setAudioOutputSampleRate(String id, double sr) throws FrontendException, BadParameterException, NotSupportedException;

    double getAudioOutputSampleRate(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public CF.DataType[] getAudioStatus(String id) throws FrontendException, BadParameterException, NotSupportedException;

}
