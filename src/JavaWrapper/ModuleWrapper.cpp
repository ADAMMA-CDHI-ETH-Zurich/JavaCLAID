#ifdef __ANDROID__

#include "JavaWrapper/JavaModule.hpp"

#include "JavaWrapper/Signatures.hpp"
#include "JavaWrapper/JNIUtils.hpp"
#include "Logger/Logger.hpp"

using namespace portaible;
using namespace portaible::JavaWrapper;


extern "C"
{

JNIEXPORT void JNICALL
Java_com_example_portaible_Module_init(JNIEnv* env, jobject wrappedModule)
{
    JavaModule* module = new JavaModule(env, wrappedModule);
    JNIHandle::setHandle(env, wrappedModule, module);
    PORTAIBLE_RUNTIME->addModule(static_cast<Module*>(module));

    Logger::printfln("New Java Module");
    if(PORTAIBLE_RUNTIME->isStarted())
    {
      Logger::printfln("Init");
      module->startModule();
    }

}

    /*
     * Class:     com_example_portaible_Module
     * Method:    publish
     * Signature: (Ljava/lang/String;)LChannel/Channel;
     */
JNIEXPORT jobject Java_com_example_portaible_Module_publishNative
  (JNIEnv* env, jobject wrappedModule, jclass dataType, jstring channelID)
{
    JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
    return module->publish(env, dataType, channelID);
}

JNIEXPORT jobject Java_com_example_portaible_Module_subscribeNative
  (JNIEnv* env, jobject wrappedModule, jclass dataType, jstring channelID, jstring callbackFunctionName, jstring functionSignature)
{
    // TODO: CHECK IF FUNCTION SIGNATURE MAKES SENSE (i.e., only one argument for callback function).
    JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
    return module->subscribe(env, dataType, channelID, callbackFunctionName, functionSignature);
}

}
#endif

