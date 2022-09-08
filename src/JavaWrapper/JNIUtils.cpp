#include "JavaWrapper/JNIUtils.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        JavaVM* JNIUtils::jvm = nullptr;
        jobject JNIUtils::gClassLoader;
        jmethodID JNIUtils::gFindClassMethod;
        bool JNIUtils::onLoadCalled = false;
    }
}