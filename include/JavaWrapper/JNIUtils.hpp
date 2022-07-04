#pragma once

#include <string>
#include <jni.h>
#include "JNIHandle.hpp"


namespace portaible
{
    namespace JavaWrapper
    {
        namespace JNIUtils
        {
            static std::string toStdString(JNIEnv *env, jstring jStr)
            {
                if (!jStr)
                        return "";

                const jclass stringClass = env->GetObjectClass(jStr);
                const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
                const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

                size_t length = (size_t) env->GetArrayLength(stringJbytes);
                jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

                std::string ret = std::string((char*) pBytes, length);
                env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

                env->DeleteLocalRef(stringJbytes);
                env->DeleteLocalRef(stringClass);
                return ret;
            }

            static jstring toJString(JNIEnv* env, const std::string& nativeString)
            {
                return env->NewStringUTF(nativeString.c_str());
            }

            // Returns the class name of a given object of type jclass.
            // Not, the object needs to be a Class object in Java!
            // E.g. String.class would work. Its NOT an instance of String in that case.
            // If it was a native function (called from java), you'd call it like getClassName(String.class).
            // If you want to get the class name of an object, make sure to get it's class first (i.e.: env->GetObjectClass(env, object)),
            // do not pass the object to this function
            static std::string getClassName(JNIEnv* env, jclass dataType)
            {
                // Find base class Class
                jclass ccls = env->FindClass("java/lang/Class");
                jmethodID mid_getName = env->GetMethodID(ccls, "getName", "()Ljava/lang/String;");
                jstring strObj = (jstring)env->CallObjectMethod(dataType, mid_getName);

                return JNIUtils::toStdString(env, strObj);
            }

            static jclass getClassOfObject(JNIEnv* env, jobject object)
            {
                return env->GetObjectClass(object);
            }

        }
    }
}