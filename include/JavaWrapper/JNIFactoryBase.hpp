#pragma once
#include "JNIUtils.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        // Allows to create a Java Object (jobject) from the provided C++ type, if possible.
        // E.g., int -> java.lang.Integer
        // std::string -> java.lang.String
        // std::vector -> java.lang.ArrayList

        class JNIFactoryBase
        {
            public:
            template<typename... Parameters>
            static void newJavaObjectFromClassSignature(JNIEnv* env, std::string className, jobject& object, std::string constructerParameters, Parameters... parameters)
            {
                // Why use custom findClass?
                // See https://stackoverflow.com/questions/13263340/findclass-from-any-thread-in-android-jni/16302771#16302771

                jclass cls = JNIUtils::findClass(env, className.c_str());

                if(cls == nullptr)
                {
                    PORTAIBLE_THROW(Exception, "Cannot create java object, failed to lookup class " << className);
                }

                std::string constructorSignature = std::string("(") + constructerParameters + std::string(")V");
                jmethodID constructor = env->GetMethodID(cls, "<init>", constructorSignature.c_str());

                if(constructor == nullptr)
                {
                    PORTAIBLE_THROW(Exception, "Cannot create java object, failed to lookup constructor for class " << className);
                }

                object = env->NewObject(cls, constructor, parameters...);

                if(object == nullptr)
                {
                    PORTAIBLE_THROW(Exception, "Cannot create java object, object with class signature " << className << " could not be created.");
                }

                env->DeleteLocalRef(cls);
            }
        };

    }
}