#pragma once

#include "Utilities/Singleton.hpp"
#include "JavaWrapper/WrapperBase.hpp"
#include "JavaWrapper/Wrapper.hpp"

#include <map>
#include <string>

namespace portaible
{
    namespace JavaWrapper
    {
        class WrapperMaster : public Singleton<WrapperMaster>
        {
            std::map<std::string, WrapperBase*> registeredWrappers;

            public:
                bool isWrapperRegisteredForClass(const std::string& javaClassName)
                {
                    auto it = registeredWrappers.find(javaClassName);

                    return it != registeredWrappers.end();
                }

                template<typename T>
                void registerWrapper(std::string javaClassName)
                {
                    if (isWrapperRegisteredForClass(javaClassName))
                    {
                        PORTAIBLE_THROW(portaible::Exception, "Error, wrapper for class \"" << javaClassName << "\" was registered more than once");
                    }
                    Wrapper<T>* wrapper = new Wrapper<T>(javaClassName);
                    this->registeredWrappers.insert(std::make_pair(javaClassName, static_cast<WrapperBase*>(wrapper)));
                }

                WrapperBase* getWrapper(std::string javaClassName)
                {
                    auto it = registeredWrappers.find(javaClassName);

                    if(it == registeredWrappers.end())
                    {
                        return nullptr;
                    }

                    return it->second;
                }

        };


		template<typename T>
		class RegisterHelper
		{

			public:
				RegisterHelper(std::string name) 
				{
					WrapperMaster::getInstance()->registerWrapper<T>(name);
				}

	
		};
    }
}

#define DECLARE_WRAPPER(className) \
	static volatile portaible::JavaWrapper::RegisterHelper<className> classFactoryRegistrar;\
	static const std::string __CLASS_NAME__;\
    static const std::string __JAVA_CLASS_NAME__;\
    const std::string& getClassName() {return __CLASS_NAME__;} \
	const std::string& getJavaClassName() {return __JAVA_CLASS_NAME__;}


#define REGISTER_WRAPPER(javaClassName, className, wrapperName) \
	volatile portaible::JavaWrapper::RegisterHelper<className> wrapperName::classFactoryRegistrar (std::string(javaClassName));\
    const std::string __CLASS_NAME__ = #className;\
	const std::string wrapperName__JAVA_CLASS_NAME__ = javaClassName;


#define LAZY_WRAPPER(javaClassName, className) \
    using namespace portaible;\
    using namespace portaible::JavaWrapper;\
    namespace portaible \
    {\
        class className##Wrapper : public Wrapper<className>\
        {\
            DECLARE_WRAPPER(className)\
        };\
    }\
    REGISTER_WRAPPER(javaClassName, className, portaible::className##Wrapper)
