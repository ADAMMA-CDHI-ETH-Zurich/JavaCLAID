#pragma once

#include <string>
#include <jni.h>
#include<stack>
#include "JNIHandle.hpp"
#include "Exception/Exception.hpp"

namespace portaible
{
    namespace JavaWrapper
    {
        class JNIUtils
        {
            private:
                static JavaVM* jvm;
                static jobject gClassLoader;
                static jmethodID gFindClassMethod;
                static bool onLoadCalled;

                static JNIEnv* getEnv() 
                {
                    JNIEnv *env;
                    int status = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
                    if(status < 0) {    
                        status = jvm->AttachCurrentThread(&env, NULL);
                        if(status < 0) {        
                            return nullptr;
                        }
                    }
                    return env;
                }

            public:
                // See https://stackoverflow.com/questions/13263340/findclass-from-any-thread-in-android-jni/16302771#16302771
                static void onLoad(JavaVM* javaVM)
                {
                    Logger::printfln("JNIUtils onLoad");
                    JNIUtils::jvm = javaVM;
                    JNIEnv* env = getEnv();
                    Logger::printfln("JNIUtils onLoad 2");

                    // Just use any class of portaible.
                    auto randomClass = env->FindClass(Signatures::Class::Channel.c_str());
                    jclass classClass = env->GetObjectClass(randomClass);
                                        Logger::printfln("JNIUtils onLoad 3");

                    auto classLoaderClass = env->FindClass("java/lang/ClassLoader");
                    auto getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader",
                                                            "()Ljava/lang/ClassLoader;");
                                                            Logger::printfln("JNIUtils onLoad 4");

                    jobject tmpClassLoader = env->CallObjectMethod(randomClass, getClassLoaderMethod);
                    gClassLoader = env->NewGlobalRef(tmpClassLoader);
                    gFindClassMethod = env->GetMethodID(classLoaderClass, "findClass",
                                                    "(Ljava/lang/String;)Ljava/lang/Class;");
                    onLoadCalled = true;
                                        Logger::printfln("JNIUtils onLoad 5");

                }


                static jclass findClass(JNIEnv* env, const char* name) 
                {
                    if(!onLoadCalled)
                    {
                        PORTAIBLE_THROW(Exception, "Error, JNIUtils::findClass was called without prior call to onLoad. Please make sure that JNIUtils::onLoad "
                        "was called in JNI_OnLoad in native code.");
                    }
                    return static_cast<jclass>(env->CallObjectMethod(gClassLoader, gFindClassMethod, env->NewStringUTF(name)));
                }


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

                static jfloat floatObjectToPrimitiveFloat(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    Logger::printfln("Class of object %s", getClassName(env, objectClass).c_str());
                    jmethodID mGetValue = env->GetMethodID(objectClass, "floatValue", "()F");
                    if(mGetValue == NULL)
                    {
                        PORTAIBLE_THROW(Exception, "Error, cannot convert object to primitive float. JNIUtils::floatObjectToPrimitiveFloat was called with an object of class " 
                        << getClassName(env, objectClass) << ", but class java/lang/Float was expected.");
                    } 
                    jfloat value = env->CallFloatMethod(data, mGetValue);

                    return value;
                }

                static jfloat doubleObjectToPrimitiveDouble(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    jmethodID mGetValue = env->GetMethodID(objectClass, "doubleValue","()D");
                    if(mGetValue == NULL)
                    {
                        PORTAIBLE_THROW(Exception, "Error, cannot convert object to primitive double. JNIUtils::doubleObjectToPrimitiveDouble was called with an object of class " 
                        << getClassName(env, objectClass) << ", but class java/lang/Double was expected.");
                    } 
                    jfloat value = env->CallDoubleMethod(data, mGetValue);

                    return value;
                }

                static jfloat integerObjectToPrimitiveInteger(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    jmethodID mGetValue = env->GetMethodID(objectClass, "intValue","()I");
                    if(mGetValue == NULL)
                    {
                        PORTAIBLE_THROW(Exception, "Error, cannot convert object to primitive integer. JNIUtils::integerObjectToPrimitiveInteger was called with an object of class " 
                        << getClassName(env, objectClass) << ", but class java/lang/Integer was expected.");
                    } 
                    jfloat value = env->CallDoubleMethod(data, mGetValue);

                    return value;
                }

                static jfloat booleanObjectToPrimitiveBoolean(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    jmethodID mGetValue = env->GetMethodID(objectClass, "booleanValue","()B");
                    if(mGetValue == NULL)
                    {
                        PORTAIBLE_THROW(Exception, "Error, cannot convert object to primitive boolean. JNIUtils::booleanObjectToPrimitiveBoolean was called with an object of class " 
                        << getClassName(env, objectClass) << ", but class java/lang/Boolean was expected.");
                    } 
                    jfloat value = env->CallBooleanMethod(data, mGetValue);

                    return value;
                }

                static jfloat charObjectToPrimitiveChar(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    jmethodID mGetValue = env->GetMethodID(objectClass, "charValue","()C");
                    if(mGetValue == NULL)
                    {
                        PORTAIBLE_THROW(Exception, "Error, cannot convert object to primitive boolean. JNIUtils::charObjectToPrimitiveChar was called with an object of class " 
                        << getClassName(env, objectClass) << ", but class java/lang/Character was expected.");
                    } 
                    jfloat value = env->CallCharMethod(data, mGetValue);

                    return value;
                }

                static bool isJavaIntegerObject(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    std::string className = getClassName(env, objectClass);
                    return className == "java.lang.Integer";
                }

                static bool isJavaFloatObject(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    std::string className = getClassName(env, objectClass);
                    return className == "java.lang.Float";
                }

                static bool isJavaDoubleObject(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    std::string className = getClassName(env, objectClass);
                    return className == "java.lang.Double";
                }

                static bool isJavaBooleanObject(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    std::string className = getClassName(env, objectClass);
                    return className == "java.lang.Boolean";
                }

                static bool isJavaStringObject(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    std::string className = getClassName(env, objectClass);
                    return className == "java.lang.String";
                }

                static bool isJavaCharObject(JNIEnv* env, jobject data)
                {
                    jclass objectClass = getClassOfObject(env, data);
                    std::string className = getClassName(env, objectClass);
                    return className == "java.lang.Character";
                }

                static bool objectCallIntFunction(JNIEnv* env, jobject object, const std::string& functionName, const std::string& signature, int& returnValue)
                {
                    jclass objectClass = getClassOfObject(env, object);
                    jmethodID methodID = env->GetMethodID(objectClass, functionName.c_str(), signature.c_str());
                    if(methodID == NULL)
                    {
                        return false;
                    } 
                    returnValue = env->CallIntMethod(object, methodID);

                    return true;
                }

                static bool getElementAtIndex(JNIEnv* env, jobject container, const size_t index, jobject& returnValue)
                {    
                    jclass objectClass = getClassOfObject(env, container);
                    jmethodID methodID = env->GetMethodID(objectClass, "elementAt", Signatures::Function::functionSignature({"I"}, Signatures::Class::JavaObject).c_str());
                    if(methodID == NULL)
                    {
                        return false;
                    } 
                    returnValue = env->CallObjectMethod(container, methodID, index);

                    return true;
                }
        };
    }
}