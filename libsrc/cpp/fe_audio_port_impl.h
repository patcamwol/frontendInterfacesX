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
#ifndef FE_AUDIO_PORT_H
#define FE_AUDIO_PORT_H

#include <frontend/fe_port_impl.h>

#include <redhawk/FRONTEND/Frontend.h>
#include <redhawk/FRONTENDX/MediaControl.h>


namespace frontendX {
    
    class audio_delegation {
        public:
            virtual std::string getAudioType(const std::string& id) {
                throw FRONTEND::NotSupportedException("getAudioType not supported");
            }
            virtual bool getAudioDeviceControl(const std::string& id) {
                throw FRONTEND::NotSupportedException("getAudioDeviceControl not supported");
            }
            virtual unsigned short getFullBandwidthChannels(const std::string& id) {
                throw FRONTEND::NotSupportedException("getFullBandwidthChannels not supported");
            }
            virtual unsigned short getLowFrequencyEffectChannels(const std::string& id) {
                throw FRONTEND::NotSupportedException("getLowFrequencyEffectChannels not supported");
            }
            virtual void setAudioEnable(const std::string& id, bool enable) {
                throw FRONTEND::NotSupportedException("setAudioEnable not supported");
            }
            virtual bool getAudioEnable(const std::string& id) {
                throw FRONTEND::NotSupportedException("getAudioEnable not supported");
            }
            virtual void setAudioOutputSampleRate(const std::string& id, double sr) {
                throw FRONTEND::NotSupportedException("setAudioOutputSampleRate not supported");
            }
            virtual double getAudioOutputSampleRate(const std::string& id) {
                throw FRONTEND::NotSupportedException("getAudioOutputSampleRate not supported");
            }
            virtual CF::Properties* getAudioStatus(const std::string& id) = 0;
    };

    class InFrontendAudioPort : public virtual POA_FRONTENDX::FrontendAudio, public Port_Provides_base_impl
    {
        public:
            InFrontendAudioPort(std::string port_name, audio_delegation *_parent): 
            Port_Provides_base_impl(port_name)
            {
                parent = _parent;
            };
            ~InFrontendAudioPort() {};
            char* getAudioType(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (CORBA::string_dup(this->parent->getAudioType(_id).c_str()));
            };
            CORBA::Boolean getAudioDeviceControl(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getAudioDeviceControl(_id));
            };
            CORBA::UShort getFullBandwidthChannels(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getFullBandwidthChannels(_id));
            };
            CORBA::UShort getLowFrequencyEffectChannels(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getLowFrequencyEffectChannels(_id));
            };
            void setAudioEnable(const char* id, CORBA::Boolean enable) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                this->parent->setAudioEnable(_id, enable);
            };
            CORBA::Boolean getAudioEnable(const char* id) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                return (this->parent->getAudioEnable(_id));
            };
            void setAudioOutputSampleRate(const char* id, CORBA::Double sr) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                this->parent->setAudioOutputSampleRate(_id, sr);
            };
            CORBA::Double getAudioOutputSampleRate(const char* id) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                return (this->parent->getAudioOutputSampleRate(_id));
            };
            CF::Properties* getAudioStatus(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getAudioStatus(_id));
            };
            std::string getRepid() const {
                return "IDL:FRONTENDX/FrontendAudio:1.0";
            };
        protected:
            boost::mutex portAccess;
        private:
            audio_delegation *parent;
    };
    
    template<typename PortType_var, typename PortType>
    class OutFrontendAudioPortT : public frontend::OutFrontendPort<PortType_var, PortType>
    {
        public:
            OutFrontendAudioPortT(std::string port_name) : frontend::OutFrontendPort<PortType_var, PortType>(port_name)
            {};
            ~OutFrontendAudioPortT(){};
            
            std::string getAudioType(std::string &id) {
                CORBA::String_var retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getAudioType(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                std::string str_retval = ossie::corba::returnString(retval);
                return str_retval;
            };
            bool getAudioDeviceControl(std::string &id) {
                CORBA::Boolean retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getAudioDeviceControl(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            CORBA::UShort getFullBandwidthChannels(std::string &id) {
                CORBA::UShort retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getFullBandwidthChannels(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            CORBA::UShort getLowFrequencyEffectChannels(std::string &id) {
                CORBA::UShort retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getLowFrequencyEffectChannels(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            void setAudioEnable(std::string &id, bool enable) {
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            ((*i).first)->setAudioEnable(id.c_str(), enable);
                        } catch(...) {
                        }
                    }
                }
                return;
            };
            bool getAudioEnable(std::string &id) {
                CORBA::Boolean retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getAudioEnable(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            void setAudioOutputSampleRate(std::string &id, double sr) {
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            ((*i).first)->setAudioOutputSampleRate(id.c_str(), sr);
                        } catch(...) {
                        }
                    }
                }
                return;
            };
            double getAudioOutputSampleRate(std::string &id) {
                CORBA::Double retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getAudioOutputSampleRate(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            CF::Properties* getAudioStatus(std::string &id) {
                CF::Properties* retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getAudioStatus(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
    };
 
    // ----------------------------------------------------------------------------------------
    // OutFrontendAudioPort declaration
    // ----------------------------------------------------------------------------------------
    class OutFrontendAudioPort : public OutFrontendAudioPortT<FRONTENDX::FrontendAudio_var,FRONTENDX::FrontendAudio> {
        public:
            OutFrontendAudioPort(std::string port_name) : OutFrontendAudioPortT<FRONTENDX::FrontendAudio_var,FRONTENDX::FrontendAudio>(port_name)
            {};
    };

} // end of frontendX namespace


#endif
