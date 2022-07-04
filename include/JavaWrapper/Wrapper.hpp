#pragma once

#include "WrapperBase.hpp"
#include "RunTime/RunTime.hpp"
namespace portaible
{
    namespace JavaWrapper
    {
        template<typename T>
        class Wrapper : public WrapperBase
        {
            virtual jobject publish(JNIEnv* env, jstring jChannelID)
            {
                Channel<T>* channel = new Channel<T>;
                std::string channelID = JNIUtils::toStdString(env, jChannelID);
                *channel = PORTAIBLE_RUNTIME->channelManager.publish<T>(channelID);
                jclass cls = env->FindClass((std::string(Signatures::Class::Channel)).c_str());

                if(cls == nullptr)
                {
                    PORTAIBLE_THROW(Exception, "Cannot publish, failed to lookup channel class " << Signatures::Class::Channel);
                }

                jmethodID constructor = env->GetMethodID(cls, "<init>", "()V");

                if(constructor == nullptr)
                {
                    PORTAIBLE_THROW(Exception, "Cannot publish, failed to lookup constructor for channel class " << Signatures::Class::Channel);
                }

                jobject channelObject = env->NewObject(cls, constructor);

                if(channelObject == nullptr)
                {
                    PORTAIBLE_THROW(Exception, "Cannot publish, to create channel object " << Signatures::Class::Channel);
                }

                JNIHandle::setHandle(env, channelObject, channel);

                return channelObject;
            }

            virtual void post(JNIEnv* env, jobject jchannel, jobject jdata)
            {
                Channel<T>* channel = JNIHandle::getHandle<Channel<T>>(env, jchannel);
                T* data = JNIHandle::getHandle<T>(env, jdata);
                channel->post(*data);
            }

            void assignInstance(JNIEnv* env, jobject javaObject)
            {
                T* data = new T;
                JNIHandle::setHandle(env, javaObject, data);
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