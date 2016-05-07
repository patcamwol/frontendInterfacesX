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

public interface FrontendVideoDelegate {

    public String getVideoType(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public boolean getVideoDeviceControl(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public int getChannels(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public int getFrameHeight(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public int getFrameWidth(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public void setVideoEnable(String id, boolean enable) throws FrontendException, BadParameterException, NotSupportedException;

    public boolean getVideoEnable(String id) throws FrontendException, BadParameterException, NotSupportedException;

    void setVideoOutputFrameRate(String id, double fr) throws FrontendException, BadParameterException, NotSupportedException;

    double getVideoOutputFrameRate(String id) throws FrontendException, BadParameterException, NotSupportedException;

    public CF.DataType[] getVideoStatus(String id) throws FrontendException, BadParameterException, NotSupportedException;

}
