#pragma once

#include "JavaModule.hpp"


#include "WrapperBase.hpp"
#include "JavaNativeSetter.hpp"
#include "JavaNativeGetter.hpp"
#include "JNIFactoryBase.hpp"
namespace claid
{
    namespace JavaWrapper
    {
        template<typename T>
        class Wrapper : public WrapperBase
        {
            private:
                const std::string cppClassName;
                std::string javaClassName;

                intptr_t getUniqueID()
                {
                    return reinterpret_cast<intptr_t>(this);
                }

                

                void spawnJavaChannelDataObjectFromData(JNIEnv* env, jobject data, jobject& channelDataObject)
                {
                    jclass cls = JNIUtils::findClass(env, std::string(Signatures::Class::ChannelData).c_str());

                    if(cls == nullptr)
                    {
                        CLAID_THROW(Exception, "Cannot create java object, failed to lookup ChannelData class " << Signatures::Class::ChannelData);
                    }

                    jmethodID constructor = env->GetMethodID(cls, "<init>", Signatures::Function::functionSignatureVoid({"java/lang/Object"}).c_str());

                    if(constructor == nullptr)
                    {
                        CLAID_THROW(Exception, "Cannot create java object, failed to lookup constructor for ChannelData class " << Signatures::Class::ChannelData);
                    }

                    channelDataObject = env->NewObject(cls, constructor, data);

                    if(channelDataObject == nullptr)
                    {
                        CLAID_THROW(Exception, "Cannot create java object, object with class signature " << Signatures::Class::ChannelData << " could not be created.");
                    }

                    env->DeleteLocalRef(cls);
                }

                jobject channelObjectToJavaChannelObject(JNIEnv* env, Channel<T>* channel)
                {
                    jobject channelObject;
                    JNIFactoryBase::newJavaObjectFromClassSignature(env, std::string(Signatures::Class::Channel), channelObject, "");

                    JNIHandle::setHandle(env, channelObject, channel);
                    return channelObject;
                }

                // Template trick to solve cyclic dependancies yet again ! Whoop whoop.
                // Problem was that there is a cycle dependency between JavaModule, WrapperBase and Serializations (as usual)
                // Serialization includes AdditionalSerialization, which includes WrapperMaster, which includes WrapperBase, which includes
                // JavaModule, which includes RunTime, which includes Serialization. Therefore, when we try to use any function of the JavaModule here in one
                // of our functions, we get "accessing member of incomplete type", or sth like this... Using the templated helper functions solves this,
                // as it forces the compiler to fully resolve the types and later implement the templated functions.. It's a fun game we're playing!
                template<typename JModule>
                jobject publishHelper(JNIEnv* env, JModule* module, jstring jChannelID)
                {
                    Channel<T>* channel = new Channel<T>;
                    std::string channelID = JNIUtils::toStdString(env, jChannelID);
                    *channel = CLAID_RUNTIME->channelManager.publish<T>(channelID, module->getUniqueIdentifier());
                    
                    jobject channelObject = channelObjectToJavaChannelObject(env, channel);

                    return channelObject;
                }

                template<typename JModule>
                jobject subscribeHelper(JNIEnv* env, JModule* module, jstring jChannelID, jstring jFunctionCallbackName, jstring jFunctionSignature)
                {
                    Logger::printfln("Subscribe 1");
                    Channel<T>* channel = new Channel<T>;
                    std::string channelID = JNIUtils::toStdString(env, jChannelID);
                    std::string callbackFunctionName = JNIUtils::toStdString(env, jFunctionCallbackName);
                    std::string callbackFunctionSignature = JNIUtils::toStdString(env, jFunctionSignature);

                    std::function<void (ChannelData<T>)> callbackFunction = 
                        std::bind(&Wrapper::onData, this, module, callbackFunctionName, callbackFunctionSignature, std::placeholders::_1);

                    Logger::printfln("Subscribe 2");

                    *channel = 
                        CLAID_RUNTIME->channelManager.subscribe<T>(channelID, module->makeSubscriber(callbackFunction), module->getUniqueIdentifier());
                    
                    jobject channelObject = channelObjectToJavaChannelObject(env, channel);
                    Logger::printfln("Subscribe 3");

                    return channelObject;
                }

                template<typename JModule>
                void onDataHelper(JModule* module, std::string javaCallbackFunctionToExecute, std::string javaCallbackFunctionSignature, ChannelData<T> channelData)
                {
                    Logger::printfln("OnData in Wrapper %s", this->javaClassName.c_str());
                    const T& data = channelData->value();

                    T* copiedData = new T(data);

                    JNIEnv* env = module->getEnv();

                    jobject javaDataObject;
                    // Reminder: javaClassName is the class name of the associated java class that this wrapper is responsible for.
                    JNIFactoryBase::newJavaObjectFromClassSignature(env, Signatures::Class::classNameToSignature(this->javaClassName), javaDataObject, "");
                    JNIHandle::setHandle(env, javaDataObject, copiedData);

                    // Now we have the data object.. and need to store it in the Java ChannelDataObject.
                    jobject javaChannelDataObject;
                    spawnJavaChannelDataObjectFromData(env, javaDataObject, javaChannelDataObject);

                    module->callCallbackFunction(javaCallbackFunctionToExecute, javaChannelDataObject);
                    env->DeleteLocalRef(javaDataObject);
                    env->DeleteLocalRef(javaChannelDataObject);
                }


            public:
                Wrapper(std::string cppClassName, std::string javaClassName) : cppClassName(cppClassName), javaClassName(javaClassName)
                {

                }

                Wrapper(std::string cppClassName) : cppClassName(cppClassName)
                {

                }

                void assignJavaClassName(std::string javaClassName)
                {
                    this->javaClassName = javaClassName;
                }

                virtual jobject publish(JNIEnv* env, JavaModule* module, jstring jChannelID)
                {
                    return publishHelper(env, module, jChannelID);
                }

                virtual jobject subscribe(JNIEnv* env, JavaModule* module, jstring jChannelID, jstring jFunctionCallbackName, jstring jFunctionSignature)
                {
                    return subscribeHelper(env, module, jChannelID, jFunctionCallbackName, jFunctionSignature);
                }

                virtual void post(JNIEnv* env, jobject jchannel, jobject jdata)
                {
                    Logger::printfln("Successfully posted data!");
                    Channel<T>* channel = JNIHandle::getHandle<Channel<T>>(env, jchannel);
                    T* data = JNIHandle::getHandle<T>(env, jdata);
                    // This will create a copy of the data.
                    channel->post(*data);
                }

                void assignInstance(JNIEnv* env, jobject javaObject)
                {
                    T* data = new T;
                    JNIHandle::setHandle(env, javaObject, data);
                }

                // Using template trick to solve cyclic dependency... yet again. Sorry.
                void onData(JavaModule* module, std::string javaCallbackFunctionToExecute, std::string javaCallbackFunctionSignature, ChannelData<T> channelData)
                {
                    onDataHelper(module, javaCallbackFunctionToExecute, javaCallbackFunctionSignature, channelData);
               
                }

                void set(JNIEnv* env, jobject object, jstring jVariableName, jobject value)
                {
                    std::string variableName = JNIUtils::toStdString(env, jVariableName);
                    Logger::printfln("Setting %s", variableName.c_str());
                    T* data = JNIHandle::getHandle<T>(env, object);
                    JavaNativeSetter setter;
                    setter.set(env, *data, variableName, value);
                }

                jobject get(JNIEnv* env, jobject object, jstring jVariableName)
                {
                    std::string variableName = JNIUtils::toStdString(env, jVariableName);
                    Logger::printfln("Getting %s", variableName.c_str());
                    T* data = JNIHandle::getHandle<T>(env, object);
                    JavaNativeGetter getter;
                    return getter.get(env, *data, variableName);
                }

        };
    }
}

/*
#define WRAP(T) \
jobject xpublish(JNIEnv* env, std::string channelID)\
{\
    Channel<T>* channel = new Channel<T>;\
    *channel = CLAID_RUNTIME->channelManager.publish<T>(channelID);\
    jclass cls = env->FindClass((std::string(Signatures::Class::Channel)).c_str());\
    \
    if(cls == nullptr)\
    {\
        CLAID_THROW(Exception, "Cannot publish, failed to lookup channel class " << Signatures::Class::Channel);\
    }\
    \
    jmethodID constructor = env->GetMethodID(cls, "<init>", "()V");\
    \
    if(constructor == nullptr)\
    {\
        CLAID_THROW(Exception, "Cannot publish, failed to lookup constructor for channel class " << Signatures::Class::Channel);\
    }\
    \
    jobject channelObject = env->NewObject(cls, constructor);\
    \
    if(channelObject == nullptr)\
    {\
        CLAID_THROW(Exception, "Cannot publish, to create channel object " << Signatures::Class::Channel);\
    }\
\
    JNIHandle::setHandle(env, channelObject, channel);\
\
    return channelObject;\
}*/