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

#include "JavaWrapper/JavaModule.hpp"
#include "JavaWrapper/JavaWrapperMaster.hpp"
#include "JavaWrapper/Signatures.hpp"
#include "JavaWrapper/ChannelDataWrapper.hpp"

namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        

        void JavaModule::initialize()
        {
            JNIEnv* env = java::JNIUtils::getEnv();
            jobject self = java::cast(this);
            jclass cls = java::JNIUtils::getClassOfObject(env, self);

            jmethodID mid =
                    env->GetMethodID(cls, "initialize", "()V");

            if(mid == nullptr)
            {
                // CLAID_THROW(Exception, "Error, function initialize with signature void () not found for class "
                //     << JNIUtils::getClassName(env, cls));
            }
            else
            {
                env->CallVoidMethod(self, mid);
            }


            env->DeleteLocalRef(cls);
        }

        void JavaModule::postInitialize()
        {
            // jclass cls = JNIUtils::getClassOfObject(env, javaObject);

            // jmethodID mid =
            //         env->GetMethodID(cls, "postInitialize", "()V");

            // if(mid == nullptr)
            // {
            //     // CLAID_THROW(Exception, "Error, function initialize with signature void () not found for class "
            //     //     << JNIUtils::getClassName(env, cls));
            // }
            // else
            // {
            //     // Return should be of type java class "Channel" (portaible.JavaWrappers.Channel)
            //     env->CallVoidMethod(javaObject, mid);

            // }

            // env->DeleteLocalRef(cls);
        }

        ChannelWrapper JavaModule::publish(jclass dataType, std::string channelID)
        {
            std::string canonicalClassName = java::JNIUtils::getClassName(java::JNIUtils::getEnv(), dataType);
          
            if(!JavaWrapperMaster::getInstance()->isWrapperRegisteredForJavaClass(canonicalClassName))
            {
                CLAID_THROW(Exception, "Error, JavaModule tried to publish channel\"" << channelID << "\" with data type \"" << canonicalClassName << "\", however"
                << "no JavaWrapper is available for this data type. Please implement a wrapper for this type using DECLARE_SERIALIZATION and"
                << "REGISTER_SERIALIZATION (invasively or non-invasively).");
            }
            JavaWrapperBase* javaWrapper = JavaWrapperMaster::getInstance()->getWrapperForJavaClass(canonicalClassName);
            return javaWrapper->publish(this, channelID);
        }

        ChannelWrapper JavaModule::subscribe(jclass dataType, std::string channelID, java::Consumer consumer)
        {
            std::string canonicalClassName = java::JNIUtils::getClassName(java::JNIUtils::getEnv(), dataType);
          
            if(!JavaWrapperMaster::getInstance()->isWrapperRegisteredForJavaClass(canonicalClassName))
            {
                CLAID_THROW(Exception, "Error, JavaModule tried to subscribe to channel\"" << channelID << "\" with data type \"" << canonicalClassName << "\", however"
                << "no JavaWrapper is available for this data type. Please implement a wrapper for this type using DECLARE_SERIALIZATION and"
                << "REGISTER_SERIALIZATION (invasively or non-invasively).");
            }

            JavaWrapperBase* javaWrapper = JavaWrapperMaster::getInstance()->getWrapperForJavaClass(canonicalClassName);
            return javaWrapper->subscribe(this, channelID, consumer);
        }

        void JavaModule::callCallbackFunction(const std::string& functionName, jobject dataObject)
        {
            JNIEnv* env = java::JNIUtils::getEnv();

            // Looks up the handle associated with this JavaModule from the global table.
            jobject self = java::cast(this);


            jclass cls = java::JNIUtils::getClassOfObject(env, self);
            jmethodID mid =
                    env->GetMethodID(cls, functionName.c_str(), Signatures::Function::functionSignatureVoid({Signatures::Class::ChannelData}).c_str());
            
            if(mid == nullptr)
            {
                claid::Logger::printfln("NULL");
                CLAID_THROW(Exception, "Error, could not call callback function. Function " << functionName << " with signature void " << Signatures::Class::ChannelData << " not found for class "
                    << java::JNIUtils::getNameOfClassOfObject(env, dataObject));
            }
            else
            {
                ChannelDataWrapper channelData(dataObject);
                
                jobject javaChannelData = java::cast(channelData);

                env->CallVoidMethod(self, mid, javaChannelData);
                env->DeleteLocalRef(javaChannelData);
                
            }

            env->DeleteLocalRef(cls);
        }
    
        void JavaModule::registerPeriodicFunction(std::string identifier, java::Runnable runnable, int32_t periodInMilliseconds)
        {
            std::function<void()> function = std::bind(&JavaModule::invokeJavaFunction, this, runnable);
            Module::registerPeriodicFunction(identifier, function, static_cast<size_t>(periodInMilliseconds));
        }

        void JavaModule::scheduleFunctionAtTime(std::string name, java::Runnable runnable, int32_t hour, int32_t minute, int32_t second, int32_t millisecond)
        {
            std::function<void()> function = std::bind(&JavaModule::invokeJavaFunction, this, runnable);
            Module::scheduleFunctionAtTime(name, function, hour, minute, second, millisecond);
        }

        void JavaModule::scheduleFunctionInXDays(std::string name, java::Runnable runnable, int32_t days)
        {
            std::function<void()> function = std::bind(&JavaModule::invokeJavaFunction, this, runnable);
            Module::scheduleFunctionInXDays(name, function, days);
        }

        void JavaModule::scheduleFunctionInXHours(std::string name, java::Runnable runnable, int32_t hours)
        {
            std::function<void()> function = std::bind(&JavaModule::invokeJavaFunction, this, runnable);
            Module::scheduleFunctionInXHours(name, function, hours);
        }

        void JavaModule::scheduleFunctionInXMinutes(std::string name, java::Runnable runnable, int32_t minutes)
        {
            std::function<void()> function = std::bind(&JavaModule::invokeJavaFunction, this, runnable);
            Module::scheduleFunctionInXMinutes(name, function, minutes);
        }

        void JavaModule::scheduleFunctionInXSeconds(std::string name, java::Runnable runnable, int32_t seconds)
        {
            std::function<void()> function = std::bind(&JavaModule::invokeJavaFunction, this, runnable);
            Module::scheduleFunctionInXSeconds(name, function, seconds);
        }

        void JavaModule::scheduleFunctionInXMilliSeconds(std::string name, java::Runnable runnable, int32_t seconds)
        {
            std::function<void()> function = std::bind(&JavaModule::invokeJavaFunction, this, runnable);
            Module::scheduleFunctionInXMilliSeconds(name, function, seconds);
        }

        void JavaModule::unregisterPeriodicFunction(std::string identifier)
        {
            Module::unregisterPeriodicFunction(identifier);
        }

        void JavaModule::invokeJavaFunction(java::Runnable runnable)
        {
            runnable.run();
        }

        JNIEnv* JavaModule::getEnv()
        {
            return java::JNIUtils::getEnv();
        }

        const std::string JavaModule::getModuleName()
        {
            jobject self = java::cast(this);
            JNIEnv* env = java::JNIUtils::getEnv();

            return java::JNIUtils::getNameOfClassOfObject(env, self);
        }


    }
}

REGISTER_MODULE(claid::JavaWrapper::JavaModule)