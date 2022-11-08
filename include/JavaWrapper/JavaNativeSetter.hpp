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
JNIEXPORT void JNICALL Java_JavaCLAID_Wrapper_setSomeInt(JNIEnv* env, jobject object, jint int);
JNIEXPORT jint JNICALL Java_JavaCLAID_Wrapper_getSomeInt(JNIEnv* env, jobject object);
JNIEXPORT void JNICALL Java_JavaCLAID_Wrapper_setSomeFloat(JNIEnv* env, jobject object, jfloat int);
JNIEXPORT jfloat JNICALL Java_JavaCLAID_Wrapper_getSomeFloat(JNIEnv* env, jobject object);
JNIEXPORT void JNICALL Java_JavaCLAID_Wrapper_setSomeString(JNIEnv* env, jobject object, jobject int);
JNIEXPORT jobject JNICALL Java_JavaCLAID_Wrapper_getSomeString(JNIEnv* env, jobject object);


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

#include "Serialization/Serializer.hpp"
#include "TypeChecking/TypeCheckingFunctions.hpp"
#include "PolymorphicReflector/PolymorphicReflector.hpp"
#include "Traits/is_specialization_of.hpp"
#include <deque>
#include <stack>

namespace claid
{
    namespace JavaWrapper
    {
        class JavaNativeSetter : public Deserializer<JavaNativeSetter>
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
                std::string originalTargetVariable;

                // Required to deserialize collections, e.g. vector.
                // Used in beginSequence() and endSequence()
                std::stack<jobject> sequenceObjectsStack;

                bool finished = false;
              
                JNIEnv* env;

                // Stores the compiler specific compile time name of the class that was last used to
                // call beginClass(...)
                std::string lastClassName;
                // Stores the property name of the member that was last used to call beginClass(...)
                std::string lastClassProperty; 

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
                    

                    return std::string(property) == targetQueue.front() || isSequence();
                }

                bool isSequence()
                {
                    return this->sequenceObjectsStack.size() > 0;
                }

                bool checkTargetQueueValid(const char* property)
                {
                    if(this->targetQueue.size() == 0)
                    {
                        CLAID_THROW(Exception, "Error in JavaNativerSetter::set. Somehow, callFloatOrDouble was called with an empty target queue. This should not happen and is definitely a bug.");
                    }                  

                    if(this->targetQueue.size() > 1)
                    {
                        CLAID_THROW(Exception, "Error, failed to set member variable \"" << property << "\" of Native C++ class from Java using set(...) function."
                                << "The type of the member variable is a primitive type (float or double), however it was tried to assign a value to one of it's member variables."
                                << "In other words, the primitive variable \"" << property << "\" does not have a member variable of name \"" << this->targetQueue[1] << "\".");
                    }

                    return true;
                }

                void throwTypeMismatch(const char* property, std::string primitiveType, std::string expectedType)
                {
                     CLAID_THROW(Exception, "Error, tried to set member variable \"" << property << "\" of Native C++ class from Java using set(...) function, " 
                            << "but the passed object is not a java object of class " << expectedType
                            << "As \"" << property << "\" is a C++ " << primitiveType << " type, it was expected that the passed java object (the value of which shall be assigned to the C++ object) is of type " 
                            << expectedType << ", however it is \"" << JNIUtils::getNameOfClassOfObject(env, this->valueToSet) << "\".");
                }

                template<typename T>
                void setPrimitive(const char* property, T& member)
                {
                    if(this->finished)
                        return;

                    if(!this->isCurrentTarget(property))
                    {
                        return;
                    }

                    this->checkTargetQueueValid(property);

                    JNIUtils::javaPrimitiveObjectToNativePrimitive(env, member, this->valueToSet);
         

                    if(!this->isSequence())
                        finished = true;
                }

            public:
                
                JavaNativeSetter()
                {
                    
                }

                template<typename T>
                void callFloatOrDouble(const char* property, T& member)
                {
                    setPrimitive(property, member);
                }   

                // Also includes any variants of signed, unsigned, short, long, long long, ...
                template<typename T>
                void callInt(const char* property, T& member)
                {
                    setPrimitive(property, member);
                }

                void callBool(const char* property, bool& member)
                {
                    setPrimitive(property, member);
                }

                // Why template? Because we can have signed and unsigned char.
                template<typename T>
                void callChar(const char* property, T& member)
                {
                    setPrimitive(property, member);
                }

                void callString(const char* property, std::string& member)
                {
                    if(this->finished)
                        return;

                    if(!this->isCurrentTarget(property))
                    {
                        return;
                    }

                    this->checkTargetQueueValid(property);

                    if(!JNIUtils::isJavaStringObject(this->env, this->valueToSet))
                    {
                        throwTypeMismatch(property, "std::string", "java/lang/String");
                    }

                    member = JNIUtils::toStdString(this->env, valueToSet);
                    if(!this->isSequence())
                        finished = true;
                }

                template<typename T>
                void callBeginClass(const char* property, T& member)
                {
                    // Store class and property names for later use, might be required (e.g., to print error messages).
                    // Why use compiler independent name here? Because otherwise, sth like std::vector<std::vector<int>> might result in:
                    // std::__ndk1::vector<std::__ndk1::vector<int, std::__ndk1::allocator<int> >, std::__ndk1::allocator<std::__ndk1::vector<int, std::__ndk1::allocator<int> > > >
                    this->lastClassName = TypeChecking::getCompilerIndependentTypeNameOfClass<T>();
                    this->lastClassProperty = std::string(property);

                    // For nested containers, like vector<vector<int>>, it could happen that callBeginClass() is called
                    // multiple times for the same property (i.e. for the inner and the outer vector).
                    // However, in callEndClass, finished will be set property is the current target,
                    // which would lead to stop assignment of any further variables (nested vectors).
                    // This, if callBeginClass is called, and property is still the current target, we set finished to false again.
                    // Only the LAST callEndClass of nested containers should finish the process.
                    if(this->isCurrentTarget(property))
                    {
                        finished = false;
                    }
                }

                template<typename T>
                void callEndClass(const char* property, T& member)
                {

                    // If the class was set ("deserialized") successfully,
                    // set to finish.
                    if(this->isCurrentTarget(property) && !finished)
                    {
                        this->finished = true;
                    }

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
                    // e.g. for std::vector<int> -> java Vector<int>, count is 
                    // the size of the java vector.
                    // Check if element has length variable.. 
                    // I.e. this->valueToSet -> does it have a function size().
                    // valueToSet needs to be of type Vector, ArrayList, Map or whatever.

                    if(this->isCurrentTarget(this->lastClassProperty.c_str()) && !this->finished)
                    {
                        this->checkTargetQueueValid(this->lastClassProperty.c_str());

                        int tmp = 0;
                        if(!JNIUtils::objectCallIntFunction(env, this->valueToSet, "size", "()I", tmp))
                        {
                            CLAID_THROW(Exception, "Error, tried to set member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using set(...) function." 
                                << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                                << "As such, it was expected that the Java object has a size function to retrieve the amount of stored elements."
                                << "A size() function, however, is not available for the passed Java object of type \"" << JNIUtils::getNameOfClassOfObject(env, this->valueToSet) << "\".");
                        }

                        count = tmp;
                    }

                    

                }

                void countElements(size_t& count)
                {
                    // e.g. for std::vector<int> -> java Vector<int>, count is 
                    // the size of the java vector.
                    // Check if element has length variable.. 
                    // I.e. this->valueToSet -> does it have a function size().
                    // valueToSet needs to be of type Vector, ArrayList, Map or whatever.
                    int tmp = 0;
                    if(!JNIUtils::objectCallIntFunction(env, this->valueToSet, "size", "()I", tmp))
                    {
                        CLAID_THROW(Exception, "Error, tried to set member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using set(...) function." 
                            << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                            << "As such, it was expected that the Java object has a size function to retrieve the amount of stored elements."
                            << "A size() function, however, is not available for the passed Java object of type \"" << JNIUtils::getNameOfClassOfObject(env, this->valueToSet) << "\".");
                    }

                    count = tmp;
                }

                void beginSequence()
                {
                    sequenceObjectsStack.push(this->valueToSet);
                }

                void itemIndex(const size_t i)
                {
                    int size = 0;
                    jobject container = sequenceObjectsStack.top();
                    if(!JNIUtils::objectCallIntFunction(env, container, "size", "()I", size))
                    {
                        CLAID_THROW(Exception, "Error, tried to set member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using set(...) function." 
                            << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                            << "As such, it was expected that the Java object has a size function to retrieve the amount of stored elements."
                            << "A size() function, however, is not available for the passed Java object of type \"" << JNIUtils::getNameOfClassOfObject(env, this->valueToSet) << "\".");
                    }


                    if(i >= size)
                    {
                        CLAID_THROW(Exception, "Error, tried to set member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using set(...) function." 
                            << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                            << "The provided Java object is a collection as wel, however it does not contain enough elements to fill the Native C++ container (C++: " << i + 1<< " vs. Java: " << size << ").");                    
                    }


                    if(!JNIUtils::getElementAtIndex(this->env, container, i, this->valueToSet))
                    {
                        CLAID_THROW(Exception, "Error, tried to set member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using set(...) function." 
                                << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                                << "As such, it was expected that the Java object has a \"elementAt()\" to retrieve an Element by index."
                                << "This function, however, is not available for the passed Java object of type \"" << JNIUtils::getNameOfClassOfObject(env, this->valueToSet) << "\".");
                    }


                }

         

                void endSequence()
                {
                    jobject container = this->sequenceObjectsStack.top();
                    this->sequenceObjectsStack.pop();
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

                void read(char*& data, size_t size)
                {
                    CLAID_THROW(Exception, "Error, JavaNativeSetter::read called. Function not implemented!");
                }

                template<typename NativeType>
                void set(JNIEnv* env, NativeType& nativeObject, const std::string& targetVariable, jobject value)
                {
                    this->originalTargetVariable = targetVariable;
                    splitToTargets(targetVariable, this->targetQueue);

              


                    this->valueToSet = value;
                    this->finished = false;
                    this->env = env;

                    invokeReflectOnObject(nativeObject);

                    if(!this->finished)
                    {
                        CLAID_THROW(Exception, "Tried to assign a value to member variable \"" << targetVariable << "\" to Native C++ class \""
                        << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<NativeType>() << "\" from Java, however a reflected member variable with name "
                        << "\"" << targetVariable << "\" was not found. Please make sure the variable was included in the reflect function of the mentioned class.");
                    }

                    

                }

        };
    }    
}