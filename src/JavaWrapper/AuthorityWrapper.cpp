#ifdef __ANDROID__

#include "JavaWrapper/JavaAuthority.hpp"

#include "JavaWrapper/Signatures.hpp"
#include "JavaWrapper/JNIUtils.hpp"
#include "Logger/Logger.hpp"

using namespace portaible;
using namespace portaible::JavaWrapper;


extern "C"
{

JNIEXPORT void JNICALL
Java_com_example_portaible_Authority_init(JNIEnv* env, jobject wrappedAuthority)
{
    JavaAuthority* authority = new JavaAuthority(env, wrappedAuthority);
    JNIHandle::setHandle(env, wrappedAuthority, authority);
    PORTAIBLE_RUNTIME->addAuthority(static_cast<Authority*>(authority));

}

    /*
     * Class:     com_example_portaible_Authority
     * Method:    publish
     * Signature: (Ljava/lang/String;)LChannel/Channel;
     */
JNIEXPORT jobject Java_com_example_portaible_Authority_publish
  (JNIEnv* env, jobject wrappedAuthority, jclass dataType, jstring channelID)
{
    // "(Ljava/lang/String;)Lcom/example/portaible/Channel;";//
    std::string signature = Signatures::Function::functionSignature({Signatures::Class::JavaClass, Signatures::Class::String}, Signatures::Class::Channel);

    Logger::printfln("Data type %s", JNIUtils::getClassName(env, dataType).c_str());
    jmethodID mid =
            env->GetStaticMethodID(dataType, "publish", signature.c_str());

    if(mid == nullptr)
    {
        portaible::Logger::printfln("NULL");
        PORTAIBLE_THROW(Exception, "Error, function publish with signature " << signature << " not found for class " << JNIUtils::getClassName(env, dataType));
    }
    else
    {
        // Return should be of type java class "Channel" (portaible.JavaWrappers.Channel)
        return env->CallStaticObjectMethod(dataType, mid, dataType, channelID);
    }
}

}

#endif

/*
class TestCollector : public Collector
{
    public:

        void initialize()
        {
            this->testChannel = publish<std::string>("TestChannel");
            this->dataChannel = publish<AudioData>("MicrophoneData");

            this->requestChannel = subscribe<Request>("Requests");

        }

        void onRequest(Request& request)
        {

        }

    private:
        Channel<std::string> testChannel;
        Channel<AudioData> dataChannel;
        
        Channel<Request> requestChannel;


}*/