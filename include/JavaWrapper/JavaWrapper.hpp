#pragma once
#include "jbind11/jbind11.hpp"
namespace java = jbind11;

#include "JavaWrapper/JavaModule.hpp"
#include "JavaWrapper/ChannelWrapper.hpp"

#include "Logger/Logger.hpp"
#include "RunTime/RunTime.hpp"

#include "JbindWrapperGenerator.hpp"

namespace claid
{

    namespace JavaWrapper
    {
        // The JavaWrapper combines two functionalities:
        //      1st It may be used as reflector. Then, it creates a java class (using jbind) for the the type it is applied on.
        // 
        //      2nd Additionally, it provides additional functions for the class, which are not added to the created Java class (created in 1st), like publish and subscribe.
        //      They can not be used from the corresponding Java object directly, as they were not added to the wrapper with jbind. 
        //      However, those functions can be accessed from the C++ side, if the name of the type is known. For example, the JavaModule can retrieve an instance of this JavaWrapper class
        //      using the type name as a look up. Then, it can call the publish and subscribe functions from the C++ side.
        //      For convenience, the className is registered as def_readonly to the class, so it can be easily retrieved from the C++ side.
        //      E.g., if you pass a java object to C++, and the class of the object was created using jbind (1st)), the name of the class associated with the JavaWrapper
        //      is stored inside the object and can be retrieved from the C++ side, to find the corresponding wrapper. See JavaModule::publish and JavaModule::subscribe for examples.
        template<typename Class>
        class JavaWrapper : public JavaWrapperBase
        {   
            private:
                std::string className;
               
                bool staticMemberRegisted = false;


                // Template trick to solve cyclic dependancies yet again ! Whoop whoop.
                // Problem was that there is a cycle dependency between Java, JavaWrapperBase and Serializations (as usual)
                // Serialization includes AdditionalSerialization, which includes JavaWrapperMaster, which includes JavaWrapperBase, which includes
                // JavaModule, which includes RunTime, which includes Serialization. Therefore, when we try to use any function of the JavaModule here in one
                // of our functions, we get "accessing member of incomplete type", or sth like this... Using the templated helper functions solves this,
                // as it forces the compiler to fully resolve the types and later implement the templated functions.. It's a fun game we're playing!
                template<typename JavaModule>
                jobject subscribeHelper(JavaModule* module, jstring channelID, jstring callbackFunctionName) 
                {
                    std::string stdChannelID = java::fromJavaObject<std::string>(java::JNIUtils::getEnv(), channelID);
                    std::shared_ptr<Channel<Class>> channel(new Channel<Class>);
               

                    std::function<void (ChannelData<Class>)> callbackFunction = 
                        std::bind(&JavaWrapper::onData, this, module, java::fromJavaObject(java::JNIUtils::getEnv(), callbackFunctionName), std::placeholders::_1);

                    *channel = 
                        CLAID_RUNTIME->channelManager.subscribe<Class>(stdChannelID, module->makeSubscriber(callbackFunction), module->getUniqueIdentifier());
                    
                    // the claid::JavaWrapper::ChannelWrapper is available in JavaCLAID package, thus can be casted to 
                    // a java object automatically.
                 
                    ChannelWrapper channelWrapper(stdChannelID, this->className, std::static_pointer_cast<void>(channel));
                    printf("Subscribed to channel %s", stdChannelID.c_str());

                    jobject channelObject = java::cast(java::JNIUtils::getEnv(), channelWrapper);
                    return channelObject;
                }

                template<typename JavaModule>
                jobject publishHelper(JavaModule* module, jstring channelID) 
                {
                    std::string stdChannelID = java::fromJavaObject<std::string>(java::JNIUtils::getEnv(), channelID);
                    std::shared_ptr<Channel<Class>> channel(new Channel<Class>);
            
                    *channel = 
                        CLAID_RUNTIME->channelManager.publish<Class>(stdChannelID, module->getUniqueIdentifier());
                
                  
                    ChannelWrapper channelWrapper(channelID, this->className, std::static_pointer_cast<void>(channel));
                    jobject channelObject = java::cast(java::JNIUtils::getEnv(), channelWrapper);
                    return channelObject;
                }

                template<typename JavaModule>
                void onDataHelper(JavaModule* module, std::string callbackFunctionName, ChannelData<Class> channelData)
                {
                    Class* dataCopy = new Class(channelData->value());                   
                    jobject javaData = java::cast(dataCopy); 
                    module->callCallbackFunction(callbackFunctionName, javaData);
                    delete dataCopy;
                }

            public:
            
                JavaWrapper(std::string className) : className(className)
                {
                    // Register default generator (can be overriden later).
                    this->overrideJbindWrapperGenerator(new JbindWrapperGenerator<Class>());
                }

                virtual void generateWrapper(java::JavaPackage& package)
                {
                    this->wrapperGenerator->generate(package, this->className);
                }

                jobject subscribe(JavaModule* module, jstring channelID, jstring callbackFunctionName) 
                {
                    return subscribeHelper(module, channelID, callbackFunctionName);
                }

                virtual jobject publish(JavaModule* module, jstring channelID)
                {
                    return publishHelper(module, channelID);
                }

                virtual void post(std::shared_ptr<void> channelReference, jobject data)
                {
                    std::shared_ptr<Channel<Class>> channel = std::static_pointer_cast<Channel<Class>>(channelReference);

                    Class* obj = java::fromJavaObject<Class*>(java::JNIUtils::getEnv(), data);

                    channel->post(*obj);
                }


                void onData(JavaModule* module, std::string callbackFunctionName, ChannelData<Class> channelData)
                {
                    printf("creating data\n");
                    onDataHelper(module, callbackFunctionName, channelData);
                    printf("back\n");
                }

            
        };

    };
}

