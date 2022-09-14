#pragma once

#include <string>
#include <jni.h>
#include<stack>
#include <map>

#include "JNIHandle.hpp"
#include "Exception/Exception.hpp"
#include "TypeChecking/TypeCheckingFunctions.hpp"



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

                template<typename T>
                static typename std::enable_if<is_integer_no_bool<T>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "intValue";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, float>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "floatValue";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, double>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "doubleValue";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, char>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "charValue";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, bool>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "booleanValue";
                }

                // ================================================================================

                template<typename T>
                static typename std::enable_if<is_integer_no_bool<T>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallIntMethod(object, methodID);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, float>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallFloatMethod(object, methodID);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, double>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallDoubleMethod(object, methodID);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, char>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallCharMethod(object, methodID);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, bool>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallBooleanMethod(object, methodID);
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

                static void triggerGarbageCollector(JNIEnv* env)
                {
                    jclass    systemClass    = nullptr;
                    jmethodID systemGCMethod = nullptr;
                    // Take out the trash.
                    systemClass    = env->FindClass("java/lang/System");
                    systemGCMethod = env->GetStaticMethodID(systemClass, "gc", "()V");
                    env->CallStaticVoidMethod(systemClass, systemGCMethod);
                }

                static jclass findClass(JNIEnv* env, const char* name) 
                {
                    if(!onLoadCalled)
                    {
                        PORTAIBLE_THROW(Exception, "Error, JNIUtils::findClass was called without prior call to onLoad. Please make sure that JNIUtils::onLoad "
                        "was called in JNI_OnLoad in native code.");
                    }

                    std::string className;


                    jstring str = env->NewStringUTF(name);
                    jclass cls = static_cast<jclass>(env->CallObjectMethod(gClassLoader, gFindClassMethod, str));
                
                    env->DeleteLocalRef(str);
                    
                    // Save class for later use, so we do not always need to query it again.
                    // Also, might save space in the local reference table.
                    Logger::printfln("Returning normal %s", name);
                    return cls;
                }

                static std::string toStdString(JNIEnv *env, jobject jStr)
                {
                    return toStdString(env, static_cast<jstring>(jStr));
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
                    // Find base class Class to call it's getName function.

                    std::string className = "java/lang/Class";
                    jclass ccls = env->FindClass("java/lang/Class");
                
                    jmethodID mid_getName = env->GetMethodID(ccls, "getName", "()Ljava/lang/String;");
                    jstring strObj = (jstring)env->CallObjectMethod(dataType, mid_getName);

                    std::string stdString = JNIUtils::toStdString(env, strObj);

                    env->DeleteLocalRef(ccls);
                    env->DeleteLocalRef(strObj);

                    return stdString;
                }

                static std::string getNameOfClassOfObject(JNIEnv* env, jobject object)
                {
                    jclass cls = getClassOfObject(env, object);
                    std::string className = getClassName(env, cls);
                    env->DeleteLocalRef(cls);

                    return className;
                }

                static jclass getClassOfObject(JNIEnv* env, jobject object)
                {
                    return env->GetObjectClass(object);
                }

                

                // ============================================================

                static bool isJavaIntegerObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Integer";
                }

                static bool isJavaFloatObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Float";
                }

                static bool isJavaDoubleObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Double";
                }

                static bool isJavaBooleanObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Boolean";
                }

                static bool isJavaStringObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.String";
                }

                static bool isJavaCharObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Character";
                }

                // ================================================================================================

                // Set primitive
                template<typename T>
                static void javaPrimitiveObjectToNativePrimitive(JNIEnv* env, T& nativePrimitive, jobject& javaObject)
                {
                    std::string expectedJavaPrimitiveClassName = Signatures::Class::signatureToClassName(Signatures::Primitive::getJavaClassOfPrimitiveType<T>());

                    std::string className = getNameOfClassOfObject(env, javaObject);
                    
                    // Does java object match the primitive?
                    if(className != expectedJavaPrimitiveClassName)
                    {
                        PORTAIBLE_THROW(Exception, "Error, cannot convert javaObject to native C++ primitive " << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<T>() << "."
                        << "Java object is of type \"" << className << "\", however an object of type \"" << expectedJavaPrimitiveClassName << "\" was expected.");
                    }

                    // Get the function of the java object that can be used to retrieve the primitive (e.g., integerValue, floatValue, ... as in
                    // Integer(42).integerValue() etc.)
                    std::string signature = "()" + Signatures::Primitive::getSignatureOfPrimitiveType<T>();
                    std::string getterMethodName = getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject<T>().c_str();

                    jclass objectClass = getClassOfObject(env, javaObject);
                    jmethodID mGetValue = env->GetMethodID(objectClass, getterMethodName.c_str(), signature.c_str());
                    env->DeleteLocalRef(objectClass);
                    if(mGetValue == NULL)
                    {
                        PORTAIBLE_THROW(Exception, "Error, cannot convert java object of type \"" << className << "\" to native C++ primitive \"" << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<T>() << "\"."
                        << "Method \"" << getterMethodName << "\" was not found in object of type \"" << className << "\"");
                    } 

                    nativePrimitive = callPrimitiveMethod<T>(env, javaObject, mGetValue);
                }

               

                


                static bool objectCallIntFunction(JNIEnv* env, jobject object, const std::string& functionName, const std::string& signature, int& returnValue)
                {
                    jclass objectClass = getClassOfObject(env, object);
                    jmethodID methodID = env->GetMethodID(objectClass, functionName.c_str(), signature.c_str());
                    env->DeleteLocalRef(objectClass);
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
                    jmethodID methodID = env->GetMethodID(objectClass, "elementAt", "(I)Ljava/lang/Object;");
                    env->DeleteLocalRef(objectClass);
                    
                    if(methodID == NULL)
                    {
                        return false;
                    } 
                    returnValue = env->CallObjectMethod(container, methodID, index);


                    return true;
                }

                static jobject createJavaVector(JNIEnv* env, int size)
                {
                    std::string className = Signatures::Class::Vector;
                    jclass cls = JNIUtils::findClass(env, className.c_str());

                    if(cls == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java Vector object, failed to lookup class " << className);
                    }

                    jmethodID constructor = env->GetMethodID(cls, "<init>", "(I)V");

                    if(constructor == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java Vector object, failed to lookup constructor for class " << className);
                    }

                    jobject javaObject = env->NewObject(cls, constructor, size);

                    if(javaObject == nullptr)
                    {
                        PORTAIBLE_THROW(Exception, "Cannot create java Vector object, object with class signature " << className << " could not be created.");
                    }
                    env->DeleteLocalRef(cls);

                    return javaObject;
                }
        };
    }
}