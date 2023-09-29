/***************************************************************************
* Copyright (C) 2023 ETH Zurich
* Core AI & Digital Biomarker, Acoustic and Inflammatory Biomarkers (ADAMMA)
* Centre for Digital Health Interventions (c4dhi.org)
* 
* Authors: Patrick Langer
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*         http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***************************************************************************/

#pragma once

#include "RunTime/RunTime.hpp"
#include "JavaWrapper/JavaWrapperMaster.hpp"
#include "JavaExtras/GenericParams.hpp"
#include "jbind11/jbind11.hpp"
namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        class ChannelWrapper
        {

            private:
                // ID of the channel.
                std::string channelID;

                // Name of the JavaWrapper that can handle data posted to this channel.
                // I.e., the name of the Wrapper of the data type that was specified in subscribe or post function.
                std::string wrapperNameOfDataTypeOfChannel;

                // Pointer to a channel object of type T that was created from the associated JavaWrapper.
                // The JavaWrapper knows how to cast this back to the original channel object.
                std::shared_ptr<void> channelReference; 

                JavaWrapperBase* getWrapperForDataType(const std::string& javaClassName)
                {
                    if(!JavaWrapperMaster::getInstance()->isWrapperRegisteredForNativeClass(this->wrapperNameOfDataTypeOfChannel))
                    {
                        // Check primitive types..
                        CLAID_THROW(Exception, "Error, a JavaModule tried to post data to channel with ID \"" << channelID << "\", but a wrapper for the type of data \n"
                        << "(\"" << this->wrapperNameOfDataTypeOfChannel << "\") of the channel was not found.\n"
                        << "This should not happen, as the channel previously was registered using such a wrapper, thus there HAS to be a wrapper available.\n"
                        << "This must a serious bug or linker problem. Did you load CLAID as a dynamic library *privately* or forgot to provide the correct flags for the linker to support global symbol lookup?");
                    }

                    // If a wrapper is available, data needs to have the attribute __CPP_CLASS_NAME__, as this was registered for all wrapped types.
                    if(!JavaWrapperMaster::getInstance()->isWrapperRegisteredForJavaClass(javaClassName))
                    {
                        CLAID_THROW(Exception, "Error, a JavaModule tried to post data to channel with ID \"" << channelID << "\", but the type of the provided data is invalid. "
                        "It is of an unsupported type \"" << javaClassName << "\", as no CLAID JavaWrapper is available for the type. Please proivide a CLAID JavaWrapper using DECLARE_SERIALIZATION and REGISTER_SERIALIZATION (C++) for this type.");
                    }

                    return JavaWrapperMaster::getInstance()->getWrapperForJavaClass(javaClassName);
                }

            public:
                
                ChannelWrapper()
                {

                }

                ChannelWrapper(std::string channelID, std::string wrapperNameOfDataTypeOfChannel, std::shared_ptr<void> channelReference) :
                    channelID(channelID), wrapperNameOfDataTypeOfChannel(wrapperNameOfDataTypeOfChannel), channelReference(channelReference)
                {

                }

                ~ChannelWrapper()
                {
                }

                // Reflection(ChannelWrapper,
                //     reflectFunction(post);
                // )

                static void addToJbindPackage(jbind11::JavaPackage& package)
                {
                    java::JavaClass<ChannelWrapper> javaClass(package, "Channel", java::GenericClass());

                    javaClass.def("post", &ChannelWrapper::post, java::GenericParams({0}));
                    javaClass.def("postWithTimestamp", &ChannelWrapper::postWithTimestamp, java::GenericParams({0}));
                //  pyClass.def("publish", &PythonModule::publish);
                }


                void post(jobject data)
                {
                    std::string javaClassName = java::JNIUtils::getNameOfClassOfObject(java::JNIUtils::getEnv(), data);
                    JavaWrapperBase* wrapper = getWrapperForDataType(javaClassName);

                    std::string dataTypeWrapperName = wrapper->getFullyQualifiedCppClassName();

                    if(dataTypeWrapperName != this->wrapperNameOfDataTypeOfChannel)
                    {
                        CLAID_THROW(Exception, "Error, a JavaModule tried to post data to channel with ID \"" << channelID << "\", but the type of the provided data does not match the data type of the channel. "
                        "The data of the channel is of type \"" << this->wrapperNameOfDataTypeOfChannel << "\" whereas the posted data is of type \"" << dataTypeWrapperName << "\".");
                    }

                    wrapper->post(this->channelReference, data);   
                }

                void postWithTimestamp(jobject data, int64_t timestamp)
                {
                    std::string javaClassName = java::JNIUtils::getNameOfClassOfObject(java::JNIUtils::getEnv(), data);
                    JavaWrapperBase* wrapper = getWrapperForDataType(javaClassName);

                    std::string dataTypeWrapperName = wrapper->getFullyQualifiedCppClassName();

                    if(dataTypeWrapperName != this->wrapperNameOfDataTypeOfChannel)
                    {
                        CLAID_THROW(Exception, "Error, a JavaModule tried to post data to channel with ID \"" << channelID << "\", but the type of the provided data does not match the data type of the channel. "
                        "The data of the channel is of type \"" << this->wrapperNameOfDataTypeOfChannel << "\" whereas the posted data is of type \"" << dataTypeWrapperName << "\".");
                    }

                    wrapper->postWithTimestamp(this->channelReference, data, timestamp);   
                }
        };
    }
}