#
# This file is protected by Copyright. Please refer to the COPYRIGHT file
# distributed with this source distribution.
#
# This file is part of REDHAWK frontendInterfaces.
#
# REDHAWK frontendInterfaces is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# REDHAWK frontendInterfaces is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.
#

from ossie.cf import CF, CF__POA, ExtendedCF
from ossie.cf.CF import Port
from ossie.utils import uuid

import threading
from redhawk.frontendInterfaces import FRONTEND
from redhawk.frontendxInterfaces import FRONTENDX


class OutPort (CF__POA.Port ):
    def __init__(self, name, PortTypeClass):
        self.name = name
        self.PortType = PortTypeClass
        self.outConnections = {} # key=connectionId,  value=port
        self.port_lock = threading.Lock()

    def connectPort(self, connection, connectionId):
        self.port_lock.acquire()
        try:
           try:
              port = connection._narrow(self.PortType)
              self.outConnections[str(connectionId)] = port
              self.refreshSRI = True
           except:
              raise Port.InvalidPort(1, "Invalid Port for Connection ID:" + str(connectionId) )
        finally:
            self.port_lock.release()

    def disconnectPort(self, connectionId):
        self.port_lock.acquire()
        try:
            self.outConnections.pop(str(connectionId), None)
        finally:
            self.port_lock.release()

    def _get_connections(self):
        currentConnections = []
        self.port_lock.acquire()
        for id_, port in self.outConnections.items():
            currentConnections.append(ExtendedCF.UsesConnection(id_, port))
        self.port_lock.release()
        return currentConnections

class OutFrontendAudioPort(OutPort):
    def __init__(self, name):
        OutPort.__init__(self, name, FRONTENDX.FrontendAudio)

    def getAudioType(self, id):
        retVal = ""
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getAudioType(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()
            
        return retVal

    def getAudioDeviceControl(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getAudioDeviceControl(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()
            
        return retVal

    def getFullBandwidthChannels(self, id):
        retVal = 0
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getFullBandwidthChannels(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()
            
        return retVal

    def getLowFrequencyEffectChannels(self, id):
        retVal = 0
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getLowFrequencyEffectChannels(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()
            
        return retVal

    def setAudioEnable(self, id, enable):
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        port.setAudioEnable(id, enable)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

    def getAudioEnable(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getAudioEnable(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def setAudioOutputSampleRate(self, id, sr):
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        port.setAudioOutputSampleRate(id, sr)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

    def getAudioOutputSampleRate(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getAudioOutputSampleRate(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def getAudioStatus(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getAudioStatus(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()
            
        return retVal

class OutFrontendVideoPort(OutPort):
    def __init__(self, name):
        OutPort.__init__(self, name, FRONTENDX.FrontendVideo)

    def getVideoType(self, id):
        retVal = ""
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getVideoType(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def getVideoDeviceControl(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getVideoDeviceControl(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def getChannels(self, id):
        retVal = 0
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getChannels(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def getFrameHeight(self, id):
        retVal = 0
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getFrameHeight(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def getFrameWidth(self, id):
        retVal = 0
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getFrameWidth(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def setVideoEnable(self, id, enable):
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        port.setVideoEnable(id, enable)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

    def getVideoEnable(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getVideoEnable(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def setVideoOutputFrameRate(self, id, fr):
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        port.setVideoOutputFrameRate(id, fr)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

    def getVideoOutputFrameRate(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getVideoOutputFrameRate(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

    def getVideoStatus(self, id):
        retVal = None
        self.port_lock.acquire()

        try:
            for connId, port in self.outConnections.items():
                if port != None:
                    try:
                        retVal = port.getVideoStatus(id)
                    except Exception:
                        pass
        finally:
            self.port_lock.release()

        return retVal

