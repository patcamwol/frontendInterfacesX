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

import threading
from redhawk.frontendxInterfaces import FRONTENDX__POA
from redhawk.frontendxInterfaces import FRONTENDX
from redhawk.frontendInterfaces import FRONTEND
import copy


'''provides port(s)'''

class audio_delegation(object):
    def getAudioType(id):
        raise FRONTEND.NotSupportedException("getAudioType not supported")
    def getAudioDeviceControl(id):
        raise FRONTEND.NotSupportedException("getAudioDeviceControl not supported")
    def getFullBandwidthChannels(id):
        raise FRONTEND.NotSupportedException("getFullBandwidthChannels not supported")
    def getLowFrequencyEffectChannels(id):
        raise FRONTEND.NotSupportedException("getLowFrequencyEffectChannels not supported")
    def setAudioEnable(id, enable):
        raise FRONTEND.NotSupportedException("setAudioEnable not supported")
    def getAudioEnable(id):
        raise FRONTEND.NotSupportedException("getAudioEnable not supported")
    def setAudioOutputSampleRate(id, sr):
        raise FRONTEND.NotSupportedException("setAudioOutputSampleRate not supported")
    def getAudioOutputSampleRate(id):
        raise FRONTEND.NotSupportedException("getAudioOutputSampleRate not supported")
    def getAudioStatus(id):
        raise FRONTEND.NotSupportedException("getAudioStatus not supported")

class InFrontendAudioPort(FRONTEND__POA.FrontendAudio):
    def __init__(self, name, parent=audio_delegation()):
        self.name = name
        self.port_lock = threading.Lock()
        self.parent = parent

    def getAudioType(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getAudioType(id)
        finally:
            self.port_lock.release()

    def getAudioDeviceControl(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getAudioDeviceControl(id)
        finally:
            self.port_lock.release()

    def getFullBandwidthChannels(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getFullBandwidthChannels(id)
        finally:
            self.port_lock.release()

    def getLowFrequencyEffectChannels(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getLowFrequencyEffectChannels(id)
        finally:
            self.port_lock.release()

    def setAudioEnable(self, id, enable):
        self.port_lock.acquire()
        try:
            return self.parent.setAudioEnable(id,enable)
        finally:
            self.port_lock.release()

    def getAudioEnable(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getAudioEnable(id)
        finally:
            self.port_lock.release()

    def setAudioOutputSampleRate(self, id, sr):
        self.port_lock.acquire()
        try:
            return self.parent.setAudioOutputSampleRate(id,sr)
        finally:
            self.port_lock.release()

    def getAudioOutputSampleRate(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getAudioOutputSampleRate(id)
        finally:
            self.port_lock.release()

    def getAudioStatus(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getAudioStatus(id)
        finally:
            self.port_lock.release()

class video_delegation(object):
    def getVideoType(id):
        raise FRONTEND.NotSupportedException("getVideoType not supported")
    def getVideoDeviceControl(id):
        raise FRONTEND.NotSupportedException("getVideoDeviceControl not supported")
    def getChannels(id):
        raise FRONTEND.NotSupportedException("getChannels not supported")
    def getFrameHeight(id):
        raise FRONTEND.NotSupportedException("getFrameHeight not supported")
    def getFrameWidth(id):
        raise FRONTEND.NotSupportedException("getFrameWidth not supported") 
    def setVideoEnable(id, enable):
        raise FRONTEND.NotSupportedException("setVideoEnable not supported")
    def getVideoEnable(id):
        raise FRONTEND.NotSupportedException("getVideoEnable not supported")
    def setVideoOutputFrameRate(id, fr):
        raise FRONTEND.NotSupportedException("setVideoOutputFrameRate not supported")
    def getVideoOutputFrameRate(id):
        raise FRONTEND.NotSupportedException("getVideoOutputFrameRate not supported")
    def getVideoStatus(id):
        raise FRONTEND.NotSupportedException("getVideoStatus not supported")

class InFrontendVideoPort(FRONTEND__POA.FrontendVideo):
    def __init__(self, name, parent=audio_delegation()):
        self.name = name
        self.port_lock = threading.Lock()
        self.parent = parent

    def getVideoType(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getVideoType(id)
        finally:
            self.port_lock.release()

    def getVideoDeviceControl(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getVideoDeviceControl(id)
        finally:
            self.port_lock.release()

    def getChannels(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getChannels(id)
        finally:
            self.port_lock.release()

    def getFrameHeight(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getFrameHeight(id)
        finally:
            self.port_lock.release()

    def getFrameWidth(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getFrameWidth(id)
        finally:
            self.port_lock.release()

    def setVideoEnable(self, id, enable):
        self.port_lock.acquire()
        try:
            return self.parent.setVideoEnable(id,enable)
        finally:
            self.port_lock.release()

    def getVideoEnable(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getVideoEnable(id)
        finally:
            self.port_lock.release()

    def setVideoOutputFrameRate(self, id, fr):
        self.port_lock.acquire()
        try:
            return self.parent.setVideoOutputFrameRate(id,fr)
        finally:
            self.port_lock.release()

    def getVideoOutputFrameRate(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getVideoOutputFrameRate(id)
        finally:
            self.port_lock.release()

    def getVideoStatus(self, id):
        self.port_lock.acquire()
        try:
            return self.parent.getVideoStatus(id)
        finally:
            self.port_lock.release()

