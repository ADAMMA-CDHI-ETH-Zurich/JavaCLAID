#pragma once

#include <jni.h>
#include <string>
#include <vector>

#include "Utilities/StringUtils.hpp"
#include "Traits/is_integer_no_bool.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        namespace Signatures
        {
            namespace Class
            {
                #define CLASS_SIGNATURE(name) const std::string name = prefix + std::string(#name);
                static const std::string prefix = "com/example/portaible/";

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
                static typename std::enable_if<is_integer_no_bool<T>::value, std::string>::type
                getJavaClassOfPrimitiveType()
                {
                    return "java/lang/Integer";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, float>::value, std::string>::type
                getJavaClassOfPrimitiveType()
                {
                    return "java/lang/Float";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, double>::value, std::string>::type
                getJavaClassOfPrimitiveType()
                {
                    return "java/lang/Double";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, char>::value, std::string>::type
                getJavaClassOfPrimitiveType()
                {
                    return "java/lang/Character";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, bool>::value, std::string>::type
                getJavaClassOfPrimitiveType()
                {
                    return "java/lang/Boolean";
                }

                // =====================================================================

                template<typename T>
                typename std::enable_if<is_integer_no_bool<T>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "I";
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
                typename std::enable_if<std::is_same<T, char>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "C";
                }

                template<typename T>
                typename std::enable_if<std::is_same<T, bool>::value, std::string>::type
                getSignatureOfPrimitiveType()
                {
                    return "B";
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