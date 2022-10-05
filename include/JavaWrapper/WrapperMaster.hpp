#pragma once

#include "Utilities/Singleton.hpp"
#include "JavaWrapper/WrapperBase.hpp"

#include <map>
#include <string>

namespace portaible
{
    namespace JavaWrapper
    {
        class WrapperMaster : public Singleton<WrapperMaster>
        {
            // string is the C++ class name
            std::map<std::string, WrapperBase*> registeredWrappers;

            std::map<std::string /* JavaClassName */, std::string /* C++ class name */> javaCppClassNamesMap;

            public:
                
                bool isWrapperAssignedToJavaClass(const std::string& javaClassName);
                bool isWrapperRegisteredForNativeClass(const std::string& cppClassName);
                WrapperBase* getWrapperForJavaClass(std::string javaClassName);
                

                template<typename WrapperType>
                void registerWrapper(std::string cppClassName)
                {
                    if (isWrapperRegisteredForNativeClass(cppClassName))
                    {
                        // Not an error. This might happen when importing shared libraries that also were build with CLAID (e.g., importing PyCLAID from a PythonModule).
                        return;
                        //PORTAIBLE_THROW(portaible::Exception, "Error, wrapper for class \"" << cppClassName << "\" was registered more than once");
                    }
                    WrapperType* wrapper = new WrapperType(cppClassName);
                    this->registeredWrappers.insert(std::make_pair(cppClassName, static_cast<WrapperBase*>(wrapper)));
                }

                void assignJavaClassNameToCppClassName(std::string javaClassName, std::string cppClassName)
                {
                    auto it = this->javaCppClassNamesMap.find(javaClassName);

                    if(it != javaCppClassNamesMap.end())
                    {
                        if(it->second != cppClassName)
                        {
                            // It was tried to register the same JavaClass again for a different C++  class.
                            PORTAIBLE_THROW(Exception, "Failed to assign wrapper to java class " << javaClassName << "."
                            << "It was tried to assign wrapper for C++ class \"" << cppClassName << "\" to java class \"" << javaClassName << "\", however"
                            << "there was already registered another C++ class as wrapper for this java class, namely  \"" << it->second << "\".");
                        }
                        else
                        {
                            // Else means it was tried to register the same C++ class multiple times as wrapper for the java class.
                            // We do nothing and just ignore it.
                            return ;
                        }   
                    }
                    this->registeredWrappers[cppClassName]->assignJavaClassName(javaClassName);
                    this->javaCppClassNamesMap.insert(std::make_pair(javaClassName, cppClassName));
                }

                
        };


		template<typename WrapperType>
		class RegisterHelper
		{

			public:
				RegisterHelper(std::string name) 
				{
					WrapperMaster::getInstance()->registerWrapper<WrapperType>(name);
				}

                RegisterHelper(std::string cppClassName, std::string javaClassName) 
				{
					WrapperMaster::getInstance()->registerWrapper<WrapperType>(cppClassName);
                    WrapperMaster::getInstance()->assignJavaClassNameToCppClassName(javaClassName, cppClassName);
				}

	
		};
    }
}

// Solving cyclic dependencies is !fun.

#include "Wrapper.hpp"

#define DECLARE_JAVA_WRAPPER(className) \
	static volatile portaible::JavaWrapper::RegisterHelper<portaible::JavaWrapper::Wrapper<className>> wrapperRegistrar;\

#define REGISTER_JAVA_WRAPPER(fullyQualifiedClassName) \
	volatile portaible::JavaWrapper::RegisterHelper<portaible::JavaWrapper::Wrapper<fullyQualifiedClassName>> fullyQualifiedClassName::wrapperRegistrar (std::string(#fullyQualifiedClassName));\

#define LAZY_WRAPPER(javaClassName, fullyQualifiedClassName, className) \
    class className##Wrapper##Helper\
    {\
        DECLARE_JAVA_WRAPPER(fullyQualifiedClassName)\
    };\
    volatile portaible::JavaWrapper::RegisterHelper<portaible::JavaWrapper::Wrapper<fullyQualifiedClassName>> className##Wrapper##Helper::wrapperRegistrar(std::string(#fullyQualifiedClassName), std::string(javaClassName));\
    // Note, that in contrast to REGISTER_JAVA_WRAPPER, here we use another constructor of the registrar, which already matches the corresponding javaClassName to the C++ className.
