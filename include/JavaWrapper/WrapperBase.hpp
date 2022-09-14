#pragma once

#include "JavaWrapper/JNIUtils.hpp"
#include "JavaModule.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        class WrapperBase
        {
        public:
            virtual jobject publish(JNIEnv* env, JavaModule* module, jstring jChannelID) = 0;
            virtual jobject subscribe(JNIEnv* env, JavaModule* module, jstring jChannelID, jstring jFunctionCallbackName, jstring jFunctionSignature) = 0;
            virtual void post(JNIEnv* env, jobject channel, jobject data) = 0;
            virtual void assignInstance(JNIEnv* env, jobject javaObject) = 0;

            virtual void set(JNIEnv* env, jobject object, jstring jVariableName, jobject value) = 0;
        };
    }
}