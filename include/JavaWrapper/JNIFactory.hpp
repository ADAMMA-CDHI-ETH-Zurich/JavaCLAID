#pragma once
#include "JNIUtils.hpp"
#include "JNIFactoryBase.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        // Allows to create a Java Object (jobject) from the provided C++ type, if possible.
        // E.g., int -> java.lang.Integer
        // std::string -> java.lang.String
        // std::vector -> java.lang.ArrayList

        

        // For any other class / Default case
        template<typename T, class Enable = void>
        struct JNIFactory : protected JNIFactoryBase
        {
            // default case
            static jobject spawn(JNIEnv* env, T& member) 
            {
                PORTAIBLE_THROW(Exception, "Error, cannot convert Native C++ class " << TypeChecking::getCompilerIndependentTypeNameOfClass<T>() << " to java object."
                    << "We do not know how to do the conversion (which Java object corresponds to this type?).");
            }

            // default case
            static jobject spawnImmutable(JNIEnv* env, T& member) 
            {
                PORTAIBLE_THROW(Exception, "Error, cannot convert Native C++ class " << TypeChecking::getCompilerIndependentTypeNameOfClass<T>() << " to java object."
                    << "We do not know how to do the conversion (which Java object corresponds to this type?).");
            }
        }; 

        // For primitive / arithmetic types and byte
        template<typename T>
        struct JNIFactory<T, typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<T, byte>::value>::type> : protected JNIFactoryBase
        {
            protected:
                static jobject createJavaPrimitiveObjectFromNativePrimitive(JNIEnv* env, T& nativePrimitive)
                {
                    // Java primitive objects like Integer, Boolean, Float etc. are usually immutable.
                    // However, from JNI, you can still change them by just adressing the corresponding value field.
                    std::string primitiveClassName =  Signatures::Primitive::getJavaClassNameOfPrimitiveType<T>();
                    jobject javaObject;
                    std::string signature = Signatures::Primitive::getSignatureOfPrimitiveType<T>();
                    Logger::printfln("Signature %s\n", signature.c_str());
                    newJavaObjectFromClassSignature(env, primitiveClassName, javaObject, Signatures::Primitive::getSignatureOfPrimitiveType<T>(), nativePrimitive);
                    return javaObject;
                }

            public:
            static jobject spawnImmutable(JNIEnv* env, T& nativeValue) 
            {
                jobject object = createJavaPrimitiveObjectFromNativePrimitive(env, nativeValue);
                return object;
            }

        }; 

        // For class std::string
        template<typename T>
        struct JNIFactory<T, typename std::enable_if<std::is_same<T, std::string>::value>::type> : protected JNIFactoryBase
        {
            static jobject spawnImmutable(JNIEnv* env, T& nativeValue) 
            {
                Logger::printfln("Spawning string !");
                jstring jString = JNIUtils::toJString(env, nativeValue);
                return jString;
            }

        };

        // For class std::vector
        template<typename T>
        struct JNIFactory<T, typename std::enable_if<is_specialization_of<T, std::vector>::value>::type> : protected JNIFactoryBase
        {
            static jobject spawn(JNIEnv* env) 
            {
                jobject javaObject = JNIUtils::createJavaVector(env, 0);
                Logger::printfln("Spawning vector !");
      
                return javaObject;
            }
        };

         

        

       
    }
}