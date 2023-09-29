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

#include "JavaWrapper/JavaWrapperMaster.hpp"

namespace claid
{
    namespace JavaWrapper
    {
   
        bool JavaWrapperMaster::isWrapperRegisteredForNativeClass(const std::string& cppClassName)
        {
            auto it = this->registeredWrappers.find(cppClassName);
            return it != this->registeredWrappers.end();
        }

        bool JavaWrapperMaster::isWrapperRegisteredForJavaClass(const std::string& canonicalClassName)
        {
            for(auto& entry : this->registeredWrappers)
            {
                if(entry.second->getCanonicalJavaClassName() == canonicalClassName)
                {
                    return true;
                }
            }
            return false;
        }

        JavaWrapperBase* JavaWrapperMaster::getWrapperForNativeClass(const std::string& className)
        {
            auto it = this->registeredWrappers.find(className);

            if(it == this->registeredWrappers.end())
            {
                return nullptr;
            }

            return it->second;
        }


        JavaWrapperBase* JavaWrapperMaster::getWrapperForJavaClass(const std::string& canonicalClassName)
        {
            for(auto& entry : this->registeredWrappers)
            {
                if(entry.second->getCanonicalJavaClassName() == canonicalClassName)
                {
                    return entry.second;
                }
            }
            return nullptr;
        }

        void JavaWrapperMaster::getRegisteredWrapperClasses(std::vector<std::string>& wrappers)
        {
            wrappers.clear();
            for(auto it : registeredWrappers)
            {
                wrappers.push_back(it.first);
            }
        }
    }
}