// The JavaNativeSetter and the JavaNativeGetter use CLAID's reflection system in order to easily make member variables of native C++ classes/structs
// available to Java classes.
// This prevents the necessity to implement getter/setter functions to access members of the C++ classes/structs.
// (E.g., if you want to use a C++ class/struct in java, you usually need to provide jni getters and setters for each individual member)
// Example:
/*
WITHOUT USING CLAIDS REFLECTION SYSTEM:

struct MyStruct
{
    int someInt;
    float someFloat;
    std::string someString;
};
// Implement wrapper functions for each variable (function bodies not implemented here because I think the point is clear,
// but ofc, normally you'd need to implement all those functions):
JNIEXPORT void JNICALL Java_com_example_portaible_Wrapper_setSomeInt(JNIEnv* env, jobject object, jint int);
JNIEXPORT jint JNICALL Java_com_example_portaible_Wrapper_getSomeInt(JNIEnv* env, jobject object);
JNIEXPORT void JNICALL Java_com_example_portaible_Wrapper_setSomeFloat(JNIEnv* env, jobject object, jfloat int);
JNIEXPORT jfloat JNICALL Java_com_example_portaible_Wrapper_getSomeFloat(JNIEnv* env, jobject object);
JNIEXPORT void JNICALL Java_com_example_portaible_Wrapper_setSomeString(JNIEnv* env, jobject object, jobject int);
JNIEXPORT jobject JNICALL Java_com_example_portaible_Wrapper_getSomeString(JNIEnv* env, jobject object);


Java:

public class MyStruct extends Wrapper<MyStruct>
{
    public void test()
    {
        setSomeInt(10);
        setSomeFloat(13.37);
        setSomeString("SixtyNine");
    }
}


============================================================================================================================

WITH USING CLAIDs REFLECTION SYSTEM:
C++:

struct MyStruct
{
    int someInt;
    float someFloat;
    std::string someString;

    template<typename Reflector>
    void reflect(Reflector& r)
    {
        r.member("SomeInt", someInt, "");
        r.member("SomeFloat", someFloat, "");
        r.member("SomeString", someString, "");
    }
};

Java:

public class MyStruct extends Wrapper<MyStruct>
{
    public void test()
    {
        set("SomeInt", 10);
        set("SomeFloat", 13.37);
        set("SomeString", "SixtyNine");
       
        int someInt = get("SomeInt");
        float someFloat = get("SomeFloat");
        String someString = get("SomeString");
    }
}

*/

#pragma once
#include "JNIUtils.hpp"
#include "ClassFactory/ClassFactory.hpp"

#include "Reflection/Serializer.hpp"
#include "TypeChecking/TypeCheckingFunctions.hpp"
#include "PolymorphicReflector/PolymorphicReflector.hpp"

#include <deque>

namespace portaible
{
    namespace JavaWrapper
    {
        class JavaNativeSetter : public Serializer<JavaNativeSetter>
        {
            private:
                jobject valueToSet;

                // If the target variable is a member of a nested class/struct
                // (class/struct, that is a member of the previous class/struct), then this variable can
                // be adressed using dots. I.e.:
                /*
                    class Engine
                    {
                        int power;
                        // reflect..
                    };

                    class Car
                    {
                        Engine engine;
                        // reflect..
                    };
                    

                    set("Car.Engine.Power", 42);
                */ 
                // In that case, the target variable needs to be split by dots and we have multiple targets:
                // First "Car", then "Engine", then "Power".
                // The targets are stored by this variable.
                // If the target is a direct member of the class, then ofc the following variable only has one entry.
                std::deque<std::string> targetQueue;

                bool finished = false;
                JNIEnv* env;

                void splitToTargets(const std::string& targetString, std::deque<std::string>& targetQueue)
                {
                    targetQueue.clear();
                    std::vector<std::string> tmp;
                    splitStringToVector(targetString, ".", tmp);

                    for(const std::string& str : tmp)
                    {
                        targetQueue.push_back(str);
                    }
                }

                bool isCurrentTarget(const char* property)
                {
                    if(targetQueue.size() == 0)
                    {
                        return false;
                    }
                    return std::string(property) == targetQueue.front();
                }

            


            public:
                
                JavaNativeSetter()
                {
                    
                }

                template<typename T>
                void callFloatOrDouble(const char* property, T& member)
                {
                    if(!this->isCurrentTarget(property) || this->finished)
                    {
                        return;
                    }

                    if(this->targetQueue.size() > 1)
                    {
                        PORTAIBLE_THROW(Exception, "Error, failed to set member variable \"" << property << "\" of Native C++ class from Java using set(...) function."
                                << "The type of the member variable is a primitive type (float or double), however it was tried to assign a value to one of IT's member variables."
                                << "In other words, the primitive variable \"" << property << "\" does not have a member variable of name \"" << this->targetQueue[2] << "\".");
                    }

                    if(std::is_same<T, float>::value)
                    {
                        if(!JNIUtils::isJavaFloatObject(this->env, this->valueToSet))
                        {
                            PORTAIBLE_THROW(Exception, "Error, tried to set member variable \"" << property << "\" of Native C++ class from Java using set(...) function, " 
                            << "but the passed object is not a java object of class Float."
                            << "As \"" << property << "\" is a C++ float type, it was expected that the passed java object (the value of which shall be assigned to the C++ object) is of type " 
                            << "java/lang/Float, however it is \"" << JNIUtils::getClassName(this->env, JNIUtils::getClassOfObject(this->env, this->valueToSet)) << "\".");
                        }
                        member = JNIUtils::floatObjectToPrimitiveFloat(this->env, valueToSet);
                        finished = true;
                    }
                    else
                    {
                        if(!JNIUtils::isJavaDoubleObject(this->env, this->valueToSet))
                        {
                            PORTAIBLE_THROW(Exception, "Error, tried to set member variable \"" << property << "\" of Native C++ class from Java using set(...) function, " 
                            << "but the passed object is not a java object of class Double."
                            << "As \"" << property << "\" is a C++ double type, it was expected that the passed java object (the value of which shall be assigned to the C++ object) is of type " 
                            << "java/lang/Double, however it is \"" << JNIUtils::getClassName(this->env, JNIUtils::getClassOfObject(this->env, this->valueToSet)) << "\".");
                        }
                        member = JNIUtils::doubleObjectToPrimitiveDouble(this->env, this->valueToSet);
                        finished = true;
                    }
                }   


                // Also includes any variants of signed, unsigned, short, long, long long, ...
                template<typename T>
                void callInt(const char* property, T& member)
                {
                }

                void callBool(const char* property, bool& member)
                {
                }

                void callChar(const char* property, char& member)
                {
                }

                void callString(const char* property, std::string& member)
                {
                }

                template<typename T>
                void callBeginClass(const char* property, T& member)
                {
                    
                }

                template<typename T>
                void callEndClass(const char* property, T& member)
                {

                }


                template<typename T>
                void callPointer(const char* property, T*& member)
                {
                                
                }
                
                template<typename T>
                void callSharedPointer(const char* property, T& member)
                {
                    
                }

                template<typename T>
                void callEnum(const char* property, T& member)
                {
             
                }

                void count(const std::string& name, size_t& count)
                {

                }

                void countElements(size_t& count)
                {

                }

                void beginSequence()
                {
                    // Do nothing
                }

                void endSequence()
                {
                    // Do nothing
                }
                
                void write(const char* data, size_t size)
                {

                }

                void enforceName(std::string& name, int idInSequence = 0)
                {
                    // Some serializers, like JavaNativeSetter, might not store the members name (i.e., property parameters),
                    // as it is not necessary to be known in the binary data).
                    // For some cases, however, it might be necessary to store such strings in the serialized data nevertheless,
                    // as it might be needed for deserialization etc.
                    // Thus, this function allows to make sure the string "name" is explicitly stored.
                    

                }

                template<typename NativeType>
                void set(JNIEnv* env, NativeType& nativeObject, const std::string& targetVariable, jobject value)
                {
                    splitToTargets(targetVariable, this->targetQueue);
                    this->valueToSet = value;
                    this->finished = false;
                    this->env = env;

                    invokeReflectOnObject(nativeObject);

                    if(!this->finished)
                    {
                        PORTAIBLE_THROW(Exception, "Tried to assign a value to member variable \"" << targetVariable << "\" to Native C++ class \""
                        << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<NativeType>() << "\" from Java, however a reflected member variable with name "
                        << "\"" << targetVariable << "\" was not found. Please make sure the variable was included in the reflect function of the mentioned class.");
                    }

                }

        };
    }    
}