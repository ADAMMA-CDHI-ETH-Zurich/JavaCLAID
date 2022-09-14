#pragma once

#include <jni.h>

#include "Signatures.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        namespace JNIHandle
        {
            static jfieldID getHandleField(JNIEnv* env, jobject obj)
            {
                jclass c = env->GetObjectClass(obj);
                // J is the type signature for long:
                jfieldID fieldID = env->GetFieldID(c, "nativeHandle", "J");
                env->DeleteLocalRef(c);

                return fieldID;
            }

            template <typename T>
            static T* getHandle(JNIEnv* env, jobject obj)
            {
                jlong handle = env->GetLongField(obj, getHandleField(env, obj));
                return reinterpret_cast<T *>(handle);
            }

            template <typename T>
            static void setHandle(JNIEnv* env, jobject obj, T* t)
            {
                jlong handle = reinterpret_cast<jlong>(t);
                env->SetLongField(obj, getHandleField(env, obj), handle);
            }
        }
    }
}