#pragma once

#include "GenericJavaReflector.hpp"
#include "jbind11/JNIUtils/JNIUtils.hpp"

namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        template<typename Reflector>
        class TypedJavaReflector : public GenericJavaReflector
        {  
            jobject objectToReflect;
            Reflector* reflector;

            TypedJavaReflector(jobject objectToReflect, Reflector* reflector) : reflector(reflector), objectToReflect(objectToReflect)
            {

            }

            
            virtual void reflect(std::string memberFieldName, jobject member)
            {
                JNIEnv* env = java::JNIUtils::getEnv();
                std::string className = java::getNameOfClassOfObject(member);

                if(className = java::JNIUtils::)
            };

            std::function<void (T&)> setter;
            std::function<T& ()> getter;

            template<typename T>
            void javaPrimitiveSetter(const std::string memberFieldName, jobject member, T&)
            {
                if(getPrimitiveFieldd)
            }

            void checkIsNative()
            {
                if(className == "java.lang.Byte")
                {
                    reflector.member(memberFieldName, &nativeFieldGetter<int8_t>)
                }
                

                else if(className == "java.lang.Short")
            

            
                else if(className == "java.lang.Integer")
            

            
                // Yes, it's "J", because "L" is used for clases.
                else if(className == "java.lang.Long")
        

        
                else if(className == "java.lang.Float")
        
                else if(className == "java.lang.Double")
        
                else if(className == "java.lang.Character")
            

        
                else if(className == "java.lang.Boolean")
             
            }
            
        };
    }
}