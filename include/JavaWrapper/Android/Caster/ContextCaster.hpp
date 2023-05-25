#pragma once
#include "JavaWrapper/Android/Context.hpp"

#if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)

#include "jbind11/Caster/Caster.hpp"
using namespace jbind11;

namespace jbind11
{
    template<typename T>
    struct Caster<T, typename std::enable_if<std::is_same<T, claid::JavaWrapper::Context>::value>::type>
    {
        // T is jobject.
        public:
            static claid::JavaWrapper::Context fromJavaObject(JNIEnv* env, jobject javaObject)
            {
                return claid::JavaWrapper::Context(javaObject);
            }

            static jobject cast(JNIEnv* env, claid::JavaWrapper::Context& value)
            {
                return value.getJavaObject();
            }

            static std::string canonicalTypeName()
            {
                return "android.content.Context";
            }

            static bool isGeneric()
            {
                return false;
            }
    };
}

#endif