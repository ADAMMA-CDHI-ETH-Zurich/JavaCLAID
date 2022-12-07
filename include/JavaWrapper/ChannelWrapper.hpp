#pragma once

#include "RunTime/RunTime.hpp"
#include "JavaWrapper/JavaWrapperMaster.hpp"
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

                static void addToPybindModule(jbind11::JavaPackage& package)
                {
                    java::JavaClass<ChannelWrapper> javaClass(package, "Channel");

                    // No init function. A Channel should not be creatable from within Python, only should be retrievable using publish and subscribe functions.
                    //pyClass.def(py::init<>());
                    javaClass.def("post", &ChannelWrapper::post);
                //  pyClass.def("publish", &PythonModule::publish);
                }


                void post(jobject data)
                {
                    if(!JavaWrapperMaster::getInstance()->isWrapperRegisteredForNativeClass(this->wrapperNameOfDataTypeOfChannel))
                    {
                        // Check primitive types..
                        CLAID_THROW(Exception, "Error, a JavaModule tried to post data to channel with ID \"" << channelID << "\", but a wrapper for the type of data "
                        << "of the channel was not found. This should not happen, as the channel previously was registered using such a wrapper, thus there HAS to be a wrapper available. "
                        << "This must a serious bug or linker problem. Did you load CLAID as a dynamic library *privately* or forgot to provide the correct flags for the linker to support global symbol lookup?");
                    }

                    // If a wrapper is available, data needs to have the attribute __CPP_CLASS_NAME__, as this was registered for all wrapped types.
                    if(!java::hasattr(data, "__CPP_CLASS_NAME__"))
                    {
                        std::string typeName = java::JNIUtils::getNameOfClassOfObject(java::JNIUtils::getEnv(), data);
                       
                        CLAID_THROW(Exception, "Error, a JavaModule tried to post data to channel with ID \"" << channelID << "\", but the type of the provided data is invalid. "
                        "It is of an unsupported type \"" << typeName << "\", as no CLAID JavaWrapper is available for the type. Please proivide a CLAID JavaWrapper using DECLARE_SERIALIZATION and REGISTER_SERIALIZATION (C++) for this type.");
                    }

                    std::string dataTypeWrapperName = java::fromJavaObject<std::string>(java::JNIUtils::getEnv(), java::attr(data, "__CPP_CLASS_NAME__"));

                    if(dataTypeWrapperName != this->wrapperNameOfDataTypeOfChannel)
                    {
                        CLAID_THROW(Exception, "Error, a JavaModule tried to post data to channel with ID \"" << channelID << "\", but the type of the provided data does not match the data type of the channel. "
                        "The data of the channel is of type \"" << this->wrapperNameOfDataTypeOfChannel << "\" whereas the posted data is of type \"" << dataTypeWrapperName << "\".");
                    }

                    JavaWrapperBase* wrapper = JavaWrapperMaster::getInstance()->getWrapperForNativeClass(this->wrapperNameOfDataTypeOfChannel);
                    wrapper->post(this->channelReference, data);   
                }
        };
    }
}