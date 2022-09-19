#ifdef __ANDROID__


#include "JavaWrapper/JNIUtils.hpp"
#include "Logger/Logger.hpp"
#include "JavaWrapper/WrapperMaster.hpp"
using namespace portaible;
using namespace portaible::JavaWrapper;
extern "C"
{
    JNIEXPORT void JNICALL Java_com_example_portaible_Wrapper_init(JNIEnv* env, jobject object)
    {
        std::string className = JNIUtils::getNameOfClassOfObject(env, object);
        if(!WrapperMaster::getInstance()->isWrapperRegisteredForClass(className))
        {
            PORTAIBLE_THROW(Exception, "Error, init was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
        }

        WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapper(className);

        wrapper->assignInstance(env, object);
    }

    JNIEXPORT void JNICALL Java_com_example_portaible_Wrapper_set(JNIEnv* env, jobject object, jstring variableName, jobject value)
    {
        std::string className = JNIUtils::getNameOfClassOfObject(env, object);
        if(!WrapperMaster::getInstance()->isWrapperRegisteredForClass(className))
        {
            PORTAIBLE_THROW(Exception, "Error, set was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
        }

        WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapper(className);

        wrapper->set(env, object, variableName, value);
    }

    JNIEXPORT void JNICALL Java_com_example_portaible_Wrapper_get(JNIEnv* env, jobject object, jstring variableName, jobject targetObject)
    {
        std::string className = JNIUtils::getNameOfClassOfObject(env, object);
        if(!WrapperMaster::getInstance()->isWrapperRegisteredForClass(className))
        {
            PORTAIBLE_THROW(Exception, "Error, set was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
        }

        WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapper(className);

        wrapper->get(env, object, variableName, targetObject);
    }

    // // Native class = cpp class
    // JNIEXPORT void JNICALL Java_com_example_portaible_Wrapper_registerWrapperForNativeClass(JNIEnv* env, jclass class, jstring nativeClass)
    // {
    //     std::string className = JNIUtils::getNameOfClassOfObject(env, object);
    //     if(!WrapperMaster::getInstance()->isWrapperRegisteredForClass(className))
    //     {
    //         PORTAIBLE_THROW(Exception, "Error, init was called for java class " << className.c_str() << ", but no corresponding C++ wrapper was found.");
    //     }

    //     WrapperBase* wrapper = WrapperMaster::getInstance()->getWrapper(className);

    //     wrapper->assignInstance(env, object);
    // }
}

#endif





