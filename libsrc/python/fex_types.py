#
# This file is protected by Copyright. Please refer to the COPYRIGHT file
# distributed with this source distribution.
#
# This file is part of Frontend Interfaces X.
#
# Frontend Interfaces X is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# Frontend Interfaces X is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.
#

import bulkio
from frontend import fe_types
from ossie.cf import CF
from ossie.properties import simple_property
from omniORB import any as _any

class frontend_audio_allocation(object):
    audio_type = simple_property(id_="FRONTEND::audio_allocation::audio_type",
                                     name="audio_type",
                                     type_="string",
                                     )
    allocation_id = simple_property(id_="FRONTEND::audio_allocation::allocation_id",
                                    name="allocation_id",
                                    type_="string",
                                    )
    sample_rate = simple_property(id_="FRONTEND::audio_allocation::sample_rate",
                                       name="sample_rate",
                                       type_="double",
                                       )
    sample_rate_tolerance = simple_property(id_="FRONTEND::audio_allocation::sample_rate_tolerance",
                                name="sample_rate_tolerance",
                                type_="double",
                                )
    device_control = simple_property(id_="FRONTEND::audio_allocation::device_control",
                                          name="device_control",
                                          type_="boolean",
                                          defvalue=True,
                                          )
    full_bandwidth_channels = simple_property(id_="FRONTEND::audio_allocation::full_bandwidth_channels",
                                  name="full_bandwidth_channels",
                                  type_="unsigned short",
                                  defvalue=0,
                                  )
    low_frequency_effect_channels = simple_property(id_="FRONTEND::audio_allocation::low_frequency_effect_channels",
                                           name="low_frequency_effect_channels",
                                            type_="unsigned short",
                                            defvalue=0,
                                            )
    
    def __init__(self, **kw):
        """Construct an initialized instance of this struct definition"""
        for attrname, classattr in type(self).__dict__.items():
            if type(classattr) == simple_property:
                classattr.initialize(self)
        for k,v in kw.items():
            setattr(self,k,v)

    def __str__(self):
        """Return a string representation of this structure"""
        d = {}
        d["audio_type"] = self.audio_type
        d["allocation_id"] = self.allocation_id
        d["sample_rate"] = self.sample_rate
        d["sample_rate_tolerance"] = self.sample_rate_tolerance
        d["device_control"] = self.device_control
        d["full_bandwidth_channels"] = self.full_bandwidth_channels
        d["low_frequency_effect_channels"] = self.low_frequency_effect_channels
        return str(d)

    def getId(self):
        return "FRONTEND::audio_allocation"

    def isStruct(self):
        return True
    
    def getProp(self):
        content = []
        for member in self.getMembers():
            content.append(CF.DataType(id=member[0],value=_any.to_any(member[1])))
        retval = CF.DataType(id=self.getId(),value=_any.to_any(content))
        return retval
                                                            
    def getMembers(self):
        return [("FRONTEND::audio_allocation::audio_type",self.audio_type),("FRONTEND::audio_allocation::allocation_id",self.allocation_id),("FRONTEND::audio_allocation::sample_rate",self.sample_rate),("FRONTEND::audio_allocation::sample_rate_tolerance",self.sample_rate_tolerance),("FRONTEND::audio_allocation::device_control",self.device_control),("FRONTEND::audio_allocation::full_bandwidth_channels",self.full_bandwidth_channels),("FRONTEND::audio_allocation::low_frequency_effect_channels",self.low_frequency_effect_channels)]

class default_frontend_audio_status_struct_struct(object):
    audio_type = simple_property(id_="FRONTEND::audio_status::audio_type",
                                 name="audio_type",
                                 type_="string",
                                 )
    allocation_id_csv = simple_property(id_="FRONTEND::audio_status::allocation_id_csv",
                                        name="allocation_id_csv",
                                        type_="string",
                                        )
    full_bandwidth_channels = simple_property(id_="FRONTEND::audio_status::full_bandwidth_channels",
                                       name="full_bandwidth_channels",
                                       type_="unsigned short",
                                       )
    low_frequency_effect_channels = simple_property(id_="FRONTEND::audio_status::low_frequency_effect_channels",
                                name="low_frequency_effect_channels",
                                type_="unsigned short",
                                )
    sample_rate = simple_property(id_="FRONTEND::audio_status::sample_rate",
                                  name="sample_rate",
                                  type_="double",
                                  )
    enabled = simple_property(id_="FRONTEND::audio_status::enabled",
                              name="enabled",
                              type_="boolean",
                              )

    def __init__(self, audio_type="", allocation_id_csv="", full_bandwidth_channels=0, low_frequency_effect_channels=0, sample_rate=0.0, enabled=False):
        self.audio_type = audio_type
        self.allocation_id_csv = allocation_id_csv
        self.full_bandwidth_channels = full_bandwidth_channels
        self.low_frequency_effect_channels = low_frequency_effect_channels
        self.sample_rate = sample_rate
        self.enabled = enabled

    def __str__(self):
        """Return a string representation of this structure"""
        d = {}
        d["audio_type"] = self.audio_type
        d["allocation_id_csv"] = self.allocation_id_csv
        d["full_bandwidth_channels"] = self.full_bandwidth_channels
        d["low_frequency_effect_channels"] = self.low_frequency_effect_channels
        d["sample_rate"] = self.sample_rate
        d["enabled"] = self.enabled
        return str(d)

    def getId(self):
        return "frontend_audio_status_struct"

    def isStruct(self):
        return True
    
    def getProp(self):
        content = []
        for member in self.getMembers():
            content.append(CF.DataType(id=member[0],value=_any.to_any(member[1])))
        retval = CF.DataType(id=self.getId(),value=_any.to_any(content))
        return retval

    def getMembers(self):
        return [("FRONTEND::audio_status::audio_type",self.audio_type),("FRONTEND::audio_status::allocation_id_csv",self.allocation_id_csv),("FRONTEND::audio_status::full_bandwidth_channels",self.full_bandwidth_channels),("FRONTEND::audio_status::low_frequency_effect_channels",self.low_frequency_effect_channels),("FRONTEND::audio_status::sample_rate",self.sample_rate),("FRONTEND::audio_status::enabled",self.enabled)]

class frontend_video_allocation(object):
    video_type = simple_property(id_="FRONTEND::video_allocation::video_type",
                                     name="video_type",
                                     type_="string",
                                     )
    allocation_id = simple_property(id_="FRONTEND::video_allocation::allocation_id",
                                    name="allocation_id",
                                    type_="string",
                                    )
    channels = simple_property(id_="FRONTEND::video_allocation::channels",
                                       name="channels",
                                       type_="long",
                                       defvalue=0
                                       )
    frame_height = simple_property(id_="FRONTEND::video_allocation::frame_height",
                                name="frame_height",
                                type_="long",
                                defvalue=0,
                                )
    frame_width = simple_property(id_="FRONTEND::video_allocation::frame_width",
                                name="frame_width",
                                type_="long",
                                defvalue=0,
                                )
    fps = simple_property(id_="FRONTEND::video_allocation::fps",
                                name="fps",
                                type_="double",
                                defvalue=0.0,
                                )
    fps_tolerance = simple_property(id_="FRONTEND::video_allocation::fps_tolerance",
                                name="fps_tolerance",
                                type_="double",
                                defvalue=10.0,
                                )
    device_control = simple_property(id_="FRONTEND::video_allocation::device_control",
                                          name="device_control",
                                          type_="boolean",
                                          defvalue=True,
                                          )
    
    def __init__(self, **kw):
        """Construct an initialized instance of this struct definition"""
        for attrname, classattr in type(self).__dict__.items():
            if type(classattr) == simple_property:
                classattr.initialize(self)
        for k,v in kw.items():
            setattr(self,k,v)

    def __str__(self):
        """Return a string representation of this structure"""
        d = {}
        d["video_type"] = self.video_type
        d["allocation_id"] = self.allocation_id
        d["channels"] = self.channels
        d["frame_height"] = self.frame_height
        d["frame_width"] = self.frame_width
        d["fps"] = self.fps
        d["fps_tolerance"] = self.fps_tolerance
        d["device_control"] = self.device_control
        return str(d)

    def getId(self):
        return "FRONTEND::video_allocation"

    def isStruct(self):
        return True
    
    def getProp(self):
        content = []
        for member in self.getMembers():
            content.append(CF.DataType(id=member[0],value=_any.to_any(member[1])))
        retval = CF.DataType(id=self.getId(),value=_any.to_any(content))
        return retval
                                                            
    def getMembers(self):
        return [("FRONTEND::video_allocation::video_type",self.video_type),("FRONTEND::video_allocation::allocation_id",self.allocation_id),("FRONTEND::video_allocation::channels",self.channels),("FRONTEND::video_allocation::frame_height",self.frame_height),("FRONTEND::video_allocation::frame_width",self.frame_width),("FRONTEND::video_allocation::fps",self.fps),("FRONTEND::video_allocation::fps_tolerance",self.fps_tolerance),("FRONTEND::video_allocation::device_control",self.device_control)]

class default_frontend_video_status_struct_struct(object):
    video_type = simple_property(id_="FRONTEND::video_status::video_type",
                                 name="video_type",
                                 type_="string",
                                 )
    allocation_id_csv = simple_property(id_="FRONTEND::video_status::allocation_id_csv",
                                        name="allocation_id_csv",
                                        type_="string",
                                        )
    channels = simple_property(id_="FRONTEND::video_status::channels",
                                       name="channels",
                                       type_="long",
                                       defvalue=0,
                                       )
    frame_height = simple_property(id_="FRONTEND::video_status::frame_height",
                                  name="frame_height",
                                  type_="long",
                                  defvalue=0,
                                  )
    frame_width = simple_property(id_="FRONTEND::vido_status::frame_width",
                              name="frame_width",
                              type_="long",
                              defvalue=0,
                              )
    fps = simple_property(id_="FRONTEND::vido_status::fps",
                              name="fps",
                              type_="double",
                              defvalue=0.0,
                              )
    enabled = simple_property(id_="FRONTEND::vido_status::enabled",
                              name="enabled",
                              type_="boolean",
                              defvalue=False,
                              )

    def __init__(self, video_type="", allocation_id_csv="", channels=0, frame_height=0, frame_width=0, fps=0.0, enabled=False):
        self.video_type = video_type
        self.allocation_id_csv = allocation_id_csv
        self.channels = channels
        self.frame_height = frame_height
        self.frame_width = frame_width
        self.fps = fps
        self.enabled = enabled

    def __str__(self):
        """Return a string representation of this structure"""
        d = {}
        d["video_type"] = self.video_type
        d["allocation_id_csv"] = self.allocation_id_csv
        d["channels"] = self.channels
        d["frame_height"] = self.frame_height
        d["frame_width"] = self.frame_width
        d["fps"] = self.fps
        d["enabled"] = self.enabled
        return str(d)

    def getId(self):
        return "frontend_video_status_struct"

    def isStruct(self):
        return True
    
    def getProp(self):
        content = []
        for member in self.getMembers():
            content.append(CF.DataType(id=member[0],value=_any.to_any(member[1])))
        retval = CF.DataType(id=self.getId(),value=_any.to_any(content))
        return retval

    def getMembers(self):
        return [("FRONTEND::video_status::video_type",self.video_type),("FRONTEND::video_status::allocation_id_csv",self.allocation_id_csv),("FRONTEND::video_status::channels",self.channels),("FRONTEND::video_status::frame_height",self.frame_height),("FRONTEND::video_status::frame_width",self.frame_width),("FRONTEND::video_status::fps",self.fps)]
