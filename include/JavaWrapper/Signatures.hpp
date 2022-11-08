#pragma once

#include <jni.h>
#include <string>
#include <vector>

#include "Utilities/StringUtils.hpp"
#include "Traits/is_integer_no_bool.hpp"
#include "Utilities/byte.hpp"
namespace claid
{
    namespace JavaWrapper
    {
        namespace Signatures
        {
            namespace Class
            {
                #define CLASS_SIGNATURE(name) const std::string name = prefix + std::string(#name);
                static const std::string prefix = "JavaCLAID/";

                CLASS_SIGNATURE(Channel)
                CLASS_SIGNATURE(ChannelData)
                
                const std::string String = "java/lang/String";
                const std::string FloatClass = "java/lang/Float";
                const std::string JavaClass = "java/lang/Class";
                const std::string JavaObject = "java/lang/Object";
                const std::string Vector = "java/util/Vector";

                static std::string classNameToSignature(const std::string& javaClassName)
                {
                    std::string result = javaClassName;
                    stringReplaceAll(result, ".", "/");
                    return result;
                }

                static std::string signatureToClassName(const std::string& javaClassName)
                {
                    std::string result = javaClassName;
                    stringReplaceAll(result, "/", ".");
                    return result;
                }
            }

            namespace Primitive
            {
                template<typename T>
                static typename std::enable_if<std::is_same<T, CLAID::byte>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    return "java/lang/Byte";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    return "java/lang/Short";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    return "java/lang/Integer";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, long>::value || std::is_same<T, unsigned long>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    // Yes, it's "J", because "L" is used for clases.
                    return "java/lang/Long";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    // Yes, it's "J", because "L" is used for clases.
                    return "java/lang/Long";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, float>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    return "java/lang/Float";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, double>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    return "java/lang/Double";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    return "java/lang/Character";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, bool>::value, std::string>::type
                getJavaClassNameOfPrimitiveType()
                {
                    return "java/lang/Boolean";
                }

                // =====================================================================

                // Why not use int8_t? Because int8_t might be defined as signed char, depending on the compiler
                // See: https://stackoverflow.com/questions/16503373/difference-between-char-and-signed-char-in-c
                template<typename T>
                static typename std::enable_if<std::is_same<T, CLAID::byte>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "B";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "S";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "I";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, long>::value || std::is_same<T, unsigned long>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    // Yes, it's "J", because "L" is used for clases.
                    return "J";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    // Yes, it's "J", because "L" is used for clases.
                    return "J";
                }
                
                template<typename T>
                typename std::enable_if<std::is_same<T, float>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "F";
                }

                template<typename T>
                typename std::enable_if<std::is_same<T, double>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "D";
                }

                template<typename T>
                typename std::enable_if<std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "C";
                }

                template<typename T>
                typename std::enable_if<std::is_same<T, bool>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "Z";
                }

                

                

            }

            namespace Function
            {
                static std::string ENCAPSULATE(const std::string& val)
                {
                    // L and ; are always needed for signatures !! 
                    return std::string("L") + val + std::string(";");
                }

                static void paramsSignature(const std::vector<std::string>& params, std::string& output)
                {
                    output = "(";
                    for(size_t i = 0; i < params.size(); i++)
                    {
                        // Make sure that param becomes Lparam;

                        output += ENCAPSULATE(params[i]);

                    }

                    output += ")";
                }

                static const std::string functionSignature(std::vector<std::string> params, std::string returnType)
                {
                    // Example
                    //  (Ljava/lang/String;)LChannel/Channel

                    std::string signature;
                    paramsSignature(params, signature);



                    return signature + ENCAPSULATE(returnType);
                }

                static const std::string functionSignatureVoid(std::vector<std::string> params)
                {
                    // Example
                    //  (Ljava/lang/String;)LChannel/Channel

                    std::string signature;
                    paramsSignature(params, signature);

                    return signature + "V";
                }
            }
        }
    }
}