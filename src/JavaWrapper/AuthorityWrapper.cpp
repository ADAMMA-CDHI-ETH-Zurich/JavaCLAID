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

}

    /*
     * Class:     com_example_portaible_Module
     * Method:    publish
     * Signature: (Ljava/lang/String;)LChannel/Channel;
     */
JNIEXPORT jobject Java_com_example_portaible_Module_publish
  (JNIEnv* env, jobject wrappedModule, jclass dataType, jstring channelID)
{
    JavaModule* module = JNIHandle::getHandle<JavaModule>(env, wrappedModule);
    return module->publish(env, dataType, channelID);
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