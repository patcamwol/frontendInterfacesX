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
package frontendX;

import java.util.ArrayList;
import java.util.List;
import org.omg.CORBA.Any;
import org.ossie.properties.Action;
import org.ossie.properties.AnyUtils;
import org.ossie.properties.Kind;
import org.ossie.properties.Mode;
import org.ossie.properties.BooleanProperty;
import org.ossie.properties.DoubleProperty;
import org.ossie.properties.LongProperty;
import org.ossie.properties.StringProperty;
import org.ossie.properties.StructDef;
import org.ossie.properties.StructProperty;
import org.ossie.properties.UShortProperty;

public class FEXTypes {

    public static class frontend_audio_allocation_struct extends StructDef {
        public final StringProperty audio_type =
            new StringProperty(
                "FRONTEND::audio_allocation::audio_type", //id
                "audio_type", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final StringProperty allocation_id =
            new StringProperty(
                "FRONTEND::audio_allocation::allocation_id", //id
                "allocation_id", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final UShortProperty full_bandwidth_channels =
            new UShortProperty(
                "FRONTEND::audio_allocation::full_bandwidth_channels", //id
                "full_bandwidth_channels", //name
                (short)0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final UShortProperty low_frequency_effect_channels =
            new UShortProperty(
                "FRONTEND::audio_allocation::low_frequency_effect_channels", //id
                "low_frequency_effect_channels", //name
                (short)0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final DoubleProperty sample_rate =
            new DoubleProperty(
                "FRONTEND::audio_allocation::sample_rate", //id
                "sample_rate", //name
                0.0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final DoubleProperty sample_rate_tolerance =
            new DoubleProperty(
                "FRONTEND::audio_allocation::sample_rate_tolerance", //id
                "sample_rate_tolerance", //name
                10.0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final BooleanProperty device_control =
            new BooleanProperty(
                "FRONTEND::audio_allocation::device_control", //id
                "device_control", //name
                true, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
    
        public frontend_audio_allocation_struct() {
            addElement(this.audio_type);
            addElement(this.allocation_id);
            addElement(this.full_bandwidth_channels);
            addElement(this.low_frequency_effect_channels);
            addElement(this.sample_rate);
            addElement(this.sample_rate_tolerance);
            addElement(this.device_control);
        }
    
        public String getId() {
            return "FRONTEND::audio_allocation";
        }
    }
    
    public final StructProperty<frontend_audio_allocation_struct> frontend_audio_allocation =
        new StructProperty<frontend_audio_allocation_struct>(
            "FRONTEND::audio_allocation", //id
            "frontend_audio_allocation", //name
            frontend_audio_allocation_struct.class, //type
            new frontend_audio_allocation_struct(), //default value
            Mode.READWRITE, //mode
            new Kind[] {Kind.ALLOCATION} //kind
            );

    public static class default_frontend_audio_status_struct_struct extends StructDef {
        public final StringProperty audio_type =
            new StringProperty(
                "FRONTEND::audio_status::audio_type", //id
                "audio_type", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final StringProperty allocation_id_csv =
            new StringProperty(
                "FRONTEND::audio_status::allocation_id_csv", //id
                "allocation_id_csv", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final UShortProperty full_bandwidth_channels =
            new UShortProperty(
                "FRONTEND::audio_status::full_bandwidth_channels", //id
                "full_bandwidth_channels", //name
                (short)0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final UShortProperty low_frequency_effect_channels =
            new UShortProperty(
                "FRONTEND::audio_status::low_frequency_effect_channels", //id
                "low_frequency_effect_channels", //name
                (short)0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final BooleanProperty enabled =
            new BooleanProperty(
                "FRONTEND::audio_status::enabled", //id
                "enabled", //name
                false, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final DoubleProperty sample_rate =
            new DoubleProperty(
                "FRONTEND::audio_status::sample_rate", //id
                "sample_rate", //name
                0.0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
    
        /**
         * @generated
         */
        public default_frontend_audio_status_struct_struct() {
            addElement(this.audio_type);
            addElement(this.allocation_id_csv);
            addElement(this.full_bandwidth_channels);
            addElement(this.low_frequency_effect_channels);
            addElement(this.enabled);
            addElement(this.sample_rate);
        }
    
        public String getId() {
            return "frontend_audio_status_struct";
        }
    }

    public static class frontend_video_allocation_struct extends StructDef {
        public final StringProperty video_type =
            new StringProperty(
                "FRONTEND::video_allocation::video_type", //id
                "video_type", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final StringProperty allocation_id =
            new StringProperty(
                "FRONTEND::video_allocation::allocation_id", //id
                "allocation_id", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final LongProperty channels =
            new LongProperty(
                "FRONTEND::video_allocation::channels", //id
                "channels", //name
                0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final LongProperty frame_height =
            new LongProperty(
                "FRONTEND::video_allocation::frame_height", //id
                "frame_height", //name
                0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final LongProperty frame_width =
            new LongProperty(
                "FRONTEND::video_allocation::frame_width", //id
                "frame_width", //name
                0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final DoubleProperty fps =
            new DoubleProperty(
                "FRONTEND::video_allocation::fps", //id
                "fps", //name
                0.0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final DoubleProperty fps_tolerance =
            new DoubleProperty(
                "FRONTEND::video_allocation::fps_tolerance", //id
                "fps_tolerance", //name
                10.0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final BooleanProperty device_control =
            new BooleanProperty(
                "FRONTEND::video_allocation::device_control", //id
                "device_control", //name
                true, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
    
        public frontend_video_allocation_struct() {
            addElement(this.video_type);
            addElement(this.allocation_id);
            addElement(this.channels);
            addElement(this.frame_height);
            addElement(this.frame_width);
            addElement(this.fps);
            addElement(this.fps_tolerance);
            addElement(this.device_control);
        }
    
        public String getId() {
            return "FRONTEND::video_allocation";
        }
    }
    
    public final StructProperty<frontend_video_allocation_struct> frontend_video_allocation =
        new StructProperty<frontend_video_allocation_struct>(
            "FRONTEND::video_allocation", //id
            "frontend_video_allocation", //name
            frontend_video_allocation_struct.class, //type
            new frontend_video_allocation_struct(), //default value
            Mode.READWRITE, //mode
            new Kind[] {Kind.ALLOCATION} //kind
            );

    public static class default_frontend_video_status_struct_struct extends StructDef {
        public final StringProperty video_type =
            new StringProperty(
                "FRONTEND::video_status::video_type", //id
                "video_type", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final StringProperty allocation_id_csv =
            new StringProperty(
                "FRONTEND::video_status::allocation_id_csv", //id
                "allocation_id_csv", //name
                null, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final LongProperty channels =
            new LongProperty(
                "FRONTEND::video_status::channels", //id
                "channels", //name
                0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final LongProperty frame_height =
            new LongProperty(
                "FRONTEND::video_status::frame_height", //id
                "frame_height", //name
                0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final LongProperty frame_width =
            new LongProperty(
                "FRONTEND::video_status::frame_width", //id
                "frame_width", //name
                0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final DoubleProperty fps =
            new DoubleProperty(
                "FRONTEND::video_status::fps", //id
                "fps", //name
                0.0, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
        public final BooleanProperty enabled =
            new BooleanProperty(
                "FRONTEND::video_status::enabled", //id
                "enabled", //name
                false, //default value
                Mode.READWRITE, //mode
                Action.EXTERNAL, //action
                new Kind[] {Kind.CONFIGURE} //kind
                );
    
        /**
         * @generated
         */
        public default_frontend_video_status_struct_struct() {
            addElement(this.video_type);
            addElement(this.allocation_id_csv);
            addElement(this.channels);
            addElement(this.frame_height);
            addElement(this.frame_width);
            addElement(this.fps);
            addElement(this.enabled);
        }
    
        public String getId() {
            return "frontend_video_status_struct";
        }
    }
}
