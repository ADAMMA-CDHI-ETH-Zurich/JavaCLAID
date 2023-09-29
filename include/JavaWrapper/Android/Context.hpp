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
 
#if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)

#include "jbind11/jbind11.hpp"
namespace java = jbind11;

class AAssetManager;

#if defined(__ANDROID__)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

namespace claid
{
    namespace JavaWrapper
    {   
        class Context
        {
            private:
                jobject contextObject = nullptr;
                std::string CONTEXT_CLASS_NAME = "android.content.Context";
                std::string APPLICATION_CONTEXT_CLASS_NAME = "android.app.Application";


            public:
                Context(jobject contextObject) : contextObject(contextObject)
                {
                }

                Context()
                {
                }

                jobject getJavaObject()
                {
                    return contextObject;
                }

                jobject getAssetManagerObject()
                {
                    #if defined(__ANDROID__)
                    JNIEnv* env = java::JNIUtils::getEnv();
                    jmethodID getAssetsMethod = java::JNIUtils::getMethodID(env, this->contextObject, "getAssets", "()Landroid/content/res/AssetManager;");

                    if(getAssetsMethod == nullptr)
                    {
                        JBIND_THROW("Error, Context.getAssets() failed. Cannot find \"getAssets\" function in Java object.\n"
                        "Is the internal java object really a context object?");
                    }

                    // AAssetManager object has to be freed manually.
                    return env->CallObjectMethod(this->contextObject, getAssetsMethod);
                    #else
                        return nullptr;
                    #endif
                }

                bool isValid()
                {
                    claid::Logger::printfln("1");
                    if(contextObject == nullptr)
                    {
                        claid::Logger::printfln("is nullptr");

                        return false;
                    }

                    #if defined(__ANDROID__)

                    JNIEnv* env = java::JNIUtils::getEnv();
                    std::string className = java::JNIUtils::getNameOfClassOfObject(env, contextObject);
                    claid::Logger::printfln("%s\n", className.c_str());
                    if(className == CONTEXT_CLASS_NAME || className == APPLICATION_CONTEXT_CLASS_NAME)
                    {
                        return true;
                    }

                    #endif

                    return false;
                }
        };
    }
    
}


#endif