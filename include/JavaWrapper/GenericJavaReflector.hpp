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
#include <string>
#include "jbind11/jbind11.hpp"

namespace claid
{
    namespace JavaWrapper
    {
        class GenericJavaReflector
        {
            public:
                GenericJavaReflector()
                {

                }

                ~GenericJavaReflector()
                {

                }

                static void addToJbindPackage(jbind11::JavaPackage& package)
                {
                    java::JavaClass<GenericJavaReflector> javaClass(package, "Reflector");

                    javaClass.def("reflect", &GenericJavaReflector::reflectFromJava, java::GenericFunction(), java::GenericParams({1}));
                    javaClass.def("reflectWithDefaultValue", &GenericJavaReflector::reflectFromJavaWithDefaultValue, java::GenericFunction(), java::GenericParams({1, 2}));

                //  pyClass.def("publish", &PythonModule::publish);
                }


                virtual void reflectFromJava(std::string memberFieldName, jobject member)
                {
                    std::cout << "GenericReflect reflectFromJava called\n";
                };

                virtual void reflectFromJavaWithDefaultValue(std::string memberFieldName, jobject member, jobject defaultValue)
                {
                    std::cout << "GenericReflect reflectFromJava called\n";
                };
        };
    };
}