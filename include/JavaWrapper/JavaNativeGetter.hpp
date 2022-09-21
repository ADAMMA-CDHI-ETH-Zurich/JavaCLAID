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
#include "Traits/is_specialization_of.hpp"
#include "JNIFactory.hpp"
#include <deque>
#include <stack>

namespace portaible
{
    namespace JavaWrapper
    {
        class JavaNativeGetter : public Serializer<JavaNativeGetter>
        {
            private:
                jobject currentJavaObject;

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

                std::deque<jobject> createdJavaObjects;

                bool finished = false;
                bool isSequence = false;
                size_t idInSequence = 0;

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
                    

                    return std::string(property) == targetQueue.front() || isSequence;
                }

          

                bool checkTargetQueueValid(const char* property)
                {
                    if(this->targetQueue.size() == 0)
                    {
                        PORTAIBLE_THROW(Exception, "Error in JavaNativeGetter::set. Somehow, callFloatOrDouble was called with an empty target queue. This should not happen and is definitely a bug.");
                    }                  

                    if(this->targetQueue.size() > 1)
                    {
                        PORTAIBLE_THROW(Exception, "Error, failed to get member variable \"" << property << "\" of Native C++ class from Java using get(...) function."
                                << "The type of the member variable is a primitive type (float or double), however it was tried to assign a value to one of it's member variables."
                                << "In other words, the primitive variable \"" << property << "\" does not have a member variable of name \"" << this->targetQueue[1] << "\".");
                    }

                    return true;
                }

                void throwTypeMismatch(const char* property, std::string primitiveType, std::string expectedType)
                {
                    // Alternatively, we could also throw this in JNI..
                    PORTAIBLE_THROW(Exception, "Error, tried to get member variable \"" << property << "\" of Native C++ class from Java using get(...) function, " 
                            << "but the passed object is not a java object of class " << expectedType
                            << "As \"" << property << "\" is a C++ " << primitiveType << " type, it was expected that the passed java object (the value of which shall be assigned to the Java object) is of type " 
                            << expectedType << ", however it is \"" << JNIUtils::getNameOfClassOfObject(env, this->currentJavaObject) << "\".");
                }

                

                template<typename T>
                jobject spawnAndSaveCorrespondingJavaObject(T& member)
                {
                    jobject objectReference = JNIFactory<T>::spawn(env);
                    this->createdJavaObjects.push_back(objectReference);
                    return objectReference;
                }

                template<typename T>
                void handlePrimitiveTarget(const char* property, T& member)
                {
                    if(this->finished)
                        return;

                    if(!this->isCurrentTarget(property))
                    {
                        return;
                    }

                    this->checkTargetQueueValid(property);

                    jobject javaObject = JNIFactory<T>::spawnImmutable(env, member);
                    this->createdJavaObjects.push_back(javaObject);

                    assignJavaObjectAccordingly(javaObject);
         
                    if(!this->isSequence)
                        finished = true;
                }

                void assignJavaObjectAccordingly(jobject javaObject)
                {
                    if(this->isSequence)
                    {
                        jobject container = sequenceObjectsStack.top();
                        JNIUtils::setElementAtIndex(env, container, this->idInSequence, javaObject);

                    }
                    
                  
                    this->currentJavaObject = javaObject;
               
                }

            public:
                
                JavaNativeGetter()
                {
                    
                }

                template<typename T>
                void callFloatOrDouble(const char* property, T& member)
                {
                    handlePrimitiveTarget(property, member);
                }   

                // Also includes any variants of signed, unsigned, short, long, long long, ...
                template<typename T>
                void callInt(const char* property, T& member)
                {
                    handlePrimitiveTarget(property, member);
                }

                void callBool(const char* property, bool& member)
                {
                    handlePrimitiveTarget(property, member);
                }

                // Why template? Because we can have signed and unsigned char.
                template<typename T>
                void callChar(const char* property, T& member)
                {
                    handlePrimitiveTarget(property, member);
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

                    jobject javaObject = JNIFactory<std::string>::spawnImmutable(env, member);
                    this->createdJavaObjects.push_back(javaObject);
                    assignJavaObjectAccordingly(javaObject);

                    if(!this->isSequence)
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
                        jobject javaObject = spawnAndSaveCorrespondingJavaObject(member);
                        assignJavaObjectAccordingly(javaObject);
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
                    // I.e. this->currentJavaObject -> does it have a function size().
                    // currentJavaObject needs to be of type Vector, ArrayList, Map or whatever.

                    std::string javaObjectType = JNIUtils::getNameOfClassOfObject(this->env, this->currentJavaObject); 

                    if(this->isCurrentTarget(this->lastClassProperty.c_str()) && !this->finished)
                    {
                        this->checkTargetQueueValid(this->lastClassProperty.c_str());

                        if(!JNIUtils::objectCallVoidFunction(env, this->currentJavaObject, "setSize", "(I)V", count))
                        {
                            PORTAIBLE_THROW(Exception, "Error, tried to get member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using get(...) function." 
                                << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                                << "As such, it was expected that the Java object has a setSize() function to retrieve the amount of stored elements."
                                << "A setSize() function, however, is not available for the passed Java object of type \"" << javaObjectType << "\".");
                        }
                    }

                    

                }

                void countElements(size_t& count)
                {
                    // e.g. for std::vector<int> -> java Vector<int>, count is 
                    // the size of the java vector.
                    // Check if element has length variable.. 
                    // I.e. this->currentJavaObject -> does it have a function size().
                    // currentJavaObject needs to be of type Vector, ArrayList, Map or whatever.
                    int tmp = 0;
                    if(!JNIUtils::objectCallIntFunction(env, this->currentJavaObject, "size", "()I", tmp))
                    {
                        PORTAIBLE_THROW(Exception, "Error, tried to get member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using get(...) function." 
                            << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                            << "As such, it was expected that the Java object has a size function to retrieve the amount of stored elements."
                            << "A size() function, however, is not available for the passed Java object of type \"" << JNIUtils::getNameOfClassOfObject(env, this->currentJavaObject) << "\".");
                    }

                    count = tmp;
                }

                void beginSequence()
                {
                    sequenceObjectsStack.push(this->currentJavaObject);
                }

                void itemIndex(const size_t i)
                {
                    Logger::printfln("item index 1");
                    this->isSequence = true;
                    this->idInSequence = i;
                    int size = 0;
                    jobject container = sequenceObjectsStack.top();
                    if(!JNIUtils::objectCallIntFunction(env, container, "size", "()I", size))
                    {
                        PORTAIBLE_THROW(Exception, "Error, tried to get member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using get(...) function." 
                            << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                            << "As such, it was expected that the Java object has a size function to retrieve the amount of stored elements."
                            << "A size() function, however, is not available for the passed Java object of type \"" << JNIUtils::getNameOfClassOfObject(env, this->currentJavaObject) << "\".");
                    }
                    Logger::printfln("item index 2");


                    if(i >= size)
                    {
                        PORTAIBLE_THROW(Exception, "Error, tried to get member variable \"" << this->lastClassProperty << "\" of Native C++ class\"" << this->lastClassName << "\" from Java using get(...) function." 
                            << "The Native C++ type is a collection (e.g. vector, map, ...), that stores multiple elements." 
                            << "The provided Java object is a collection as wel, however it does not contain enough elements to fill the Native C++ container (C++: " << i + 1<< " vs. Java: " << size << ").");                    
                    }
                    Logger::printfln("item index 3 %d %d", size, i);


                   

                }

         

                void endSequence()
                {
                    jobject container = this->sequenceObjectsStack.top();
                    this->sequenceObjectsStack.pop();
                    this->isSequence = false;
                }
                
                void write(const char* data, size_t size)
                {

                }

                void enforceName(std::string& name, int idInSequence = 0)
                {

                    

                }

                template<typename NativeType>
                jobject get(JNIEnv* env, NativeType& nativeObject, const std::string& targetVariable)
                {   
                    // References to all objects created within the JavaNativeGetter are stored in the following stack.
                    // The stack is automatically deleted when we leave this function, which in turn leads to destroying all the objects inside.
                    // In the destructor of jobject, all references to jobject will be freed using env->DeleteLocalReference(jobject).       
                

                    this->originalTargetVariable = targetVariable;
                    splitToTargets(targetVariable, this->targetQueue);

                    this->finished = false;
                    this->env = env;

                    invokeReflectOnObject(nativeObject);

                    if(!this->finished)
                    {
                        PORTAIBLE_THROW(Exception, "Tried to get a value  \"" << targetVariable << "\" from Native C++ class \""
                        << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<NativeType>() << "\" and return to Java, however a reflected member variable with name "
                        << "\"" << targetVariable << "\" was not found. Please make sure the variable was included in the reflect function of the mentioned class.");
                    }


                    if(createdJavaObjects.size() == 0)
                    {
                       if(!this->finished)
                        {
                            PORTAIBLE_THROW(Exception, "Tried to assign a value to member variable \"" << targetVariable << "\" to Native C++ class \""
                            << TypeChecking::getCompilerSpecificCompileTypeNameOfClass<NativeType>() << "\" from Java, however somehow no java objects were created by the getter function");
                        } 
                    }


                    jobject tmp = createdJavaObjects.front();
                    this->createdJavaObjects.clear();
                    Logger::printfln("RETURNING %s", JNIUtils::getNameOfClassOfObject(env, tmp).c_str());
                    return tmp;
                }

        };
    }    
}