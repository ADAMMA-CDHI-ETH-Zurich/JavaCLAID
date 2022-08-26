#include "JavaWrapper/WrapperMaster.hpp"
#include "Audio/AudioData.hpp"

LAZY_WRAPPER("com.example.portaible.AudioData", AudioData)


// Alternative:
/*

using namespace portaible;
using namespace portaible::JavaWrapper;

namespace portaible
{
    class AudioDataWrapper : public Wrapper<AudioData>
    {
        DECLARE_WRAPPER(AudioData)
    };
}

REGISTER_WRAPPER("com.example.portaible.AudioData", portaible::AudioData, AudioDataWrapper)

*/

extern "C"
{
    JNIEXPORT void Java_com_example_portaible_AudioData_set
      (JNIEnv* env, jobject thiz, jbyteArray data)
    {
        AudioData* audioData = JNIHandle::getHandle<AudioData>(env, thiz);
        jbyte* bufferPtr = env->GetByteArrayElements(data, NULL);
        jsize lengthOfArray = env->GetArrayLength(data);
        
        audioData->data.resize(lengthOfArray);
        
        Logger::printfln("Copying %d bytes of data to audio data", lengthOfArray);
        for(size_t i = 0; i < lengthOfArray; i++)
        {
            audioData->data[i] = bufferPtr[i];
        }

        env->ReleaseByteArrayElements(data, bufferPtr, 0);

    }

    // JNIEXPORT jbyteArray Java_com_example_portaible_AudioData_get
    //   (JNIEnv* env, jobject thiz, jbyteArray data)
    // {
    //     AudioData* audioData = JNIHandle::getHandle<AudioData>(env, thiz);
    //     jbyte* bufferPtr = env->GetByteArrayElements(data, NULL);
    //     jsize lengthOfArray = env->GetArrayLength(data);
        
    //     env->ReleaseByteArrayElements(data, bufferPtr, 0);

    // }
}