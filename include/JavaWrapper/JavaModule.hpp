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

#include "jbind11/JNIUtils/JNIUtils.hpp"
#include "ChannelWrapper.hpp"
#include "TypedJavaReflector.hpp"
#include "JavaNativeClasses/Consumer.hpp"
#include "JavaNativeClasses/Runnable.hpp"
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
                    JNIEnv* env = java::JNIUtils::getEnv();

                    jobject self = java::cast(this);
                    jclass cls = java::JNIUtils::getClassOfObject(env, self);

                    std::shared_ptr<TypedJavaReflector<Reflector>> reflector = std::make_shared<TypedJavaReflector<Reflector>>(self, &r);
                    jmethodID mid =
                        env->GetMethodID(cls, "reflect", "(LJavaCLAID/Reflector;)V");

                    if(mid != nullptr)
                    {
                        //   CLAID_THROW(Exception, "Cannot reflect JavaModule \"" << this->getModuleName() << "\". This Module has no reflect function with the following signature: \n"
                        //   << "public void reflect(Reflector reflector)");
                        jobject javaReflectorObject = java::JNIUtils::createObjectFromClassName(env, "JavaCLAID.Reflector", "");
                        

                        // When we create an instance of a class that was created using jbind11 from C++,
                        // the jobject that is passed to JBindWrapper_init(...) is only temporary and not valid after init anymore.
                        // The valid reference is the one we got from createObjectFromClassName.
                        // Hence, we need to override the stored reference in the handle.
                        java::JavaHandle handle = java::JavaHandle::getHandleFromObject(java::JNIUtils::getEnv(), javaReflectorObject);
                        handle.getData()->setJavaObjectReference(javaReflectorObject);
                        handle.getData()->overrideNativeData(std::static_pointer_cast<void>(reflector));

                        GenericJavaReflector* genericReflector = java::fromJavaObject<GenericJavaReflector*>(javaReflectorObject);
                    
                        env->CallVoidMethod(self, mid, javaReflectorObject);
                        env->DeleteLocalRef(javaReflectorObject);
                    }
                    else
                    {
                        env->ExceptionClear();
                    }
                    env->DeleteLocalRef(cls);


                    // std::cout << std::flush;

                    // JavaWrapperBase* wrapper = getWrapperByName(fieldClassName);
                    // wrapper->getReflectorByName(ReflectorName<Reflector>);


                    // Type->getReflectorByName();
                    // WrappedReflector<Reflector> = getReflectorByName(ReflectorName<Reflector>)


                    // JavaWrappedReflector<Reflector> wrappedReflector(r);
                    // jmethodID mid =
                    // env->GetMethodID(cls, "reflect", "(LJavaCLAID.Reflector)V");
                    // Logger::printfln("JavaModule init  5");

                    // if(mid == nullptr)
                    // {
                    //     // CLAID_THROW(Exception, "Error, function initialize with signature void () not found for class "
                    //     //     << JNIUtils::getClassName(env, cls));
                    // }
                    // else
                    // {
                    //     env->CallVoidMethod(self, mid);
                    // }*/

                }

                static void addToJbindPackage(java::JavaPackage& p)
                {
                    java::JavaClass<JavaModule> cls(p, "Module");

                    cls.def("publish", &JavaModule::publish, java::GenericFunction(), java::GenericTypeReturn());
                    cls.def("subscribe", &JavaModule::subscribe, java::GenericFunction(), java::GenericTypeReturn(), java::GenericParams({2},{"java.util.function.Consumer<ChannelData<T>>"}));
                    cls.def("registerPeriodicFunction", &JavaModule::registerPeriodicFunction);
                    cls.def("getUniqueIdentifier", &JavaModule::getUniqueIdentifier);
                    cls.def("scheduleFunctionAtTime", &JavaModule::scheduleFunctionAtTime);
                    cls.def("scheduleFunctionInXDays", &JavaModule::scheduleFunctionInXDays);
                    cls.def("scheduleFunctionInXHours", &JavaModule::scheduleFunctionInXHours);
                    cls.def("scheduleFunctionInXMinutes", &JavaModule::scheduleFunctionInXMinutes);
                    cls.def("scheduleFunctionInXSeconds", &JavaModule::scheduleFunctionInXSeconds);
                    cls.def("scheduleFunctionInXMilliSeconds", &JavaModule::scheduleFunctionInXMilliSeconds);
                }

                void initialize();
                void postInitialize();
              
                ChannelWrapper publish(jclass dataType, std::string channelID);
                ChannelWrapper subscribe(jclass dataType, std::string channelID, java::Consumer consumer);
                
                void registerPeriodicFunction(std::string identifier, java::Runnable, int32_t periodInMilliseconds);
                void unregisterPeriodicFunction(std::string identifier);


                void scheduleFunctionAtTime(std::string name, java::Runnable runnable, int32_t hour, int32_t minute, int32_t second, int32_t millisecond);
                void scheduleFunctionInXDays(std::string name, java::Runnable runnable, int32_t days);
                void scheduleFunctionInXHours(std::string name, java::Runnable runnable, int32_t hours);
                void scheduleFunctionInXMinutes(std::string name, java::Runnable runnable, int32_t minutes);
                void scheduleFunctionInXSeconds(std::string name, java::Runnable runnable, int32_t seconds);
                void scheduleFunctionInXMilliSeconds(std::string name, java::Runnable runnable, int32_t seconds);
            


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


                void invokeJavaFunction(java::Runnable runnable);

        };
    }
}