#pragma once

#include "Module/ModuleFactory/ModuleFactoryBase.hpp"
#include "JNIUtils.hpp"
#include "JNIFactoryBase.hpp"
#include "JavaModule.hpp"

namespace claid
{
	using namespace ModuleFactory;
	namespace JavaWrapper
	{
		class JavaModuleFactory : public ModuleFactoryBase
		{
			private:
				JavaVM* javaVM;

				// The name of the Java Class of the Module.
				std::string javaModuleClassName;

			public:
				JavaModuleFactory(std::string javaModuleClassName, JavaVM* javaVM) : javaModuleClassName(javaModuleClassName), javaVM(javaVM)
				{

				}

				BaseModule* getInstanceUntyped()
				{
					return static_cast<BaseModule*>(getInstance());
				}

				JavaModule* getInstance()
				{
					if(javaVM == nullptr)
					{
						CLAID_THROW(claid::Exception, "Error in JavaModuleFactory. Cannot spawn Module\"" << javaModuleClassName << "\", because JavaVM is null. Invalid JavaVM was provided to JavaModuleFactory.\n"
						"Possibly, JNIUtils::onLoad() was not called in JNI_onLoad() ?");
					}

					JNIEnv* env = this->getEnv();

					if(env == nullptr)
					{
						CLAID_THROW(claid::Exception, "Error in JavaModuleFactory. Cannot spawn Module\"" << javaModuleClassName << "\", because we were unable to get a java environment.");
					}

					jobject javaModuleObject;
					JNIFactoryBase::newJavaObjectFromClassSignature(env, this->javaModuleClassName, javaModuleObject, "");
					JavaModule* javaModule = new JavaModule(this->javaVM, javaModuleObject);
					return javaModule;
				}

				JNIEnv* getEnv() 
				{
					JNIEnv* env;
					int status = this->javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
					if(status < 0) {    
						status = this->javaVM->AttachCurrentThread((void**)&env, NULL);
						if(status < 0) {        
							return nullptr;
						}
					}
					return env;
				}
		};
		
	}
}