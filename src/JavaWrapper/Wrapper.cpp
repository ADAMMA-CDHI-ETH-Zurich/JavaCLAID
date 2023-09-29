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

// #include "jbind11/JNIUtils.hpp"
// #include "Logger/Logger.hpp"
// #include "JavaWrapper/WrapperMaster.hpp"
// using namespace claid;
// using namespace claid::JavaWrapper;
// extern "C"
// {
//     JNIEXPORT void JNICALL Java_JavaCLAID_Wrapper_init(JNIEnv* env, jobject object)
//     {
//         // Assign wrapper if not done already:
//         WrapperMaster::getInstance()->assignWrapperToJavaClassOfObject(env, object);

//         std::string className = JNIUtils::getNameOfClassOfObject(env, object);
//         if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
//         {
//             CLAID_THROW(Exception, "Error, init was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
//         }

//         WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);

//         wrapper->assignInstance(env, object);
//     }

//     JNIEXPORT void JNICALL Java_JavaCLAID_Wrapper_set(JNIEnv* env, jobject object, jstring variableName, jobject value)
//     {
//         // Assign wrapper if not done already:
//         WrapperMaster::getInstance()->assignWrapperToJavaClassOfObject(env, object);
        
//         std::string className = JNIUtils::getNameOfClassOfObject(env, object);
//         if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
//         {
//             CLAID_THROW(Exception, "Error, set was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
//         }

//         WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);

//         wrapper->set(env, object, variableName, value);
//     }

//     JNIEXPORT jobject JNICALL Java_JavaCLAID_Wrapper_get(JNIEnv* env, jobject object, jstring variableName)
//     {
//         // Assign wrapper if not done already:
//         WrapperMaster::getInstance()->assignWrapperToJavaClassOfObject(env, object);
        
//         std::string className = JNIUtils::getNameOfClassOfObject(env, object);
//         if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
//         {
//             CLAID_THROW(Exception, "Error, set was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
//         }

//         WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);

//         jobject objectReference = wrapper->get(env, object, variableName);
//         return objectReference;

//         // Destructor of jobject should automatically release our local reference to the jobject.
//         // i.e., env->DeleteLocalRef() should get called.
//     }

//     JNIEXPORT void JNICALL Java_JavaCLAID_Wrapper_registerWrapperForNativeClass(JNIEnv* env, jclass cls, jclass javaClass, jstring jCppClassName)
//     {
//         std::string cppClassName = JNIUtils::toStdString(env, jCppClassName);
//         if(!WrapperMaster::getInstance()->isWrapperRegisteredForNativeClass(cppClassName))
//         {
//             CLAID_THROW(Exception, "Error, tried to assign java class to wrapper for native class \"" << cppClassName.c_str() << "\", but no corresponding C++ wrapper for that class was found."
//             "Please register the class for serialization, or implement a wrapper for the class e.g. by using LazyWrapper.");
//         }
//         std::string javaClassName = JNIUtils::getClassName(env, javaClass);
//                 Logger::printfln("Registering wrapper for class %s \n", javaClassName.c_str());

//         WrapperMaster::getInstance()->assignWrapperToJavaClassByName(javaClassName, cppClassName);

    

//         // Destructor of jobject should automatically release our local reference to the jobject.
//         // i.e., env->DeleteLocalRef() should get called.
//     }

//     // // Native class = cpp class
//     // JNIEXPORT void JNICALL Java_JavaCLAID_Wrapper_registerWrapperForNativeClass(JNIEnv* env, jclass class, jstring nativeClass)
//     // {
//     //     std::string className = JNIUtils::getNameOfClassOfObject(env, object);
//     //     if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
//     //     {
//     //         CLAID_THROW(Exception, "Error, init was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
//     //     }

//     //     WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);

//     //     wrapper->assignInstance(env, object);
//     // }
// }







