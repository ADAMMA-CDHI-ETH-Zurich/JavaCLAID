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
#include "jbind11/jbind11.hpp"

namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        class JbindWrapperGeneratorBase // No need to inherit from Deserializer or Serializer? Probably not, as we only care about static members.
        {
            public:
                virtual void generate(java::JavaPackage& package, const std::string& className) = 0;
        };
    }
}