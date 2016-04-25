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
#ifndef FEX_TYPES_H
#define FEX_TYPES_H

#include <ossie/CF/cf.h>
#include <vector>
#include <string>
#include <ossie/BULKIO/bulkioDataTypes.h>

namespace frontendX {
    
    struct frontend_audio_allocation_struct {
        frontend_audio_allocation_struct ()
        {
            full_bandwidth_channels = 0;
            low_frequency_effect_channels = 0;
            sample_rate = 0.0;
            sample_rate_tolerance = 10.0;
            device_control = true;
        }
        
        static std::string getId() {
            return std::string("FRONTEND::audio_allocation");
        }
        
        std::string audio_type;
        std::string allocation_id;
        unsigned short full_bandwidth_channels;
        unsigned short low_frequency_effect_channels;
        double sample_rate;
        double sample_rate_tolerance;
        bool device_control;
    };
    
    struct default_frontend_audio_status_struct_struct {
        default_frontend_audio_status_struct_struct ()
        {
            full_bandwidth_channels = 0;
            low_frequency_effect_channels = 0;
            sample_rate = 0.0;
            enabled = false;
        }
        
        static std::string getId() {
            return std::string("frontend_audio_status_struct");
        }
        
        std::string audio_type;
        std::string allocation_id_csv;
        unsigned short full_bandwidth_channels;
        unsigned short low_frequency_effect_channels;
        double sample_rate;
        bool enabled;
    };

    struct frontend_video_allocation_struct {
        frontend_video_allocation_struct ()
        {   
            channels = 0;
            frame_height = 0;
            frame_width = 0;
            fps = 0.0;
            fps_tolerance = 10.0;
            device_control = true;
        }

        static std::string getId() {
            return std::string("FRONTEND::video_allocation");
        }

        std::string allocation_id;
        CORBA::Long channels;
        CORBA::Long frame_height;
        CORBA::Long frame_width;
        double fps;
        double fps_tolerance;
        std::string video_type;
        bool device_control;
    };

    struct default_frontend_video_status_struct_struct {
        default_frontend_video_status_struct_struct ()
        {
            channels = 0;
            frame_height = 0;
            frame_width = 0;
            fps = 0.0;
            enabled = false;
        }

        static std::string getId() {
            return std::string("frontend_video_status_struct");
        }

        std::string allocation_id_csv;
        CORBA::Long channels;
        CORBA::Long frame_height;
        CORBA::Long frame_width;
        double fps;
        std::string video_type;
        bool enabled;
    };

}

#endif
