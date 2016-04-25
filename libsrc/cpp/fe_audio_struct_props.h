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
#ifndef FE_AUDIO_STRUCTPROPS_H
#define FE_AUDIO_STRUCTPROPS_H

#include <ossie/CorbaUtils.h>
#include <ossie/PropertyInterface.h>
#include <frontend/fe_types.h>
#include "fex_types.h"

inline bool operator>>= (const CORBA::Any& a, frontendX::frontend_audio_allocation_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FRONTEND::audio_allocation::audio_type", props[idx].id)) {
            if (!(props[idx].value >>= s.audio_type)) return false;
        }
        if (!strcmp("FRONTEND::audio_allocation::allocation_id", props[idx].id)) {
            if (!(props[idx].value >>= s.allocation_id)) return false;
        }
        if (!strcmp("FRONTEND::audio_allocation::sample_rate", props[idx].id)) {
            if (!(props[idx].value >>= s.sample_rate)) return false;
        }
        if (!strcmp("FRONTEND::audio_allocation::sample_rate_tolerance", props[idx].id)) {
            if (!(props[idx].value >>= s.sample_rate_tolerance)) return false;
        }
        if (!strcmp("FRONTEND::audio_allocation::device_control", props[idx].id)) {
            if (!(props[idx].value >>= s.device_control)) return false;
        }
        if (!strcmp("FRONTEND::audio_allocation::full_bandwidth_channels", props[idx].id)) {
            if (!(props[idx].value >>= s.full_bandwidth_channels)) return false;
        }
        if (!strcmp("FRONTEND::audio_allocation::low_frequency_effect_channels", props[idx].id)) {
            if (!(props[idx].value >>= s.low_frequency_effect_channels)) return false;
        }

    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const frontendX::frontend_audio_allocation_struct& s) {
    CF::Properties props;
    props.length(7);
    props[0].id = CORBA::string_dup("FRONTEND::audio_allocation::audio_type");
    props[0].value <<= s.audio_type;
    props[1].id = CORBA::string_dup("FRONTEND::audio_allocation::allocation_id");
    props[1].value <<= s.allocation_id;
    props[2].id = CORBA::string_dup("FRONTEND::audio_allocation::sample_rate");
    props[2].value <<= s.sample_rate;
    props[3].id = CORBA::string_dup("FRONTEND::audio_allocation::sample_rate_tolerance");
    props[3].value <<= s.sample_rate_tolerance;
    props[4].id = CORBA::string_dup("FRONTEND::audio_allocation::device_control");
    props[4].value <<= s.device_control;
    props[5].id = CORBA::string_dup("FRONTEND::audio_allocation::full_bandwidth_channels");
    props[5].value <<= s.full_bandwidth_channels;
    props[6].id = CORBA::string_dup("FRONTEND::audio_allocation::low_frequency_effect_channels");
    props[6].value <<= s.low_frequency_effect_channels;
    a <<= props;
}

inline bool operator== (const frontendX::frontend_audio_allocation_struct& s1, const frontendX::frontend_audio_allocation_struct& s2) {
    if (s1.audio_type!=s2.audio_type)
        return false;
    if (s1.allocation_id!=s2.allocation_id)
        return false;
    if (s1.sample_rate!=s2.sample_rate)
        return false;
    if (s1.sample_rate_tolerance!=s2.sample_rate_tolerance)
        return false;
    if (s1.device_control!=s2.device_control)
        return false;
    if (s1.full_bandwidth_channels!=s2.full_bandwidth_channels)
        return false;
    if (s1.low_frequency_effect_channels!=s2.low_frequency_effect_channels)
        return false;
    return true;
}

inline bool operator!= (const frontendX::frontend_audio_allocation_struct& s1, const frontendX::frontend_audio_allocation_struct& s2) {
    return !(s1==s2);
}

template<> inline short StructProperty<frontendX::frontend_audio_allocation_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        CORBA::TypeCode_var aType = a.type();
        if (aType->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    frontendX::frontend_audio_allocation_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

inline bool operator>>= (const CORBA::Any& a, frontendX::default_frontend_audio_status_struct_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FRONTEND::audio_status::audio_type", props[idx].id)) {
            if (!(props[idx].value >>= s.audio_type)) return false;
        }
        if (!strcmp("FRONTEND::audio_status::allocation_id_csv", props[idx].id)) {
            if (!(props[idx].value >>= s.allocation_id_csv)) return false;
        }
        if (!strcmp("FRONTEND::audio_status::full_bandwidth_channels", props[idx].id)) {
            if (!(props[idx].value >>= s.full_bandwidth_channels)) return false;
        }
        if (!strcmp("FRONTEND::audio_status::low_frequency_effect_channels", props[idx].id)) {
            if (!(props[idx].value >>= s.low_frequency_effect_channels)) return false;
        }
        if (!strcmp("FRONTEND::audio_status::sample_rate", props[idx].id)) {
            if (!(props[idx].value >>= s.sample_rate)) return false;
        }
        if (!strcmp("FRONTEND::audio_status::enabled", props[idx].id)) {
            if (!(props[idx].value >>= s.enabled)) return false;
        }
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const frontendX::default_frontend_audio_status_struct_struct& s) {
    CF::Properties props;
    props.length(6);
    props[0].id = CORBA::string_dup("FRONTEND::audio_status::audio_type");
    props[0].value <<= s.audio_type;
    props[1].id = CORBA::string_dup("FRONTEND::audio_status::allocation_id_csv");
    props[1].value <<= s.allocation_id_csv;
    props[2].id = CORBA::string_dup("FRONTEND::audio_status::full_bandwidth_channels");
    props[2].value <<= s.full_bandwidth_channels;
    props[3].id = CORBA::string_dup("FRONTEND::audio_status::low_frequency_effect_channels");
    props[3].value <<= s.low_frequency_effect_channels;
    props[4].id = CORBA::string_dup("FRONTEND::audio_status::sample_rate");
    props[4].value <<= s.sample_rate;
    props[5].id = CORBA::string_dup("FRONTEND::audio_status::enabled");
    props[5].value <<= s.enabled;
    a <<= props;
}

inline bool operator== (const frontendX::default_frontend_audio_status_struct_struct& s1, const frontendX::default_frontend_audio_status_struct_struct& s2) {
    if (s1.audio_type!=s2.audio_type)
        return false;
    if (s1.allocation_id_csv!=s2.allocation_id_csv)
        return false;
    if (s1.full_bandwidth_channels!=s2.full_bandwidth_channels)
        return false;
    if (s1.low_frequency_effect_channels!=s2.low_frequency_effect_channels)
        return false;
    if (s1.sample_rate!=s2.sample_rate)
        return false;
    if (s1.enabled!=s2.enabled)
        return false;
    return true;
}

inline bool operator!= (const frontendX::default_frontend_audio_status_struct_struct& s1, const frontendX::default_frontend_audio_status_struct_struct& s2) {
    return !(s1==s2);
}

template<> inline short StructProperty<frontendX::default_frontend_audio_status_struct_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        CORBA::TypeCode_var aType = a.type();
        if (aType->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    frontendX::default_frontend_audio_status_struct_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

inline bool operator== (const std::vector<frontendX::default_frontend_audio_status_struct_struct>& s1, const std::vector<frontendX::default_frontend_audio_status_struct_struct>& s2) {
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

inline bool operator!= (const std::vector<frontendX::default_frontend_audio_status_struct_struct>& s1, const std::vector<frontendX::default_frontend_audio_status_struct_struct>& s2) {
    return !(s1==s2);
}

template<> inline short StructSequenceProperty<frontendX::default_frontend_audio_status_struct_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        CORBA::TypeCode_var aType = a.type();
        if (aType->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    std::vector<frontendX::default_frontend_audio_status_struct_struct> tmp;
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
