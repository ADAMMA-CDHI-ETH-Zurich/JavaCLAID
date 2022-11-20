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
            this->moduleName = JNIUtils::getNameOfClassOfObject(env, javaObject);
        }

        JavaModule::JavaModule(JavaVM* javaVM, jobject javaObject)
        {
            this->javaVM = javaVM;
            JNIEnv* env = JNIUtils::getEnv();
            this->javaObject = env->NewGlobalRef(javaObject);
            this->moduleName = JNIUtils::getNameOfClassOfObject(env, javaObject);
        }

        void JavaModule::initialize()
        {
            Logger::printfln("JavaModule init");
            JavaVMAttachArgs args;
            args.version = JNI_VERSION_1_6; // choose your JNI version
            args.name = NULL; // you might want to give the java thread a name
            args.group = NULL; // you might want to assign the java thread to a ThreadGroup
            Logger::printfln("JavaModule init 2");

            #ifdef __ANDROID__
            this->javaVM->AttachCurrentThread(&env, &args);
            #else
            this->javaVM->AttachCurrentThread((void**)&env, &args);
            #endif
                                    Logger::printfln("JavaModule init 2.5");

            jclass cls = JNIUtils::getClassOfObject(env, javaObject);
                        Logger::printfln("JavaModule init 3");

            jmethodID mid =
                    env->GetMethodID(cls, "initialize", "()V");
            Logger::printfln("JavaModule init 4");

            if(mid == nullptr)
            {
                CLAID_THROW(Exception, "Error, function initialize with signature void () not found for class "
                    << JNIUtils::getClassName(env, cls));
            }
            else
            {
                // Return should be of type java class "Channel" (portaible.JavaWrappers.Channel)
                env->CallVoidMethod(javaObject, mid);

            }
                        Logger::printfln("JavaModule init 5");

            env->DeleteLocalRef(cls);
        }

        jobject JavaModule::publish(JNIEnv* env, jclass dataType, jstring channelID)
        {
            // Assign wrapper if not done already:
            WrapperMaster::getInstance()->assignWrapperToJavaClass(env, dataType);

            std::string className = JNIUtils::getClassName(env, dataType);
            if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
            {
                CLAID_THROW(Exception, "Error, publish was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
            }

            WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapperForJavaClass(className);
            jobject channel = wrapper->publish(env, this, channelID);

            return channel;
        }

        jobject JavaModule::subscribe(JNIEnv* env, jclass dataType, jstring channelID, jstring functionCallbackName, jstring functionSignature)
        {
            // Assign wrapper if not done already:
            WrapperMaster::getInstance()->assignWrapperToJavaClass(env, dataType);

            std::string className = JNIUtils::getClassName(env, dataType);
            if(!WrapperMaster::getInstance()->isWrapperAssignedToJavaClass(className))
            {
                CLAID_THROW(Exception, "Error, publish was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
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
                CLAID_THROW(Exception, "Error, could not call callback function. Function " << functionName << " with signature void " << Signatures::Class::ChannelData << " not found for class "
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

        const std::string JavaModule::getModuleName()
        {
            return this->moduleName;
        }


    }
}