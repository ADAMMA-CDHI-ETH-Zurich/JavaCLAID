/***************************************************************************
* Copyright (C) 2023 ETH Zurich
* Core AI & Digital Biomarker, Acoustic and Inflammatory Biomarkers (ADAMMA)
* Centre for Digital Health Interventions (c4dhi.org)
* 
* Authors: Patrick Langer
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*         http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***************************************************************************/

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

                static std::string classNameToEncapsulatedSignature(const std::string& javaClassName)
                {
                    std::string result = "L" + javaClassName + ";";
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