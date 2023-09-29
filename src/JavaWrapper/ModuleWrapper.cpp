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

#include "JavaWrapper/JavaModule.hpp"

#include "JavaWrapper/Signatures.hpp"
#include "Logger/Logger.hpp"

using namespace claid;
using namespace claid::JavaWrapper;


extern "C"
{

JNIEXPORT void JNICALL Java_JavaCLAID_Module_init(JNIEnv* env, jobject wrappedModule)
{


}

//     /*
//      * Class:     com_example_portaible_Module
//      * Method:    publish
//      * Signature: (Ljava/lang/String;)LChannel/Channel;
//      */
// JNIEXPORT jobject Java_JavaCLAID_Module_publishNative
//   (JNIEnv* env, jobject wrappedModule, jclass dataType, jstring channelID)
// {
//     JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
//     return module->publish(env, dataType, channelID);
// }

// JNIEXPORT jobject Java_JavaCLAID_Module_subscribeNative
//   (JNIEnv* env, jobject wrappedModule, jclass dataType, jstring channelID, jstring callbackFunctionName, jstring functionSignature)
// {
//     JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
//     return module->subscribe(env, dataType, channelID, callbackFunctionName, functionSignature);
// }

// JNIEXPORT void Java_JavaCLAID_Module_registerPeriodicFunction
//   (JNIEnv* env, jobject wrappedModule, jstring identifier, jstring functionName, jint periodInMilliseconds)
// {
//     JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
//     module->registerPeriodicFunction(env, identifier, functionName, periodInMilliseconds);
// }

// JNIEXPORT void Java_JavaCLAID_Module_unregisterPeriodicFunction
//   (JNIEnv* env, jobject wrappedModule, jstring identifier)
// {
//     JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
//     module->unregisterPeriodicFunction(env, identifier);
// }
}
