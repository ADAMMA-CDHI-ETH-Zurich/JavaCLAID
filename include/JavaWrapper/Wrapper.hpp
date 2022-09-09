#pragma once



#include "WrapperBase.hpp"
#include "RunTime/RunTime.hpp"
#include "JavaNativeSetter.hpp"
namespace portaible
{
    namespace JavaWrapper
    {
        template<typename T>
        class Wrapper : public WrapperBase
        {
            private:
                const std::string javaClassName;

                intptr_t getUniqueID()
                {
                    return reinterpret_cast<intptr_t>(this);
                }

                void newJavaObjectFromClassSignature(JNIEnv* env, std::string className, jobject& channelObject)
                {
                    // Why use custom findClass?
                    // See https://stackoverflow.com/questions/13263340/findclass-from-any-thread-in-android-jni/16302771#16302771

                    jclass cls = JNIUtils::findClass(env, className.c_str());

                    if(cls == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java object, failed to lookup class " << className);
                    }

                    jmethodID constructor = env->GetMethodID(cls, "<init>", "()V");

                    if(constructor == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java object, failed to lookup constructor for class " << className);
                    }

                    channelObject = env->NewObject(cls, constructor);

                    if(channelObject == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java object, object with class signature " << className << " could not be created.");
                    }
                }

                void spawnJavaChannelDataObjectFromData(JNIEnv* env, jobject data, jobject& channelDataObject)
                {
                    jclass cls = JNIUtils::findClass(env, std::string(Signatures::Class::ChannelData).c_str());

                    if(cls == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java object, failed to lookup ChannelData class " << Signatures::Class::ChannelData);
                    }

                    jmethodID constructor = env->GetMethodID(cls, "<init>", Signatures::Function::functionSignatureVoid({"java/lang/Object"}).c_str());

                    if(constructor == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java object, failed to lookup constructor for ChannelData class " << Signatures::Class::ChannelData);
                    }

                    channelDataObject = env->NewObject(cls, constructor, data);

                    if(channelDataObject == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java object, object with class signature " << Signatures::Class::ChannelData << " could not be created.");
                    }
                }

                jobject channelObjectToJavaChannelObject(JNIEnv* env, Channel<T>* channel)
                {
                    jobject channelObject;
                    newJavaObjectFromClassSignature(env, std::string(Signatures::Class::Channel), channelObject);

                    JNIHandle::setHandle(env, channelObject, channel);
                    return channelObject;
                }

            public:
                Wrapper(std::string javaClassName) : javaClassName(javaClassName)
                {

                }

                virtual jobject publish(JNIEnv* env, JavaModule* module, jstring jChannelID)
                {
                    Channel<T>* channel = new Channel<T>;
                    std::string channelID = JNIUtils::toStdString(env, jChannelID);
                    *channel = PORTAIBLE_RUNTIME->channelManager.publish<T>(channelID, module->getUniqueIdentifier());
                    
                    jobject channelObject = channelObjectToJavaChannelObject(env, channel);

                    return channelObject;
                }

                virtual jobject subscribe(JNIEnv* env, JavaModule* module, jstring jChannelID, jstring jFunctionCallbackName, jstring jFunctionSignature)
                {
                    Channel<T>* channel = new Channel<T>;
                    std::string channelID = JNIUtils::toStdString(env, jChannelID);
                    std::string callbackFunctionName = JNIUtils::toStdString(env, jFunctionCallbackName);
                    std::string callbackFunctionSignature = JNIUtils::toStdString(env, jFunctionSignature);

                    std::function<void (ChannelData<T>)> callbackFunction = 
                        std::bind(&Wrapper::onData, this, module, callbackFunctionName, callbackFunctionSignature, std::placeholders::_1);


                    *channel = 
                        PORTAIBLE_RUNTIME->channelManager.subscribe<T>(channelID, module->makeSubscriber(callbackFunction), module->getUniqueIdentifier());
                    
                    jobject channelObject = channelObjectToJavaChannelObject(env, channel);


                    return channelObject;
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

                void onData(JavaModule* module, std::string javaCallbackFunctionToExecute, std::string javaCllbackFunctionSignature, ChannelData<T> channelData)
                {
                    Logger::printfln("OnData in Wrapper");
                    const T& data = channelData->value();

                    T* copiedData = new T(data);

                    JNIEnv* env = module->getEnv();

                    jobject javaDataObject;
                    // Reminder: javaClassName is the class name of the associated java class that this wrapper is responsible for.
                    newJavaObjectFromClassSignature(env, Signatures::Class::classNameToSignature(this->javaClassName), javaDataObject);
                    JNIHandle::setHandle(env, javaDataObject, copiedData);

                    // Now we have the data object.. and need to store it in the Java ChannelDataObject.
                    jobject javaChannelDataObject;
                    spawnJavaChannelDataObjectFromData(env, javaDataObject, javaChannelDataObject);

                    module->callCallbackFunction(javaCallbackFunctionToExecute, javaChannelDataObject);
               
                }

                void set(JNIEnv* env, jobject object, std::string variableName, jobject value)
                {
                    T* data = JNIHandle::getHandle<T>(env, object);
                    JavaNativeSetter setter;
                    setter.set(env, object, variableName, value);
                }

        };
    }
}

/*
#define WRAP(T) \
jobject xpublish(JNIEnv* env, std::string channelID)\
{\
    Channel<T>* channel = new Channel<T>;\
    *channel = PORTAIBLE_RUNTIME->channelManager.publish<T>(channelID);\
    jclass cls = env->FindClass((std::string(Signatures::Class::Channel)).c_str());\
    \
    if(cls == nullptr)\
    {\
        PORTAIBLE_THROW(Exception, "Cannot publish, failed to lookup channel class " << Signatures::Class::Channel);\
    }\
    \
    jmethodID constructor = env->GetMethodID(cls, "<init>", "()V");\
    \
    if(constructor == nullptr)\
    {\
        PORTAIBLE_THROW(Exception, "Cannot publish, failed to lookup constructor for channel class " << Signatures::Class::Channel);\
    }\
    \
    jobject channelObject = env->NewObject(cls, constructor);\
    \
    if(channelObject == nullptr)\
    {\
        PORTAIBLE_THROW(Exception, "Cannot publish, to create channel object " << Signatures::Class::Channel);\
    }\
\
    JNIHandle::setHandle(env, channelObject, channel);\
\
    return channelObject;\
}*/