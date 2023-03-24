#pragma once
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

                static void addToJbindPackage(jbind11::JavaPackage& package)
                {
                    java::JavaClass<GenericJavaReflector> javaClass(package, "Reflector");

                    javaClass.def("reflect", &GenericJavaReflector::reflectFromJava, java::GenericFunction(), java::GenericParams({1}));
                    javaClass.def("reflectWithDefaultValue", &GenericJavaReflector::reflectFromJava, java::GenericFunction(), java::GenericParams({1, 2}));

                //  pyClass.def("publish", &PythonModule::publish);
                }


                virtual void reflectFromJava(std::string memberFieldName, jobject member)
                {
                    std::cout << "GenericReflect reflectFromJava called\n";
                };

                // virtual void reflectFromJavaWithDefaultValue(std::string memberFieldName, jobject member, jobject defaultValue)
                // {
                //     std::cout << "GenericReflect reflectFromJava called\n";
                // };
        };
    };
}