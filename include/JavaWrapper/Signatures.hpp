#pragma once

#include <jni.h>
#include <string>
#include <vector>

#include "Utilities/StringUtils.hpp"
#include "Traits/is_integer_no_bool.hpp"
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
                    StringUtils::stringReplaceAll(result, ".", "/");
                    return result;
                }

                static std::string signatureToClassName(const std::string& javaClassName)
                {
                    std::string result = javaClassName;
                    StringUtils::stringReplaceAll(result, "/", ".");
                    return result;
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