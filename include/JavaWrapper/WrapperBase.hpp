#pragma once

#include "JavaWrapper/JNIUtils.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        class WrapperBase
        {
        public:
            virtual jobject publish(JNIEnv* env, jstring name) = 0;
            virtual void post(JNIEnv* env, jobject channel, jobject data) = 0;
            virtual void assignInstance(JNIEnv* env, jobject javaObject) = 0;
        };
    }
}