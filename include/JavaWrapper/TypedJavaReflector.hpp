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

                if(isPrimitive(className))
                {
                    handlePrimitive(member);
                }
                else
                {
                    handleWithWrapper(member);
                }
            };

            std::function<void (T&)> setter;
            std::function<T& ()> getter;

            template<typename T>
            void javaPrimitiveSetter(const std::string memberFieldName, jobject member, T&)
            {
                if(getPrimitiveFieldd)
            }

            void isPrimitive()
            {
                return  className == "java.lang.Byte" ||
                        className == "java.lang.Short" ||
                        className == "java.lang.Integer" ||
                        className == "java.lang.Long" ||
                        className == "java.lang.Float" ||
                        className == "java.lang.Double" ||
                        className == "java.lang.Character" ||
                        className == "java.lang.Boolean";
             
            }

            void handlePrimitive()
            {
                                    reflector.member(memberFieldName, &nativeFieldGetter<int8_t>)

            }
            
        };
    }
}