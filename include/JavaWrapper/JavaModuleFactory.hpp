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
					JNIEnv* env = this->getEnv();
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
						status = this->javaVM->AttachCurrentThread(&env, NULL);
						if(status < 0) {        
							return nullptr;
						}
					}
					return env;
				}
		};
		
	}
}