#pragma once

#include <string>
#include <jni.h>
#include<stack>
#include <map>

#include "JNIHandle.hpp"
#include "Exception/Exception.hpp"
#include "TypeChecking/TypeCheckingFunctions.hpp"


namespace claid
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

                // Why not map char to java/lang/Character? Because Character class in Java is 16 bits, while in C++ its only always 8 bits.
                // Hence, Java's byte type corresponds to C++ chars type. 
                // And what about the java/lang/Character type? There is no corresponding C++ type.
                // If there is a java/lang/Character object coming from Java, it needs to be converted to uint16_t (unsigned 16 bit signed integer).
                // See https://docs.oracle.com/javase/tutorial/java/nutsandbolts/datatypes.html
                template<typename T>
                static typename std::enable_if<std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "byteValue";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "shortValue";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "intValue";
                }

                // long can be either 32 or 64 bit: https://en.cppreference.com/w/cpp/language/types
                template<typename T>
                static typename std::enable_if<std::is_same<T, long>::value || std::is_same<T, unsigned long>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "longValue";
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value, std::string>::type
                getGetterFunctionNameUsedToRetrievePrimitiveFromJavaObject()
                {
                    return "longValue";
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

                // Note that Java's Character class is 16 bit. 
                // Further note: char16_t can neither be signed nor unsigned.
                template<typename T>
                static typename std::enable_if<std::is_same<T, char16_t>::value, std::string>::type
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

                // Why not map char to java/lang/Character? Because Character class in Java is 16 bits, while in C++ its only always 8 bits.
                // Hence, Java's byte type corresponds to C++ chars type. 
                // And what about the java/lang/Character type? There is no corresponding C++ type.
                // If there is a java/lang/Character object coming from Java, it needs to be converted to uint16_t (unsigned 16 bit signed integer).
                // See https://docs.oracle.com/javase/tutorial/java/nutsandbolts/datatypes.html
                template<typename T>
                static typename std::enable_if<std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    // Force conversion to byte.
                    // byte is defined as "enum class : signed char".
                    // jbyte is defined as typedef signed char jbyte aswell.
                    // Thus, we can force the cast by reinterpreting the memory address, even though the compiler would complain.
                    jbyte jByte = env->CallByteMethod(object, methodID);
                
                    return jByte;
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallShortMethod(object, methodID);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallIntMethod(object, methodID);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, long>::value || std::is_same<T, unsigned long>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallLongMethod(object, methodID);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallLongMethod(object, methodID);
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

                // // Why distinguish both? See: https://stackoverflow.com/questions/16503373/difference-between-char-and-signed-char-in-c
                // // The c++ standarda does not define whether "char" is "signed char" or "unsigned char".
                // template<typename T>
                // static typename std::enable_if<std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value, T>::type
                // callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                // {
                //     return env->CallCharMethod(object, methodID);
                // }

                template<typename T>
                static typename std::enable_if<std::is_same<T, bool>::value, T>::type
                callPrimitiveMethod(JNIEnv* env, jobject& object, jmethodID methodID)
                {
                    return env->CallBooleanMethod(object, methodID);
                }

                // ================================================================================

                // Why not map char to java/lang/Character? Because Character class in Java is 16 bits, while in C++ its only always 8 bits.
                // Hence, Java's byte type corresponds to C++ chars type. 
                // And what about the java/lang/Character type? There is no corresponding C++ type.
                // If there is a java/lang/Character object coming from Java, it needs to be converted to uint16_t (unsigned 16 bit signed integer).
                // See https://docs.oracle.com/javase/tutorial/java/nutsandbolts/datatypes.html
                template<typename T>
                static typename std::enable_if<std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value, T>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetFloatField(object, fieldID, value);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value, T>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetFloatField(object, fieldID, value);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value, T>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetFloatField(object, fieldID, value);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value, T>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetFloatField(object, fieldID, value);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, float>::value, void>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetFloatField(object, fieldID, value);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, double>::value, void>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetDoubleField(object, fieldID, value);
                }

                // Note that Java's Character class is 16 bit. 
                // Further note: char16_t can neither be signed nor unsigned.
                template<typename T>
                static typename std::enable_if<std::is_same<T, char16_t>::value, void>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetCharField(object, fieldID, value);
                }

                template<typename T>
                static typename std::enable_if<std::is_same<T, bool>::value, void>::type
                setPrimitiveField(JNIEnv* env, jfieldID fieldID, jobject& object, T& value)
                {
                    env->SetBooleanField(object, fieldID, value);
                }
                

            public:
                // See https://stackoverflow.com/questions/13263340/findclass-from-any-thread-in-android-jni/16302771#16302771
                static void onLoad(JavaVM* javaVM)
                {
                    JNIUtils::jvm = javaVM;
                    JNIEnv* env = getEnv();

                    // Just use any class.
                    std::string className = "java/lang/Class";
                    auto randomClass = env->FindClass(className.c_str());

                    if(randomClass == nullptr)
                    {
                        CLAID_THROW(claid::Exception, "Error in claid::JNIUtils::onLoad. Cannot find class " << className << " therefore cannot find ClassLoader.")
                    }

                    jclass classClass = env->GetObjectClass(randomClass);

                    auto classLoaderClass = env->FindClass("java/lang/ClassLoader");
                    auto getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader",
                                                            "()Ljava/lang/ClassLoader;");

                    jobject tmpClassLoader = env->CallObjectMethod(randomClass, getClassLoaderMethod);
                    gClassLoader = env->NewGlobalRef(tmpClassLoader);
                    gFindClassMethod = env->GetMethodID(classLoaderClass, "findClass",
                                                    "(Ljava/lang/String;)Ljava/lang/Class;");
                    onLoadCalled = true;
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
                        CLAID_THROW(Exception, "Error, JNIUtils::findClass was called without prior call to onLoad. Please make sure that JNIUtils::onLoad "
                        "was called in JNI_OnLoad in native code.");
                    }



                    jstring str = env->NewStringUTF(name);
                    jclass cls = static_cast<jclass>(env->CallObjectMethod(gClassLoader, gFindClassMethod, str));
                
                    env->DeleteLocalRef(str);

                    if(cls == nullptr)
                    {
                        // If failed, try conventional method.
                        env->ExceptionClear();
                        cls = env->FindClass(name);
                    }
                    
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
                    jstring characterSetString =  env->NewStringUTF("UTF-8");
                    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, characterSetString);

                    size_t length = (size_t) env->GetArrayLength(stringJbytes);
                    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

                    std::string ret = std::string((char*) pBytes, length);
                    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

                    env->DeleteLocalRef(stringJbytes);
                    env->DeleteLocalRef(stringClass);
                    env->DeleteLocalRef(characterSetString);
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

                static bool isJavaByteObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Byte";
                }

                static bool isJavaShortObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Short";
                }

                static bool isJavaIntegerObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Integer";
                }

                static bool isJavaLongObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Long";
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

                static bool isJavaCharObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.Character";
                }

                static bool isJavaStringObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.String";
                }

                static bool isJavaStringBuilderObject(JNIEnv* env, jobject data)
                {
                    std::string className = getNameOfClassOfObject(env, data);
                    return className == "java.lang.StringBuilder";
                }

                

                // ================================================================================================

                // Set primitive
                template<typename T>
                static void javaPrimitiveObjectToNativePrimitive(JNIEnv* env, T& nativePrimitive, jobject& javaObject)
                {
                    std::string expectedJavaPrimitiveClassName = Signatures::Class::signatureToClassName(Signatures::Primitive::getJavaClassNameOfPrimitiveType<T>());

                    std::string className = getNameOfClassOfObject(env, javaObject);
                    
                    // Does java object match the primitive?
                    if(className != expectedJavaPrimitiveClassName)
                    {
                        CLAID_THROW(Exception, "Error, cannot convert javaObject to native C++ primitive " << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<T>() << "."
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
                        CLAID_THROW(Exception, "Error, cannot convert java object of type \"" << className << "\" to native C++ primitive \"" << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<T>() << "\"."
                        << "Method \"" << getterMethodName << "\" was not found in object of type \"" << className << "\"");
                    } 

                    nativePrimitive = callPrimitiveMethod<T>(env, javaObject, mGetValue);
                }

               
                // Modify java primitive object (mutate normally immutable object).
                template<typename T>
                static void assignNativePrimitiveToJavaPrimitiveObject(JNIEnv* env, T& nativePrimitive, jobject& javaObject)
                {
                    // Java primitive objects like Integer, Boolean, Float etc. are usually immutable.
                    // However, from JNI, you can still change them by just adressing the corresponding value field.
                    std::string expectedJavaPrimitiveClassName = Signatures::Class::signatureToClassName(Signatures::Primitive::getJavaClassNameOfPrimitiveType<T>());

                    std::string className = getNameOfClassOfObject(env, javaObject);
                    
                    // Does java object match the primitive?
                    if(className != expectedJavaPrimitiveClassName)
                    {
                        CLAID_THROW(Exception, "Error, cannot convert native C++ primitive " << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<T>() << "to java object."
                        << "Java object is of type \"" << className << "\", however an object of type \"" << expectedJavaPrimitiveClassName << "\" was expected.");
                    }

                    jclass clazz = env->GetObjectClass(javaObject);
                    jfieldID fieldID = env->GetFieldID(clazz, "value", Signatures::Primitive::getSignatureOfPrimitiveType<T>().c_str());

                    setPrimitiveField<T>(env, fieldID, javaObject, nativePrimitive);

                    env->DeleteLocalRef(clazz);
                }
                


                static bool objectCallIntFunction(JNIEnv* env, jobject object, const std::string& functionName, const std::string& signature, int& returnValue)
                {
                    Logger::printfln("objectCallIntFunction 1 %s", functionName.c_str());
                    jclass objectClass = getClassOfObject(env, object);
                    Logger::printfln("objectCallIntFunction 2 %s", functionName.c_str());
                    jmethodID methodID = env->GetMethodID(objectClass, functionName.c_str(), signature.c_str());
                    Logger::printfln("objectCallIntFunction 3 %s", functionName.c_str());
                    env->DeleteLocalRef(objectClass);
                    if(methodID == nullptr)
                    {
                        Logger::printfln("objectCallIntFunction 4 %s", functionName.c_str());
                        return false;
                    } 
                        Logger::printfln("objectCallIntFunction 5 %s", functionName.c_str());
                    returnValue = env->CallIntMethod(object, methodID);

                    return true;
                }

                template<typename... Args>
                static bool objectCallVoidFunction(JNIEnv* env, jobject object, const std::string& functionName, const std::string& signature, Args... args)
                {
                    Logger::printfln("Call void function %s 1", functionName.c_str());
                    jclass objectClass = getClassOfObject(env, object);
                    jmethodID methodID = env->GetMethodID(objectClass, functionName.c_str(), signature.c_str());
                    Logger::printfln("Call void function %s 2", functionName.c_str());
                    env->DeleteLocalRef(objectClass);
                    if(methodID == nullptr)
                    {
                        return false;
                    } 
                    Logger::printfln("Call void function %s 3 %d", functionName.c_str(), args...);
                    env->CallVoidMethod(object, methodID, args...);
                    Logger::printfln("Call void function %s 4", functionName.c_str());
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

                static bool setElementAtIndex(JNIEnv* env, jobject container, const size_t index, jobject& value)
                {    
                    jclass objectClass = getClassOfObject(env, container);
                    jmethodID methodID = env->GetMethodID(objectClass, "setElementAt", "(Ljava/lang/Object;I)V");
                    env->DeleteLocalRef(objectClass);
                    
                    if(methodID == NULL)
                    {
                        return false;
                    } 
                    env->CallVoidMethod(container, methodID, value, index);


                    return true;
                }

                static jobject createJavaVector(JNIEnv* env, int size)
                {
                    std::string className = Signatures::Class::Vector;
                    jclass cls = JNIUtils::findClass(env, className.c_str());

                    if(cls == nullptr)
                    {
                        CLAID_THROW(Exception, "Cannot create java Vector object, failed to lookup class " << className);
                    }

                    jmethodID constructor = env->GetMethodID(cls, "<init>", "(I)V");

                    if(constructor == nullptr)
                    {
                        CLAID_THROW(Exception, "Cannot create java Vector object, failed to lookup constructor for class " << className);
                    }

                    jobject javaObject = env->NewObject(cls, constructor, size);

                    if(javaObject == nullptr)
                    {
                        CLAID_THROW(Exception, "Cannot create java Vector object, object with class signature " << className << " could not be created.");
                    }
                    env->DeleteLocalRef(cls);

                    return javaObject;
                }

                static JavaVM* getJavaVM()
                {
                    return jvm;
                }

                static JNIEnv* getEnv() 
                {
                    JNIEnv *env;
                    int status = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
                    if(status < 0) {    
                        status = jvm->AttachCurrentThread((void**)&env, NULL);
                        if(status < 0) {        
                            return nullptr;
                        }
                    }
                    return env;
                }
        };
    }
}