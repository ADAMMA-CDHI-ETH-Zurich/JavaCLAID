#pragma once

#include "GenericJavaReflector.hpp"
#include "jbind11/JNIUtils/JNIUtils.hpp"
#include "JavaWrapperMaster.hpp"
#include "Signatures.hpp"
namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        template<typename Reflector>
        class TypedJavaReflector : public GenericJavaReflector
        {  
            private:
                jobject objectToReflect;
                Reflector* reflector;


                // Actually the member is not really needed, since we use the string to look up the field ID.
                // But its helpful to ensure that the user really specifies a variable on the java side.
                virtual void reflectFromJava(std::string memberFieldName, jobject member)
                {
                    std::cout << "TypedReflect reflectFromJava called\n";
                    JNIEnv* env = java::JNIUtils::getEnv();

                    std::string className = java::JNIUtils::getNameOfClassOfObject(env, this->objectToReflect);


                    if(member == nullptr)
                    {
                        std::string className = java::JNIUtils::getNameOfClassOfObject(env, this->objectToReflect);
                        CLAID_THROW(claid::Exception, "Error, cannot reflect member \"" << memberFieldName << "\" of Java class \"" << className << "\".\n"
                        << "The object \"" << memberFieldName << "\" has not been initialized.\n"
                        << "Please initialize the object using " << memberFieldName << " = new ...");
                    }
                    std::string memberClassName = java::JNIUtils::getNameOfClassOfObject(env, member);

                    if(isPrimitive(memberClassName))
                    {
                        handlePrimitive(memberFieldName, member);
                    }
                    else
                    {
                        handleWithWrapper(memberFieldName, member);
                    }
                };

                template<typename T>
                void javaPrimitiveSetter(std::string memberFieldName, jobject member, const T& value)
                {
                    JNIEnv* env = java::JNIUtils::getEnv();
                    jfieldID fieldID = java::JNIUtils::getFieldOfClassOfObject(env, this->objectToReflect, memberFieldName, java::JNIUtils::getSignatureOfPrimitiveType<T>());

                    if(fieldID == nullptr)
                    {
                        std::string className = java::JNIUtils::getNameOfClassOfObject(env, objectToReflect);
                        CLAID_THROW(claid::Exception, "Error, cannot set member \"" << memberFieldName << "\" of Java class \"" << className << "\"."
                        << "The class does not have a member field called \"" << memberFieldName << "\" that has signature \"" << java::JNIUtils::getSignatureOfPrimitiveType<T>() << "\"."); 
                    }

                    java::JNIUtils::setPrimitiveField<T>(env, this->objectToReflect, fieldID, value);
                }

                template<typename T>
                T javaPrimitiveGetter(std::string memberFieldName, jobject member)
                {
                    JNIEnv* env = java::JNIUtils::getEnv();
                    jfieldID fieldID = java::JNIUtils::getFieldOfClassOfObject(env, this->objectToReflect, memberFieldName, java::JNIUtils::getSignatureOfPrimitiveType<T>());

                    if(fieldID == nullptr)
                    {
                        std::string className = java::JNIUtils::getNameOfClassOfObject(env, this->objectToReflect);
                        CLAID_THROW(claid::Exception, "Error, cannot get member \"" << memberFieldName << "\" of Java class \"" << className << "\"."
                        << "The class does not have a member field called \"" << memberFieldName << "\" that has signature \"" << java::JNIUtils::getSignatureOfPrimitiveType<T>() << "\"."); 
                    }

                    return java::JNIUtils::getPrimitiveField<T>(env, this->objectToReflect, fieldID);
                }

                template<typename T>
                void javaObjectSetter(std::string memberFieldName, jobject member, const T& value)
                {
                    JNIEnv* env = java::JNIUtils::getEnv();
                    std::string signature = Signatures::Class::classNameToEncapsulatedSignature(java::Caster<T>::canonicalTypeName());
                    jfieldID fieldID = java::JNIUtils::getFieldOfClassOfObject(env, this->objectToReflect, memberFieldName, signature);

                    if(fieldID == nullptr)
                    {
                        std::string className = java::JNIUtils::getNameOfClassOfObject(env, objectToReflect);
                        CLAID_THROW(claid::Exception, "Error, cannot set member \"" << memberFieldName << "\" of Java class \"" << className << "\"."
                        << "The class does not have a member field called \"" << memberFieldName << "\" that has signature \"" << signature << "\"."); 
                    }

                    std::string& nonConstString = *const_cast<std::string*>(&value);
                    env->SetObjectField(this->objectToReflect, fieldID, java::cast<std::string>(nonConstString));
                }

                template<typename T>
                T javaObjectGetter(std::string memberFieldName, jobject member)
                {
                    JNIEnv* env = java::JNIUtils::getEnv();
                    std::string signature = Signatures::Class::classNameToEncapsulatedSignature(java::Caster<T>::canonicalTypeName());
                    jfieldID fieldID = java::JNIUtils::getFieldOfClassOfObject(env, this->objectToReflect, memberFieldName, signature);

                    if(fieldID == nullptr)
                    {
                        std::string className = java::JNIUtils::getNameOfClassOfObject(env, this->objectToReflect);
                        CLAID_THROW(claid::Exception, "Error, cannot get member \"" << memberFieldName << "\" of Java class \"" << className << "\"."
                        << "The class does not have a member field called \"" << memberFieldName << "\" that has signature \"" << signature << "\"."); 
                    }

                    return java::fromJavaObject<T>(env->GetObjectField(this->objectToReflect, fieldID));
                }

                bool isPrimitive(const std::string& className)
                {
                    return  className == "java.lang.Byte" ||
                            className == "java.lang.Short" ||
                            className == "java.lang.Integer" ||
                            className == "java.lang.Long" ||
                            className == "java.lang.Float" ||
                            className == "java.lang.Double" ||
                            className == "java.lang.Character" ||
                            className == "java.lang.Boolean" ||
                            className == "java.lang.String";
                
                }
                
                template<typename T>
                void reflectPrimitive(std::string memberFieldName, jobject member)
                {
                    Getter<T> getter(&TypedJavaReflector::javaPrimitiveGetter<T>, this, memberFieldName, member);
                    Setter<T> setter(&TypedJavaReflector::javaPrimitiveSetter<T>, this, memberFieldName, member);
                    reflector->member(memberFieldName.c_str(), getter, setter);
                }

                void reflectString(std::string memberFieldName, jobject member)
                {
                    typedef std::string T;
                    Getter<T> getter(&TypedJavaReflector::javaObjectGetter<T>, this, memberFieldName, member);
                    Setter<T> setter(&TypedJavaReflector::javaObjectSetter<T>, this, memberFieldName, member);
                    reflector->member(memberFieldName.c_str(), getter, setter);
                }

                void handlePrimitive(std::string memberFieldName, jobject member)
                {
                    JNIEnv* env = java::JNIUtils::getEnv();
                    std::string className = java::JNIUtils::getNameOfClassOfObject(env, member);

                    if(className == "java.lang.Byte")
                    {
                        reflectPrimitive<int8_t>(memberFieldName, member);
                    }
                    else if(className == "java.lang.Short")
                    {
                        reflectPrimitive<int16_t>(memberFieldName, member);
                    }
                    else if(className == "java.lang.Integer")
                    {
                        reflectPrimitive<int32_t>(memberFieldName, member);
                    }
                    else if(className == "java.lang.Long")
                    {
                        reflectPrimitive<int64_t>(memberFieldName, member);
                    }
                    else if(className == "java.lang.Float")
                    {
                        reflectPrimitive<float>(memberFieldName, member);
                    }
                    else if(className == "java.lang.Double")
                    {
                        reflectPrimitive<double>(memberFieldName, member);
                    }
                    else if(className == "java.lang.Character")
                    {
                        reflectPrimitive<char16_t>(memberFieldName, member);
                    }
                    else if(className == "java.lang.Boolean")
                    {
                        reflectPrimitive<bool>(memberFieldName, member);
                    }
                    else if(className == "java.lang.String")
                    {
                        // Treat string as primitive.
                        reflectString(memberFieldName, member);
                    }
                }

                void handleWithWrapper(const std::string& memberFieldName, jobject member)
                {
                    JNIEnv* env = java::JNIUtils::getEnv();
                    std::string memberClassName = java::JNIUtils::getNameOfClassOfObject(env, member);

                    
                    claid::JavaWrapper::JavaWrapperBase* javaWrapper = claid::JavaWrapper::JavaWrapperMaster::getInstance()->getWrapperForJavaClass(memberClassName);
                    if(javaWrapper == nullptr)
                    {
                        std::string className = java::JNIUtils::getNameOfClassOfObject(env, this->objectToReflect);
                        CLAID_THROW(claid::Exception, "Error, cannot reflect member \"" << memberFieldName << "\" of Java class \"" << className << "\".\n"
                        << "A wrapper for class \"" << memberClassName << "\" was not found. Only primitive types or classes registered to the CLAID reflection system are supported for reflection.\n"
                        << "This means that on the java side, only Java classes are supported that have been generated from a native C++ type using jbind11.");
                    }
                    javaWrapper->forwardReflectorToNativeObject(memberFieldName.c_str(), *this->reflector, member);
                }

            public:
                TypedJavaReflector(jobject objectToReflect, Reflector* reflector) : reflector(reflector), objectToReflect(objectToReflect)
                {

                }
            
        };
    }
}