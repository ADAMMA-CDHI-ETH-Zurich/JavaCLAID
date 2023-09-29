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
#include "JavaWrapper/Android/Context.hpp"

#if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)

#include "jbind11/Caster/Caster.hpp"
using namespace jbind11;

namespace jbind11
{
    template<typename T>
    struct Caster<T, typename std::enable_if<std::is_same<T, claid::JavaWrapper::Context>::value>::type>
    {
        // T is jobject.
        public:
            static claid::JavaWrapper::Context fromJavaObject(JNIEnv* env, jobject javaObject)
            {
                return claid::JavaWrapper::Context(javaObject);
            }

            static jobject cast(JNIEnv* env, claid::JavaWrapper::Context& value)
            {
                return value.getJavaObject();
            }

            static std::string canonicalTypeName()
            {
                return "android.content.Context";
            }

            static bool isGeneric()
            {
                return false;
            }
    };
}

#endif