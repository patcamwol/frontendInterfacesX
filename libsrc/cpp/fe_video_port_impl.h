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
#ifndef FE_VIDEO_PORT_H
#define FE_VIDEO_PORT_H

#include <frontend/fe_port_impl.h>

#include <redhawk/FRONTEND/Frontend.h>
#include <redhawk/FRONTENDX/MediaControl.h>


namespace frontendX {
    
    class video_delegation {
        public:
            virtual std::string getVideoType(const std::string& id) {
                throw FRONTEND::NotSupportedException("getVideoType not supported");
            }
            virtual bool getVideoDeviceControl(const std::string& id) {
                throw FRONTEND::NotSupportedException("getVideoDeviceControl not supported");
            }
            virtual long getChannels(const std::string& id) {
                throw FRONTEND::NotSupportedException("getChannels not supported");
            }
            virtual long getFrameHeight(const std::string& id) {
                throw FRONTEND::NotSupportedException("getFrameHeight not supported");
            }
            virtual long getFrameWidth(const std::string& id) {
                throw FRONTEND::NotSupportedException("getFrameWidth not supported");
            }
            virtual void setVideoEnable(const std::string& id, bool enable) {
                throw FRONTEND::NotSupportedException("setVideoEnable not supported");
            }
            virtual bool getVideoEnable(const std::string& id) {
                throw FRONTEND::NotSupportedException("getVideoEnable not supported");
            }
            virtual void setVideoOutputFrameRate(const std::string& id, double sr) {
                throw FRONTEND::NotSupportedException("setVideoOutputFrameRate not supported");
            }
            virtual double getVideoOutputFrameRate(const std::string& id) {
                throw FRONTEND::NotSupportedException("getVideoOutputFrameRate not supported");
            }
            virtual CF::Properties* getVideoStatus(const std::string& id) = 0;
    };

    class InFrontendVideoPort : public virtual POA_FRONTENDX::FrontendVideo, public Port_Provides_base_impl
    {
        public:
            InFrontendVideoPort(std::string port_name, video_delegation *_parent): 
            Port_Provides_base_impl(port_name)
            {
                parent = _parent;
            };
            ~InFrontendVideoPort() {};
            char* getVideoType(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (CORBA::string_dup(this->parent->getVideoType(_id).c_str()));
            };
            CORBA::Boolean getVideoDeviceControl(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getVideoDeviceControl(_id));
            };
            CORBA::Long getChannels(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getChannels(_id));
            };
            CORBA::Long getFrameHeight(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getFrameHeight(_id));
            };
            CORBA::Long getFrameWidth(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getFrameWidth(_id));
            };
            void setVideoEnable(const char* id, CORBA::Boolean enable) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                this->parent->setVideoEnable(_id, enable);
            };
            CORBA::Boolean getVideoEnable(const char* id) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                return (this->parent->getVideoEnable(_id));
            };
            void setVideoOutputFrameRate(const char* id, CORBA::Double sr) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                this->parent->setVideoOutputFrameRate(_id, sr);
            };
            CORBA::Double getVideoOutputFrameRate(const char* id) {
                boost::mutex::scoped_lock lock(this->portAccess);
                std::string _id(id);
                return (this->parent->getVideoOutputFrameRate(_id));
            };
            CF::Properties* getVideoStatus(const char* id) {
                boost::mutex::scoped_lock lock(portAccess);
                std::string _id(id);
                return (this->parent->getVideoStatus(_id));
            };
            std::string getRepid() const {
                return "IDL:FRONTENDX/FrontendVideo:1.0";
            };
        protected:
            boost::mutex portAccess;
        private:
            video_delegation *parent;
    };
    
    template<typename PortType_var, typename PortType>
    class OutFrontendVideoPortT : public frontend::OutFrontendPort<PortType_var, PortType>
    {
        public:
            OutFrontendVideoPortT(std::string port_name) : frontend::OutFrontendPort<PortType_var, PortType>(port_name)
            {};
            ~OutFrontendVideoPortT(){};
            
            std::string getVideoType(std::string &id) {
                CORBA::String_var retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getVideoType(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                std::string str_retval = ossie::corba::returnString(retval);
                return str_retval;
            };
            bool getVideoDeviceControl(std::string &id) {
                CORBA::Boolean retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getVideoDeviceControl(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            CORBA::Long getChannels(std::string &id) {
                CORBA::Long retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getChannels(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            CORBA::Long getFrameHeight(std::string &id) {
                CORBA::Long retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getFrameHeight(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            CORBA::Long getFrameWidth(std::string &id) {
                CORBA::Long retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getFrameWidth(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            void setVideoEnable(std::string &id, bool enable) {
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            ((*i).first)->setVideoEnable(id.c_str(), enable);
                        } catch(...) {
                        }
                    }
                }
                return;
            };
            bool getVideoEnable(std::string &id) {
                CORBA::Boolean retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getVideoEnable(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            void setVideoOutputFrameRate(std::string &id, double sr) {
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            ((*i).first)->setVideoOutputFrameRate(id.c_str(), sr);
                        } catch(...) {
                        }
                    }
                }
                return;
            };
            double getVideoOutputFrameRate(std::string &id) {
                CORBA::Double retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getVideoOutputFrameRate(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
            CF::Properties* getVideoStatus(std::string &id) {
                CF::Properties* retval;
                typename std::vector < std::pair < PortType_var, std::string > >::iterator i;
                boost::mutex::scoped_lock lock(this->updatingPortsLock);   // don't want to process while command information is coming in
                if (this->active) {
                    for (i = this->outConnections.begin(); i != this->outConnections.end(); ++i) {
                        try {
                            retval = ((*i).first)->getVideoStatus(id.c_str());
                        } catch(...) {
                        }
                    }
                }
                return retval;
            };
    };
 
    // ----------------------------------------------------------------------------------------
    // OutFrontendVideoPort declaration
    // ----------------------------------------------------------------------------------------
    class OutFrontendVideoPort : public OutFrontendVideoPortT<FRONTENDX::FrontendVideo_var,FRONTENDX::FrontendVideo> {
        public:
            OutFrontendVideoPort(std::string port_name) : OutFrontendVideoPortT<FRONTENDX::FrontendVideo_var,FRONTENDX::FrontendVideo>(port_name)
            {};
    };

} // end of frontendX namespace


#endif
