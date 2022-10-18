#include "JavaWrapper/JavaModule.hpp"
#include "JavaWrapper/WrapperMaster.hpp"


namespace claid
{
    namespace JavaWrapper
    {
        JavaModule::JavaModule(JNIEnv* env, jobject javaObject)
        {
            // DO not store JNIEnv here !
            // Module will run in a separate thread later, but env
            // is only valid for the thread it was created on.
            // Thus, we need to get another env in initialize function.
            env->GetJavaVM(&this->javaVM);
            this->javaObject = env->NewGlobalRef(javaObject);
        }

        JavaModule::JavaModule(JavaVM* javaVM, jobject javaObject)
        {
            this->javaVM = javaVM;
            this->javaObject = javaObject;
        }

        void JavaModule::initialize()
        {
            Logger::printfln("JavaModule init");
            JavaVMAttachArgs args;
            args.version = JNI_VERSION_1_6; // choose your JNI version
            args.name = NULL; // you might want to give the java thread a name
            args.group = NULL; // you might want to assign the java thread to a ThreadGroup
            this->javaVM->AttachCurrentThread(&env, &args);

            jclass cls = JNIUtils::getClassOfObject(env, javaObject);
            jmethodID mid =
                    env->GetMethodID(cls, "initialize", "()V");

            if(mid == nullptr)
            {
                PORTAIBLE_THROW(Exception, "Error, function initialize with signature void () not found for class "
                    << JNIUtils::getClassName(env, cls));
            }
            else
            {
                // Return should be of type java class "Channel" (portaible.JavaWrappers.Channel)
                env->CallVoidMethod(javaObject, mid);

            }
            env->DeleteLocalRef(cls);
        }

        jobject JavaModule::publish(JNIEnv* env, jclass dataType, jstring channelID)
        {
            std::string className = JNIUtils::getClassName(env, dataType);
            if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
            {
                PORTAIBLE_THROW(Exception, "Error, publish was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
            }

            WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);
            jobject channel = wrapper->publish(env, this, channelID);

            return channel;
        }

        jobject JavaModule::subscribe(JNIEnv* env, jclass dataType, jstring channelID, jstring functionCallbackName, jstring functionSignature)
        {
            std::string className = JNIUtils::getClassName(env, dataType);
            if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
            {
                PORTAIBLE_THROW(Exception, "Error, publish was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
            }

            WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);
            jobject channel = wrapper->subscribe(env, this, channelID, functionCallbackName, functionSignature);

            return channel;
        }

        void JavaModule::callCallbackFunction(const std::string& functionName, jobject dataObject)
        {
            jclass cls = JNIUtils::getClassOfObject(env, javaObject);
            jmethodID mid =
                    env->GetMethodID(cls, functionName.c_str(), Signatures::Function::functionSignatureVoid({Signatures::Class::ChannelData}).c_str());
            
            if(mid == nullptr)
            {
                claid::Logger::printfln("NULL");
                PORTAIBLE_THROW(Exception, "Error, could not call callback function. Function " << functionName << " with signature void " << Signatures::Class::ChannelData << " not found for class "
                    << JNIUtils::getNameOfClassOfObject(env, dataObject));
            }
            else
            {
                env->CallVoidMethod(javaObject, mid, dataObject);
            }

            env->DeleteLocalRef(cls);
        }
    

        JNIEnv* JavaModule::getEnv()
        {
            return env;
        }


    }
}