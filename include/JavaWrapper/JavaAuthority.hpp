#pragma once
#include "RunTime/RunTime.hpp"

#include "JavaWrapper/JNIUtils.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        class JavaAuthority : public Authority
        {

            public:
                JavaAuthority(JNIEnv* env, jobject javaObject)
                {
                    // DO not store JNIEnv here !
                    // Authority will run in a separate thread later, but env
                    // is only valid for the thread it was created on.
                    // Thus, we need to get another env in initialize function.
                    env->GetJavaVM(&this->javaVM);
                    this->javaObject = env->NewGlobalRef(javaObject);
                }

                template<typename Reflector>
                void reflect(Reflector& r)
                {

                }

                void initialize()
                {
                    Logger::printfln("JavaAuthority init");
                    JavaVMAttachArgs args;
                    args.version = JNI_VERSION_1_6; // choose your JNI version
                    args.name = NULL; // you might want to give the java thread a name
                    args.group = NULL; // you might want to assign the java thread to a ThreadGroup
                    this->javaVM->AttachCurrentThread(&env, &args);

                    jmethodID mid =
                            env->GetMethodID(JNIUtils::getClassOfObject(env, javaObject), "initialize", "()V");
                    Logger::printfln("JavaAuthority 2");

                    if(mid == nullptr)
                    {
                        portaible::Logger::printfln("NULL");
                        PORTAIBLE_THROW(Exception, "Error, function initialize with signature void () not found for class "
                            << JNIUtils::getClassName(env, JNIUtils::getClassOfObject(env, javaObject)));
                    }
                    else
                    {
                                        Logger::printfln("JavaAuthority 3");

                        // Return should be of type java class "Channel" (portaible.JavaWrappers.Channel)
                        env->CallVoidMethod(javaObject, mid);

                    }
                }

            private:
                JNIEnv* env;
                jobject javaObject;
                JavaVM* javaVM;

        };
    }
}