#pragma once

#include "Utilities/Singleton.hpp"
#include "JavaWrapper/WrapperBase.hpp"

#include <map>
#include <string>

namespace claid
{
    namespace JavaWrapper
    {
        class WrapperMaster : public Singleton<WrapperMaster>
        {
            private:
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
                        //CLAID_THROW(claid::Exception, "Error, wrapper for class \"" << cppClassName << "\" was registered more than once");
                    }
                    WrapperType* wrapper = new WrapperType(cppClassName);
                    this->registeredWrappers.insert(std::make_pair(cppClassName, static_cast<WrapperBase*>(wrapper)));
                }

                // Assign wrapper if not done already:
                void assignWrapperToJavaClassOfObject(JNIEnv* env, jobject object)
                {
                    jclass cls = JNIUtils::getClassOfObject(env, object);
                    assignWrapperToJavaClass(env, cls);
                    env->DeleteLocalRef(cls);
                    
                }

                void assignWrapperToJavaClass(JNIEnv* env, jclass cls)
                {
                    jfieldID fieldID = env->GetStaticFieldID(cls, "wrappedClass", "Ljava/lang/String;");

                    std::string className = JNIUtils::getClassName(env, cls);

                    if(fieldID == nullptr)
                    {
                        CLAID_THROW(claid::Exception, "Error in JavaWrapper. Tried to retrieve the name of the native class that Java class \"" << className << "\" is a wrapper for. "
                        << "For this, we tried to look up a static member variable called \"wrappedClass\", however the Java class does not have such a variable. "
                        << "When defining your wrapper class, please specify a variable private static String wrappedClass=\"namespace::class\", whereas the string specifies which class you want to wrap. "
                        << "If you defined this variable already, make sure it is *static* !");

                    }

                    jstring wrappedClass = (jstring) env->GetStaticObjectField(cls, fieldID);

                    if(wrappedClass == nullptr)
                    {
                        CLAID_THROW(claid::Exception, "Error in JavaWrapper. Tried to retrieve the name of the native class that Java class \"" << className << "\" is a wrapper for. "
                        << "For this, we tried to look up a static member variable called \"wrappedClass\". We found that the variable exists, but we were unable to retrieve its value. "
                        << "Is it really a String, or another object? "
                        << "When defining your wrapper class, please specify a variable private static String wrappedClass=\"namespace::class\", whereas the string specifies which class you want to wrap. ");
                    }

                    std::string cppClassName = JNIUtils::toStdString(env, wrappedClass);
                    Logger::printfln("Assigning to %s\n", cppClassName.c_str());
                    assignWrapperToJavaClassByName(className, cppClassName);
                }

                void assignWrapperToJavaClassByName(std::string javaClassName, std::string cppClassName)
                {
                    auto it = this->javaCppClassNamesMap.find(javaClassName);

                    if(it != javaCppClassNamesMap.end())
                    {
                        if(it->second != cppClassName)
                        {
                            // It was tried to register the same JavaClass again for a different C++  class.
                            CLAID_THROW(Exception, "Failed to assign wrapper to java class \"" << javaClassName << "\"."
                            << "It was tried to assign wrapper for C++ class \"" << cppClassName << "\" to java class \"" << javaClassName << "\", however "
                            << "there was already registered another C++ class as wrapper for this java class, namely  \"" << it->second << "\".");
                        }
                        else
                        {
                            // Else means it was tried to register the same C++ class multiple times as wrapper for the java class.
                            // We do nothing and just ignore it.
                            return ;
                        }   
                    }

                    auto it2 = this->registeredWrappers.find(cppClassName);

                    if(it2 == this->registeredWrappers.end())
                    {
                        CLAID_THROW(Exception, "Failed to assign wrapper to java class \"" << javaClassName << "\"."
                        << "It was tried to assign wrapper for C++ class \"" << cppClassName << "\" to java class \"" << javaClassName << "\", however "
                        << "the cpp class \"" << cppClassName << "\" does not exist or no Wrapper was implemented for it. "
                        << "If the class exists, did you register it using the REGISTER_SERIALIZATION macro?");
                    }

                    it2->second->assignJavaClassName(javaClassName);
                                        Logger::printfln("4\n");

                    this->javaCppClassNamesMap.insert(std::make_pair(javaClassName, cppClassName));
                                        Logger::printfln("5\n");

                }

                // string is the C++ class name
                const std::map<std::string, WrapperBase*>& getRegisteredWrappers()
                {
                    return this->registeredWrappers;
                }

                const std::map<std::string, std::string >& getJavaCppClassNamesMap()
                {
                    return this->javaCppClassNamesMap;
                }

                void getRegisteredWrapperClasses(std::vector<std::string>& classNames) const
                {
                    classNames.clear();

                    for(const auto value : this->registeredWrappers)
                    {
                        classNames.push_back(value.first);
                    }

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
                    WrapperMaster::getInstance()->assignWrapperToJavaClassByName(javaClassName, cppClassName);
				}

	
		};
    }
}

// Solving cyclic dependencies is !fun.

#include "Wrapper.hpp"

#define DECLARE_JAVA_WRAPPER(className) \
	static volatile claid::JavaWrapper::RegisterHelper<claid::JavaWrapper::Wrapper<className>> wrapperRegistrar;\

#define REGISTER_JAVA_WRAPPER(fullyQualifiedClassName) \
	volatile claid::JavaWrapper::RegisterHelper<claid::JavaWrapper::Wrapper<fullyQualifiedClassName>> fullyQualifiedClassName::wrapperRegistrar (std::string(#fullyQualifiedClassName));\

#define LAZY_WRAPPER(javaClassName, fullyQualifiedClassName, className) \
    class className##Wrapper##Helper\
    {\
        DECLARE_JAVA_WRAPPER(fullyQualifiedClassName)\
    };\
    volatile claid::JavaWrapper::RegisterHelper<claid::JavaWrapper::Wrapper<fullyQualifiedClassName>> className##Wrapper##Helper::wrapperRegistrar(std::string(#fullyQualifiedClassName), std::string(javaClassName));\
    // Note, that in contrast to REGISTER_JAVA_WRAPPER, here we use another constructor of the registrar, which already matches the corresponding javaClassName to the C++ className.
