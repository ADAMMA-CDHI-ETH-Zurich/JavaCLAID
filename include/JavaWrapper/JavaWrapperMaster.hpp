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
		class RegisterHelper
		{
			public:
				RegisterHelper(std::string name) 
				{
					JavaWrapperMaster::getInstance()->registerWrapper<Class>(name);
				}
		};
    }
}

// Solving cyclic dependencies is !fun.
#include "JavaWrapper.hpp"

#define DECLARE_JAVA_WRAPPER(className) \
	static volatile claid::JavaWrapper::RegisterHelper<claid::JavaWrapper::JavaWrapper<className>> wrapperRegistrar;\

#define REGISTER_JAVA_WRAPPER(fullyQualifiedClassName) \
	volatile claid::JavaWrapper::RegisterHelper<claid::JavaWrapper::JavaWrapper<fullyQualifiedClassName>> fullyQualifiedClassName::wrapperRegistrar (std::string(#fullyQualifiedClassName));\

#define LAZY_PYTHON_WRAPPER(fullyQualifiedClassName, className) \
    class className##JavaWrapper##Helper\
    {\
        REGISTER_JAVA_WRAPPER(fullyQualifiedClassName)\
    };\
    volatile claid::JavaWrapper::RegisterHelper<claid::JavaWrapper::JavaWrapper<fullyQualifiedClassName>> className##Wrapper##Helper::wrapperRegistrar(std::string(#fullyQualifiedClassName));\
    // Note, that in contrast to REGISTER_JAVA_WRAPPER, here we use another constructor of the registrar, which already matches the corresponding javaClassName to the C++ className.
