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
#ifndef FE_AUDIO_DEVICE_BASE_H
#define FE_AUDIO_DEVICE_BASE_H

#include <frontend/fe_tuner_device.h>
#include <ossie/Device_impl.h>
#include <uuid/uuid.h>
#include <redhawk/FRONTEND/Frontend.h>
#include <redhawk/FRONTENDX/FrontendX.h>

#include "bulkio/bulkio.h"
#include "fe_audio_struct_props.h"
#include "fex_types.h"
#include "ossie/prop_helpers.h"

/*********************************************************************************************/
/**************************              FRONTENDX                   *************************/
/*********************************************************************************************/
namespace frontendX {

    /*
     *    Frontend audio class definition
     */
    template < typename AudioStatusStructType >
    class FrontendAudioDevice : public Device_impl
    {
        ENABLE_LOGGING

        public:
            FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl);
            FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev);
            FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities);
            FrontendAudioDevice(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev);
            ~FrontendAudioDevice();

            // this is implemented in the generated base class once all properties are known
            virtual void loadProperties();

            // Device specific allocation handling
            virtual CF::Device::UsageType updateUsageState();
            virtual CORBA::Boolean allocateCapacity(const CF::Properties & capacities) throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState);
            virtual void deallocateCapacity(const CF::Properties & capacities)throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState);

        protected:
            typedef std::map<std::string, size_t> string_number_mapping;
            typedef boost::mutex::scoped_lock exclusive_lock;

            // Member variables exposed as properties
            std::string device_kind;
            std::string device_model;
            frontendX::frontend_audio_allocation_struct frontend_audio_allocation;
            frontend::frontend_listener_allocation_struct frontend_listener_allocation;
            std::vector<AudioStatusStructType> frontend_audio_status;

            // audio_allocation_ids is exclusively paired with property frontend_audio_status.
            // audio_allocation_ids tracks allocation ids while frontend_audio_status provides audio device information.
            std::vector<frontend::tunerAllocationIdsStruct> audio_allocation_ids;

            // Provides mapping from unique allocation ID to internal audio device number
            string_number_mapping allocation_id_to_audio_device_id;
            boost::mutex allocation_id_mapping_lock;

            ///////////////////////////////
            // Device specific functions // -- virtual - to be implemented by device developer
            ///////////////////////////////
            virtual void audioDeviceEnable(AudioStatusStructType &fts, size_t audio_device_id) = 0;
            virtual void audioDeviceDisable(AudioStatusStructType &fts, size_t audio_device_id) = 0;
            virtual bool audioDeviceSetTuning(const frontend_audio_allocation_struct &request, AudioStatusStructType &fts, size_t audio_device_id) = 0;
            virtual bool audioDeviceDeleteTuning(AudioStatusStructType &fts, size_t audio_device_id) = 0;

            ///////////////////////////////
            // Mapping and translation helpers. External string identifiers to internal numerical identifiers
            ///////////////////////////////
            virtual std::string getControlAllocationId(size_t audio_device_id);
            virtual std::vector<std::string> getListenerAllocationIds(size_t audio_device_id);
            virtual long getAudioDeviceMapping(std::string allocation_id);
            std::string createAllocationIdCsv(size_t audio_device_id);
            virtual bool removeAudioDeviceMapping(size_t audio_device_id, std::string allocation_id);
            virtual bool removeAudioDeviceMapping(size_t audio_device_id);
            virtual void assignListener(const std::string& listen_alloc_id, const std::string& alloc_id);
            virtual void removeListener(const std::string& listen_alloc_id);
            virtual void removeAllocationIdRouting(const size_t audio_device_id) = 0;
            virtual void setNumChannels(size_t num) = 0;

            // Configure audio device - gets called during allocation
            virtual bool enableAudioDevice(size_t audio_device_id, bool enable);
            virtual bool listenerRequestValidation(frontend_audio_allocation_struct &request, size_t audio_device_id);


            ////////////////////////////
            // Other helper functions //
            ////////////////////////////
            BULKIO::StreamSRI create(std::string &stream_id, AudioStatusStructType &frontend_status) {
                BULKIO::StreamSRI sri;
                sri.hversion = 1;
                sri.xstart = 0.0;
                if ( frontend_status.sample_rate <= 0.0 )
                    sri.xdelta =  1.0;
                else
                    sri.xdelta = 1/(frontend_status.sample_rate * (frontend_status.full_bandwidth_channels + frontend_status.low_frequency_effect_channels));
                sri.xunits = BULKIO::UNITS_TIME;
                sri.subsize = 0;
                sri.ystart = 0.0;
                sri.ydelta = 0.0;
                sri.yunits = BULKIO::UNITS_NONE;
                sri.mode = 0;
                sri.blocking=false;
                sri.streamID = stream_id.c_str();
                this->addModifyKeyword<std::string> (&sri,"FRONTEND::DEVICE_ID",std::string(identifier()));
                return sri;
            }

            template <typename CORBAXX> bool addModifyKeyword(BULKIO::StreamSRI *sri, CORBA::String_member id, CORBAXX myValue, bool addOnly = false) {
                CORBA::Any value;
                value <<= myValue;
                unsigned long keySize = sri->keywords.length();
                if (!addOnly) {
                    for (unsigned int i = 0; i < keySize; i++) {
                        if (!strcmp(sri->keywords[i].id, id)) {
                            sri->keywords[i].value = value;
                            return true;
                        }
                    }
                }
                sri->keywords.length(keySize + 1);
                if (sri->keywords.length() != keySize + 1)
                    return false;
                sri->keywords[keySize].id = CORBA::string_dup(id);
                sri->keywords[keySize].value = value;
                return true;
            }

            // This is not currently used but is available as a debugging tool
            void printSRI(BULKIO::StreamSRI *sri, std::string strHeader = "DEBUG SRI"){
                std::cout << strHeader << ":\n";
                std::cout << "\thversion: " << sri->hversion<< std::endl;
                std::cout << "\txstart: " << sri->xstart<< std::endl;
                std::cout << "\txdelta: " << sri->xdelta<< std::endl;
                std::cout << "\txunits: " << sri->xunits<< std::endl;
                std::cout << "\tsubsize: " << sri->subsize<< std::endl;
                std::cout << "\tystart: " << sri->ystart<< std::endl;
                std::cout << "\tydelta: " << sri->ydelta<< std::endl;
                std::cout << "\tyunits: " << sri->yunits<< std::endl;
                std::cout << "\tmode: " << sri->mode<< std::endl;
                std::cout << "\tstreamID: " << sri->streamID<< std::endl;
                unsigned long keySize = sri->keywords.length();
                for (unsigned int i = 0; i < keySize; i++) {
                    std::cout << "\t KEYWORD KEY/VAL :: " << sri->keywords[i].id << ": " << ossie::any_to_string(sri->keywords[i].value) << std::endl;
                }
                std::cout << std::endl;
            }

        private:
            // this will be overridden by the generated base class once all ports are known
            virtual void construct();
    };

}; // end frontendX namespace

#endif
