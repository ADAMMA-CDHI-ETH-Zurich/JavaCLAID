#pragma once
#include "RunTime/RunTime.hpp"

#include "jbind11/JNIUtils/JNIUtils.hpp"
#include "ChannelWrapper.hpp"

namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        class JavaModule : public Module
        {
            private:


            public:
            

                JavaModule()
                {
                    
                }

                template<typename Reflector>
                void reflect(Reflector& r)
                {

                }

                static void addToJbindPackage(java::JavaPackage& p)
                {
                    java::JavaClass<JavaModule> cls(p, "Module");

                    cls.def("publish", &JavaModule::publish, java::GenericFunction(), java::GenericTypeReturn());
                    cls.def("subscribe", &JavaModule::subscribe, java::GenericFunction(), java::GenericTypeReturn());
                    cls.def("registerPeriodicFunction", &JavaModule::registerPeriodicFunction);
                    cls.def("getUniqueIdentifier", &JavaModule::getUniqueIdentifier);
                }

                void initialize();
                void postInitialize();
              
                ChannelWrapper publish(jclass dataType, std::string channelID);
                ChannelWrapper subscribe(jclass dataType, std::string channelID, std::string functionCallbackName);
                
                void registerPeriodicFunction(std::string identifier, std::string functionName, int32_t periodInMilliseconds);
                void unregisterPeriodicFunction(jstring identifier);

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

                std::string moduleName;

                void invokeJavaPeriodicFunction(std::string functionName);

        };
    }
}