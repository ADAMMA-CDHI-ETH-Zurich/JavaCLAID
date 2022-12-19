
#include <string>
#include "ClassFactory/ClassFactory.hpp"

#include "jbind11/jbind11.hpp"
namespace java = jbind11;



#include "JavaWrapper/JavaModule.hpp"


using namespace claid;
using namespace claid::JavaWrapper;
JBIND11_PACKAGE(JavaCLAIDDataTypes, p) 
{
    // It is safe to assume that all classes have been statically
    // registered to the JavaWrapperMaster because the JBIND11_PACKAGE declaration is executed.
    // The reason for that is that all packages definitions are executed by jbind11 when
    // after shared library has been loaded by the java runtime, whereas all static class registrations
    // are done at link time (dlopen -> static registrations -> JNI_OnLoad()).
    std::vector<std::string> registeredClasses;

    JavaWrapperMaster::getInstance()->getRegisteredWrapperClasses(registeredClasses);


    printf("Getting registered classes\n");
    for(const std::string& str : registeredClasses)
    {
        JavaWrapperBase* wrapper = 
            JavaWrapperMaster::getInstance()->getWrapperForNativeClass(str);
        wrapper->generateWrapper(p);
    }

}
