#include "JavaWrapper/JavaModule.hpp"

#include "JavaWrapper/Signatures.hpp"
#include "JavaWrapper/JNIUtils.hpp"
#include "Logger/Logger.hpp"

using namespace claid;
using namespace claid::JavaWrapper;


extern "C"
{

JNIEXPORT void JNICALL Java_JavaCLAID_Module_init(JNIEnv* env, jobject wrappedModule)
{


}

    /*
     * Class:     com_example_portaible_Module
     * Method:    publish
     * Signature: (Ljava/lang/String;)LChannel/Channel;
     */
JNIEXPORT jobject Java_JavaCLAID_Module_publishNative
  (JNIEnv* env, jobject wrappedModule, jclass dataType, jstring channelID)
{
    JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
    return module->publish(env, dataType, channelID);
}

JNIEXPORT jobject Java_JavaCLAID_Module_subscribeNative
  (JNIEnv* env, jobject wrappedModule, jclass dataType, jstring channelID, jstring callbackFunctionName, jstring functionSignature)
{
    JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
    return module->subscribe(env, dataType, channelID, callbackFunctionName, functionSignature);
}

JNIEXPORT void Java_JavaCLAID_Module_registerPeriodicFunction
  (JNIEnv* env, jobject wrappedModule, jstring identifier, jstring functionName, jint periodInMilliseconds)
{
    JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
    module->registerPeriodicFunction(env, identifier, functionName, periodInMilliseconds);
}

JNIEXPORT void Java_JavaCLAID_Module_unregisterPeriodicFunction
  (JNIEnv* env, jobject wrappedModule, jstring identifier)
{
    JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
    module->unregisterPeriodicFunction(env, identifier);
}
}
