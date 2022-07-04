
#ifdef __ANDROID__

#include "Channel/Channel.hpp"

#include "Logger/Logger.hpp"
#include "JavaWrapper/JNIUtils.hpp"

#include <jni.h>

using namespace portaible;
using namespace portaible::JavaWrapper;
extern "C"
{
    JNIEXPORT void Java_com_example_portaible_Channel_post
      (JNIEnv* env, jobject wrappedChannel, jobject data)
    {
        // From the java side, it is guaranteed that the data type of data
        // matches the data type that was specified when the channel was created using publish.
        // As Channel is a templated/generic class Channel<T> on java side,
        // it was created like Channel<AudioData> audioChannel = module.publish(AudioData.class, channelID).
        // Therefore, audioChannel.post() has to be called with an object of type AudioData, which
        // is ensured at compile time.
        Logger::printfln("Post test");
        Logger::printfln("%s", JNIUtils::getClassName(env, JNIUtils::getClassOfObject(env, wrappedChannel)).c_str());
        Logger::printfln("%s", JNIUtils::getClassName(env, JNIUtils::getClassOfObject(env, data)).c_str());

        // "(Ljava/lang/String;)Lcom/example/portaible/Channel;";//
        std::string signature = Signatures::Function::functionSignatureVoid({Signatures::Class::Channel});


        jclass dataType = JNIUtils::getClassOfObject(env, data);
        jmethodID mid =
                env->GetMethodID(dataType, "post", signature.c_str());

        if(mid == nullptr)
        {
            portaible::Logger::printfln("NULL");
            PORTAIBLE_THROW(Exception, "Error, function publish with signature " << signature << " not found for class " << JNIUtils::getClassName(env, dataType));
        }
        else
        {
            Logger::printfln("Invoking post method");
            env->CallVoidMethod(data, mid, wrappedChannel);
        }
    }
}

#endif