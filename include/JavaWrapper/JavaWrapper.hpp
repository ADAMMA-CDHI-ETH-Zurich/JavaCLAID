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
        //      1st It may be used as reflector. Then, it creates a java class (using jbind11) for the the type it is applied on.
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
                // C++ class name
                std::string fullyQualifiedClassName;

                // Java class name of the generated java class.
                std::string canonicalJavaClassName = "";
               
                bool staticMemberRegisted = false;

                // Template trick to solve cyclic dependancies yet again ! Whoop whoop.
                // Problem was that there is a cycle dependency between Java, JavaWrapperBase and Serializations (as usual)
                // Serialization includes AdditionalSerialization, which includes JavaWrapperMaster, which includes JavaWrapperBase, which includes
                // JavaModule, which includes RunTime, which includes Serialization. Therefore, when we try to use any function of the JavaModule here in one
                // of our functions, we get "accessing member of incomplete type", or sth like this... Using the templated helper functions solves this,
                // as it forces the compiler to fully resolve the types and later implement the templated functions.. It's a fun game we're playing!
                template<typename JavaModule>
                ChannelWrapper subscribeHelper(JavaModule* module, std::string channelID, std::string callbackFunctionName) 
                {
                    std::shared_ptr<Channel<Class>> channel(new Channel<Class>);
               

                    std::function<void (ChannelData<Class>)> callbackFunction = 
                        std::bind(&JavaWrapper::onData, this, module, callbackFunctionName, std::placeholders::_1);

                    std::string channelIDWithNamespace = module->addNamespacesToChannelID(channelID);
                    *channel = 
                        CLAID_RUNTIME->channelManager.subscribe<Class>(channelIDWithNamespace, module->makeSubscriber(callbackFunction), module->getUniqueIdentifier());
                    
                    // the claid::JavaWrapper::ChannelWrapper is available in JavaCLAID package, thus can be casted to 
                    // a java object automatically.
                 
                    ChannelWrapper channelWrapper(channelIDWithNamespace, this->fullyQualifiedClassName, std::static_pointer_cast<void>(channel));
                    Logger::printfln("Subscribed to channel %s", channelIDWithNamespace.c_str());

                    return channelWrapper;
                }

                template<typename JavaModule>
                ChannelWrapper publishHelper(JavaModule* module, std::string channelID) 
                {
                    std::shared_ptr<Channel<Class>> channel(new Channel<Class>);
            
                    std::string channelIDWithNamespace = module->addNamespacesToChannelID(channelID);
                    *channel = 
                        CLAID_RUNTIME->channelManager.publish<Class>(channelIDWithNamespace, module->getUniqueIdentifier());
                  
                    ChannelWrapper channelWrapper(channelIDWithNamespace, this->fullyQualifiedClassName, std::static_pointer_cast<void>(channel));
                    return channelWrapper;
                }

                template<typename JavaModule>
                void onDataHelper(JavaModule* module, std::string callbackFunctionName, ChannelData<Class> channelData)
                {
                    Class* dataCopy = new Class(channelData->value());        

                    // TODO: Allow jbind11 to take ownership, then we dont need to delete this.
                    // Otherwise, this currently requires 2 copy operations
                    // (from channelData to dataCopy and then to javaData).           
                    jobject javaData = java::cast(*dataCopy); 
                    module->callCallbackFunction(callbackFunctionName, javaData);
                    java::JNIUtils::getEnv()->DeleteLocalRef(javaData);
                    
                    delete dataCopy;
                }

                std::string getNameFromFullyQualifiedName(const std::string& fqName)
                {
                    if(fqName == "")
                    {
                        return "";
                    }

                    std::string::size_type charIndex = fqName.size() - 1;

                    while(charIndex != 0)
                    {
                        if(fqName[charIndex] == ':')
                        {
                            return fqName.substr(charIndex + 1, fqName.size());
                        }
                        charIndex--;
                    }

                    // No namespace found, hence return the full name.
                    return fqName;                    
                }   


            public:
            
                JavaWrapper(std::string fullyQualifiedClassName) : fullyQualifiedClassName(fullyQualifiedClassName)
                {
                    // Register default generator (can be overriden later).
                    this->overrideJbindWrapperGenerator(new JbindWrapperGenerator<Class>());
                }

                virtual void generateWrapper(java::JavaPackage& package)
                {
                    std::string className = getNameFromFullyQualifiedName(this->fullyQualifiedClassName);
                    this->canonicalJavaClassName = package.getPackageName() + std::string(".") + className;

                    this->wrapperGenerator->generate(package, className);
                }

                ChannelWrapper subscribe(JavaModule* module, std::string channelID, std::string callbackFunctionName) 
                {
                    return subscribeHelper(module, channelID, callbackFunctionName);
                }

                virtual ChannelWrapper publish(JavaModule* module, std::string channelID)
                {
                    return publishHelper(module, channelID);
                }

                virtual void post(std::shared_ptr<void> channelReference, jobject data)
                {
                    std::shared_ptr<Channel<Class>> channel = std::static_pointer_cast<Channel<Class>>(channelReference);

                    Class* obj = java::fromJavaObject<Class*>(data);

                    channel->post(*obj);
                }

                virtual void postWithTimestamp(std::shared_ptr<void> channelReference, jobject data, int64_t timestamp)
                {
                    std::shared_ptr<Channel<Class>> channel = std::static_pointer_cast<Channel<Class>>(channelReference);

                    Class* obj = java::fromJavaObject<Class*>(data);


                    channel->post(*obj, Time::fromUnixTimestampMilliseconds(timestamp));
                }

                void onData(JavaModule* module, std::string callbackFunctionName, ChannelData<Class> channelData)
                {
                    onDataHelper(module, callbackFunctionName, channelData);
                }

                void forwardReflector(const char* memberFieldName, const std::string& reflectorName, void* reflectorPtr, jobject javaObject, jobject defaultValue)
                {
                    Class* dataPtr = java::fromJavaObject<Class*>(javaObject);
                    std::string className = ClassFactory::getInstance()->getClassNameOfObject(*dataPtr);

                    UntypedReflector* untypedReflector;
                    if (!ReflectionManager::getInstance()->getReflectorForClass(className, reflectorName, untypedReflector))
                    {
                        CLAID_THROW(claid::Exception, "Failed to forward reflector of type \"" << reflectorName << "\" to java class of type \"" << this->getCanonicalJavaClassName() << "\"."
                        << "The underlying Cpp class \"" << className << "\" was not registered to the CLAID serialization system. Please register this class using REGISTER_SERIALIZATION(...).");
                    }

                    if(defaultValue == nullptr)
                    {
                        untypedReflector->invokeMember(memberFieldName, static_cast<void*>(reflectorPtr), static_cast<void*>(dataPtr));
                    }
                    else
                    {
                        Class* defaultValuePtr = java::fromJavaObject<Class*>(defaultValue);
                        untypedReflector->invokeMemberWithDefaultValue(memberFieldName, static_cast<void*>(reflectorPtr), static_cast<void*>(dataPtr), static_cast<void*>(defaultValuePtr));
                    }
                }

                const std::string& getFullyQualifiedCppClassName() const
                {
                    return this->fullyQualifiedClassName;
                }

                const std::string& getCanonicalJavaClassName() const
                {
                    return this->canonicalJavaClassName;
                }


            
        };

    };
}

