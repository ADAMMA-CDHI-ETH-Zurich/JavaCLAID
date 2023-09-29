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

#include "Exception/Exception.hpp"
#include "JavaWrapper/JavaWrapperBase.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/StringUtils.hpp"
#include <map>
#include <string>

namespace claid
{
    namespace JavaWrapper
    {
        class JavaWrapperMaster : public Singleton<JavaWrapperMaster>
        {
            // string is the C++ class name (fully qualified name).
            std::map<std::string, JavaWrapperBase*> registeredWrappers;


            public:
                
                template<typename WrapperType>
                void registerWrapper(std::string cppClassName)
                {
                    if (isWrapperRegisteredForNativeClass(cppClassName))
                    {
                       // Not an error. This might happen when importing shared libraries that also were build with CLAID (e.g., importing PyCLAID from a PythonModule).
                       // return;
                       // CLAID_THROW(claid::Exception, "Error, wrapper for class \"" << cppClassName << "\" was registered more than once");
                    }
                    WrapperType* wrapper = new WrapperType(cppClassName);
                    this->registeredWrappers.insert(std::make_pair(cppClassName, static_cast<JavaWrapperBase*>(wrapper)));
                }  

                bool isWrapperRegisteredForNativeClass(const std::string& cppClassName);
                bool isWrapperRegisteredForJavaClass(const std::string& canonicalClassName);

                JavaWrapperBase* getWrapperForNativeClass(const std::string& className);
                JavaWrapperBase* getWrapperForJavaClass(const std::string& className);
                
                void getRegisteredWrapperClasses(std::vector<std::string>& wrappers);              
        };


		template<typename Class>
		class JavaWrapperRegistrar
		{
			public:
				JavaWrapperRegistrar(std::string name) 
				{
					JavaWrapperMaster::getInstance()->registerWrapper<Class>(name);
				}
		};
    }
}

// Solving cyclic dependencies is !fun.
#include "JavaWrapper.hpp"


#define REGISTER_JAVA_WRAPPER(fullyQualifiedClassName)\
namespace\
{\
    static volatile claid::JavaWrapper::JavaWrapperRegistrar<claid::JavaWrapper::JavaWrapper<fullyQualifiedClassName>> wrapperRegistrar (std::string(#fullyQualifiedClassName));\
}
// #define LAZY_PYTHON_WRAPPER(fullyQualifiedClassName, className) \
//     class className##JavaWrapper##Helper\
//     {\
//         REGISTER_JAVA_WRAPPER(fullyQualifiedClassName)\
//     };\
//     volatile claid::JavaWrapper::RegisterHelper<claid::JavaWrapper::JavaWrapper<fullyQualifiedClassName>> className##Wrapper##Helper::wrapperRegistrar(std::string(#fullyQualifiedClassName));\
//     // Note, that in contrast to REGISTER_JAVA_WRAPPER, here we use another constructor of the registrar, which already matches the corresponding javaClassName to the C++ className.
