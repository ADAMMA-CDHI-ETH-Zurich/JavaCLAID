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

#include "Module/ModuleFactory/ModuleFactoryBase.hpp"
#include "JavaModule.hpp"
#include "jbind11/jbind11.hpp"

namespace java = jbind11;

namespace claid
{
	namespace JavaWrapper
	{
		class JavaModuleFactory : public ModuleFactoryBase
		{
			private:

				// The name of the Java Class of the Module.
				std::string javaModuleClassName;

			public:
				JavaModuleFactory(std::string javaModuleClassName) : javaModuleClassName(javaModuleClassName)
				{

				}

				BaseModule* getInstanceUntyped()
				{
					return static_cast<BaseModule*>(getInstance());
				}

				JavaModule* getInstance()
				{
					JNIEnv* env = this->getEnv();

					if(env == nullptr)
					{
						CLAID_THROW(claid::Exception, "Error in JavaModuleFactory. Cannot spawn Module\"" << javaModuleClassName << "\", because we were unable to get a java environment.");
					}

					jobject javaModuleObject;
					JavaModule* javaModule = nullptr;

					try
					{
						javaModuleObject = java::JNIUtils::createObjectFromClassName(env, this->javaModuleClassName, "");
						javaModuleObject = env->NewGlobalRef(javaModuleObject);

						// When we create an instance of a class that was created using jbind11 from C++,
						// the jobject that is passed to JBindWrapper_init(...) is only temporary and not valid after init anymore.
						// The valid reference is the one we got from createObjectFromClassName.
						// Hence, we need to override the stored reference in the handle.
						java::JavaHandle handle = java::JavaHandle::getHandleFromObject(java::JNIUtils::getEnv(), javaModuleObject);
						handle.getData()->setJavaObjectReference(javaModuleObject);
						
						// The JavaModule class is just a class created using jbind11, just like every other wrapped java class.
						// Therefore, when that object is constructed, it automatically creates a instance of the wrapped C++ object aswell,
						// which is stored in a JavaHandle.
						// Therefore, we can just cast the jobject to the corresponding C++ class.
						// However, make sure to cast as pointer, otherwise the native object will be copied, which is not what we want.

						// Uses pointer caster to get a reference to the JavaModule* instance which is stored
						// in the JavaHandle of the javaModuleObject.
						// For this, jbind11 will look up in a global table, which JavaHandle is associated with javaModuleObject.
						javaModule = java::fromJavaObject<JavaModule*>(javaModuleObject);
					}
					catch(const std::exception& ex)
					{
						// Rethrow jbind exception as CLAID exception.
						env->ExceptionClear();
						CLAID_THROW(claid::Exception, ex.what());
					}
					return javaModule;
				}

				JNIEnv* getEnv() 
				{
					return java::JNIUtils::getEnv();
				}
		};
		
	}
}
