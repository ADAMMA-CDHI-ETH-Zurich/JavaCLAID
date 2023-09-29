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


#include <string>
#include "ClassFactory/ClassFactory.hpp"

#include "jbind11/jbind11.hpp"
namespace java = jbind11;



#include "JavaWrapper/JavaModule.hpp"


using namespace claid;
using namespace claid::JavaWrapper;
JBIND11_PACKAGE(JavaCLAIDDataTypes, p) 
{
    // It is safe to assume that all classes have been statically
    // registered to the JavaWrapperMaster because the JBIND11_PACKAGE declaration is executed.
    // The reason for that is that all packages definitions are executed by jbind11 when
    // after shared library has been loaded by the java runtime, whereas all static class registrations
    // are done at link time (dlopen -> static registrations -> JNI_OnLoad()).
    std::vector<std::string> registeredClasses;

    JavaWrapperMaster::getInstance()->getRegisteredWrapperClasses(registeredClasses);


    printf("Getting registered classes\n");
    for(const std::string& str : registeredClasses)
    {
        JavaWrapperBase* wrapper = 
            JavaWrapperMaster::getInstance()->getWrapperForNativeClass(str);
        wrapper->generateWrapper(p);
    }

}
