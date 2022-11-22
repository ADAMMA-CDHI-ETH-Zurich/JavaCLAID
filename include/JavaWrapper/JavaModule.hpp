#pragma once
#include "RunTime/RunTime.hpp"

#include "JavaWrapper/JNIUtils.hpp"

namespace claid
{
    namespace JavaWrapper
    {
        class JavaModule : public Module
        {

            public:
                JavaModule(JNIEnv* env, jobject javaObject);
                JavaModule(JavaVM* javaVM, jobject javaObject);

                template<typename Reflector>
                void reflect(Reflector& r)
                {

                }

                void initialize();
                void postInitialize();
              
                jobject publish(JNIEnv* env, jclass dataType, jstring channelID);
                jobject subscribe(JNIEnv* env, jclass dataType, jstring channelID, jstring functionCallbackName, jstring functionSignature);
                
                void registerPeriodicFunction(JNIEnv* env, jstring identifier, jstring functionName, jint periodInMilliseconds);
                void unregisterPeriodicFunction(JNIEnv* env, jstring identifier);

                template<typename T>
                ChannelSubscriber<T> makeSubscriber(std::function<void (ChannelData<T>)> function)
                {
                    // runtime::getChannel(channelID).subscribe()
                    ChannelSubscriber<T> channelSubscriber(this->runnableDispatcherThread, function);
                    return channelSubscriber;
                }

                void callCallbackFunction(const std::string& functionName, jobject dataObject);

                JNIEnv* getEnv();


                virtual const std::string getModuleName();    

            private:
                JNIEnv* env;
                jobject javaObject;
                JavaVM* javaVM;

                std::string moduleName;

                void invokeJavaPeriodicFunction(std::string functionName);

        };
    }
}