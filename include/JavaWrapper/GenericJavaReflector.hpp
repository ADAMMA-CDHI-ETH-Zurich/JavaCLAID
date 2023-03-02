#pragma 
#include <string>
#include "jbind11/jbind11.hpp"

namespace claid
{
    namespace JavaWrapper
    {
        class GenericJavaReflector
        {
            public:
                GenericJavaReflector()
                {

                }

                ~GenericJavaReflector()
                {

                }

                

                virtual void reflectFromJava(std::string memberFieldName, jobject member)
                {

                };

       
        };
    };
}