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

#include "JbindWrapperGeneratorBase.hpp"

namespace claid
{
    namespace JavaWrapper
    {
        template<typename Class>
        class JbindWrapperGenerator : public JbindWrapperGeneratorBase // No need to inherit from Deserializer or Serializer? Probably not, as we only care about static members.
        {

          
            public:

                typedef java::JavaClass<Class> JClass;

                template<typename T>
                void member(const char*, T& member, std::string comment)
                {

                }

                template<typename T>
                void member(const char*, T& member, std::string comment, T defaultValue)
                {

                }

                

                template<typename T>
                void member(const char*, T& member)
                {

                }

                template<typename T>
                void reflect_member(std::string name, T& mem)
                {
                    // no need to do anything 
                    printf("Reflected member %s\n", name.c_str());
                }

                template<typename ClassType, typename Type>
                void pointer_to_member(std::string name, Type ClassType::*ptr)
                {
                    initJavaClassIfNecessary();
                    printf("\t\t Registering member %s to class %s\n", name.c_str(), className.c_str());
                    cls->def_readwrite(name.c_str(), ptr);
                    staticMemberRegisted = true;
                }

                template<typename ReturnType, typename ClassType, typename... Args>
                void pointer_to_function(std::string name, ReturnType (ClassType::*ptr)(Args...))
                {
                    initJavaClassIfNecessary();
                    printf("\t\t Registering function %s to class %s\n", name.c_str(), className.c_str());
                    cls->def(name.c_str(), ptr);
                    staticMemberRegisted = true;
                }

                template<typename ReturnType, typename ClassType, typename... Args>
                void pointer_to_function(std::string name, ReturnType (ClassType::*ptr)(Args...) const)
                {
                    initJavaClassIfNecessary();
                    printf("\t\t Registering function %s to class %s\n", name.c_str(), className.c_str());
                    cls->def(name.c_str(), ptr);
                    staticMemberRegisted = true;
                }

                template<typename... Extras>
                JClass* createJavaClass(java::JavaPackage& package, const std::string& className, Extras... extras)
                {
                    return new JClass(package, className.c_str(), extras...);
                }
                
                template<typename... Extras>
                void initJavaClassIfNecessary(Extras... extras)
                {
                    if(this->cls == nullptr)
                    {
                        this->cls = createJavaClass(*this->javaPackage, className, extras...);
                       // this->cls->def(py::init<>());
                    }
                }

                void generate(java::JavaPackage& package, const std::string& className)
                {
                    this->className = className;
                    staticMemberRegisted = false;
                    // Sadly, need to create an instance of the class here, because we have no means of statically calling the functions, yet.
                    Class p;
                    
                    this->javaPackage = &package;
                    // Why do we not call initJavaClassIfNecessary() first?
                    // Because this allows classes to implement custom JClass definitions in their reflect functions, if they want to.
                    p.reflect(*this);           

                    // if cls is still nullptr, that means that the class did not have any members reflected.
                    // Therefore, we create a wrapper for a python class without members.
                    // Well, who are we to doubt.
                    if(cls == nullptr)
                    {
                        this->initJavaClassIfNecessary();
                    }

                   // this->cls->attr("__CPP_CLASS_NAME__") = className;

                    if(cls == nullptr)
                    {
                        CLAID_THROW(Exception, "Error in JbindWrapperGenerator. PyClass is null, apparently no PyClass was created for object \"" << className << "\".");
                    }
                    delete cls;
                    cls = nullptr;

                    this->javaPackage = nullptr;

                    if(!staticMemberRegisted)
                    {
                        //Logger::printfln("Warning: A wrapper was created for class %s, however no members were registered! Either the class is using the old reflection system, or it really has no members");
                    }
                }

                JClass* getJavaClass()
                {
                    return cls;
                }


                void read(char*& data, size_t size)
                {

                }

                void write(const char* data, size_t size)
                {
                    
                }

                private:
                    JClass* cls = nullptr;
                    java::JavaPackage* javaPackage = nullptr;

                    std::string className;
                    bool staticMemberRegisted;

            
        };
    }
}