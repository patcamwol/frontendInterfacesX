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
#ifndef FE_VIDEO_STRUCTPROPS_H
#define FE_VIDEO_STRUCTPROPS_H

#include <ossie/CorbaUtils.h>
#include <ossie/PropertyInterface.h>
#include <frontend/fe_types.h>
#include "fex_types.h"

inline bool operator>>= (const CORBA::Any& a, frontendX::frontend_video_allocation_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FRONTEND::video_allocation::video_type", props[idx].id)) {
            if (!(props[idx].value >>= s.video_type)) return false;
        }
        if (!strcmp("FRONTEND::video_allocation::allocation_id", props[idx].id)) {
            if (!(props[idx].value >>= s.allocation_id)) return false;
        }
        if (!strcmp("FRONTEND::video_allocation::channels", props[idx].id)) {
            if (!(props[idx].value >>= s.channels)) return false;
        }
        if (!strcmp("FRONTEND::video_allocation::frame_height", props[idx].id)) {
            if (!(props[idx].value >>= s.frame_height)) return false;
        }
        if (!strcmp("FRONTEND::video_allocation::frame_width", props[idx].id)) {
            if (!(props[idx].value >>= s.frame_width)) return false;
        }
        if (!strcmp("FRONTEND::video_allocation::fps", props[idx].id)) {
            if (!(props[idx].value >>= s.fps)) return false;
        }
        if (!strcmp("FRONTEND::video_allocation::fps_tolerance", props[idx].id)) {
            if (!(props[idx].value >>= s.fps_tolerance)) return false;
        }
        if (!strcmp("FRONTEND::video_allocation::device_control", props[idx].id)) {
            if (!(props[idx].value >>= s.device_control)) return false;
        }
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const frontendX::frontend_video_allocation_struct& s) {
    CF::Properties props;
    props.length(8);
    props[0].id = CORBA::string_dup("FRONTEND::video_allocation::video_type");
    props[0].value <<= s.video_type;
    props[1].id = CORBA::string_dup("FRONTEND::video_allocation::allocation_id");
    props[1].value <<= s.allocation_id;
    props[2].id = CORBA::string_dup("FRONTEND::video_allocation::channels");
    props[2].value <<= s.channels;
    props[3].id = CORBA::string_dup("FRONTEND::video_allocation::frame_height");
    props[3].value <<= s.frame_height;
    props[4].id = CORBA::string_dup("FRONTEND::video_allocation::frame_width");
    props[4].value <<= s.frame_width;
    props[5].id = CORBA::string_dup("FRONTEND::video_allocation::fps");
    props[5].value <<= s.fps;
    props[6].id = CORBA::string_dup("FRONTEND::video_allocation::fps_tolerance");
    props[6].value <<= s.fps_tolerance;
    props[7].id = CORBA::string_dup("FRONTEND::video_allocation::device_control");
    props[7].value <<= s.device_control;
    a <<= props;
}

inline bool operator== (const frontendX::frontend_video_allocation_struct& s1, const frontendX::frontend_video_allocation_struct& s2) {
    if (s1.video_type!=s2.video_type)
        return false;
    if (s1.allocation_id!=s2.allocation_id)
        return false;
    if (s1.channels!=s2.channels)
        return false;
    if (s1.frame_height!=s2.frame_height)
        return false;
    if (s1.frame_width!=s2.frame_width)
        return false;
    if (s1.fps!=s2.fps)
        return false;
    if (s1.fps_tolerance!=s2.fps_tolerance)
        return false;
    if (s1.device_control!=s2.device_control)
        return false;
    return true;
}

inline bool operator!= (const frontendX::frontend_video_allocation_struct& s1, const frontendX::frontend_video_allocation_struct& s2) {
    return !(s1==s2);
}

template<> inline short StructProperty<frontendX::frontend_video_allocation_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        CORBA::TypeCode_var aType = a.type();
        if (aType->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    frontendX::frontend_video_allocation_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

inline bool operator>>= (const CORBA::Any& a, frontendX::default_frontend_video_status_struct_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FRONTEND::video_status::video_type", props[idx].id)) {
            if (!(props[idx].value >>= s.video_type)) return false;
        }
        if (!strcmp("FRONTEND::video_status::allocation_id_csv", props[idx].id)) {
            if (!(props[idx].value >>= s.allocation_id_csv)) return false;
        }
        if (!strcmp("FRONTEND::video_status::channels", props[idx].id)) {
            if (!(props[idx].value >>= s.channels)) return false;
        }
        if (!strcmp("FRONTEND::video_status::enabled", props[idx].id)) {
            if (!(props[idx].value >>= s.enabled)) return false;
        }
        if (!strcmp("FRONTEND::video_status::frame_height", props[idx].id)) {
            if (!(props[idx].value >>= s.frame_height)) return false;
        }
        if (!strcmp("FRONTEND::video_status::frame_width", props[idx].id)) {
            if (!(props[idx].value >>= s.frame_width)) return false;
        }
        if (!strcmp("FRONTEND::video_status::fps", props[idx].id)) {
            if (!(props[idx].value >>= s.fps)) return false;
        }
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const frontendX::default_frontend_video_status_struct_struct& s) {
    CF::Properties props;
    props.length(7);
    props[0].id = CORBA::string_dup("FRONTEND::video_status::video_type");
    props[0].value <<= s.video_type;
    props[1].id = CORBA::string_dup("FRONTEND::video_status::allocation_id_csv");
    props[1].value <<= s.allocation_id_csv;
    props[2].id = CORBA::string_dup("FRONTEND::video_status::channels");
    props[2].value <<= s.channels;
    props[3].id = CORBA::string_dup("FRONTEND::video_status::enabled");
    props[3].value <<= s.enabled;
    props[4].id = CORBA::string_dup("FRONTEND::video_status::frame_height");
    props[4].value <<= s.frame_height;
    props[5].id = CORBA::string_dup("FRONTEND::video_status::frame_width");
    props[5].value <<= s.frame_width;
    props[6].id = CORBA::string_dup("FRONTEND::video_status::fps");
    props[6].value <<= s.fps;
    a <<= props;
}

inline bool operator== (const frontendX::default_frontend_video_status_struct_struct& s1, const frontendX::default_frontend_video_status_struct_struct& s2) {
    if (s1.video_type!=s2.video_type)
        return false;
    if (s1.allocation_id_csv!=s2.allocation_id_csv)
        return false;
    if (s1.channels!=s2.channels)
        return false;
    if (s1.enabled!=s2.enabled)
        return false;
    if (s1.frame_height!=s2.frame_height)
        return false;
    if (s1.frame_width!=s2.frame_width)
        return false;
    if (s1.fps!=s2.fps)
        return false;
    return true;
}

inline bool operator!= (const frontendX::default_frontend_video_status_struct_struct& s1, const frontendX::default_frontend_video_status_struct_struct& s2) {
    return !(s1==s2);
}

template<> inline short StructProperty<frontendX::default_frontend_video_status_struct_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        CORBA::TypeCode_var aType = a.type();
        if (aType->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    frontendX::default_frontend_video_status_struct_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

inline bool operator== (const std::vector<frontendX::default_frontend_video_status_struct_struct>& s1, const std::vector<frontendX::default_frontend_video_status_struct_struct>& s2) {
    if (s1.size() != s2.size()) {
        return false;
    }
    for (unsigned int i=0; i<s1.size(); i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

inline bool operator!= (const std::vector<frontendX::default_frontend_video_status_struct_struct>& s1, const std::vector<frontendX::default_frontend_video_status_struct_struct>& s2) {
    return !(s1==s2);
}

template<> inline short StructSequenceProperty<frontendX::default_frontend_video_status_struct_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        CORBA::TypeCode_var aType = a.type();
        if (aType->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    std::vector<frontendX::default_frontend_video_status_struct_struct> tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

#endif
