
#include <string>
#include "ClassFactory/ClassFactory.hpp"

#include "jbind11/jbind11.hpp"
namespace java = jbind11;



#include "JavaWrapper/JavaModule.hpp"


using namespace claid;
using namespace claid::JavaWrapper;
JBIND11_PACKAGE(JavaCLAID, p) 
{
    JavaModule::addToJbindPackage(p);
    ChannelWrapper::addToJbindPackage(p);
    // // How can we assure that all classes were registered before calling this ? 
    // std::vector<std::string> registeredClasses;

    // claid::PythonWrapper::PythonWrapperMaster::getInstance()->getRegisteredWrapperClasses(registeredClasses);

    // // claid::PythonWrapper::PythonWrapperBase* wrapper = claid::PythonWrapper::PythonWrapperMaster::getInstance()->getWrapperForNativeClass("claid_AudioData");

    // // if(wrapper == nullptr)
    // // {
    // //     CLAID_THROW(Exception, "Error, AudioDataWrapper not found!");
    // // }
    // // wrapper->overridePybindWrapperGenerator(new PybindWrapperGeneratorAudioData());


    // printf("Getting registered classes\n");
    // for(const std::string& str : registeredClasses)
    // {
    //     claid::PythonWrapper::PythonWrapperBase* wrapper = 
    //         claid::PythonWrapper::PythonWrapperMaster::getInstance()->getWrapperForNativeClass(str);
    //     wrapper->generateWrapper(m);
    // }
    // // Manually add PythonModule and ChannelWrapper (cannot use CLAID's reflection system for this because of cyclic dependencies).
    // PythonModule::addToPybindModule(m);
    // ChannelWrapper::addToPybindModule(m);


    // py::class_<PythonModule>(m, "PythonModule")
    //     .def(py::init<>())
    //     .def("print", &PythonModule::print);


    
    // py::class_<Person> cls(m, "Person");
    // // PythonReflector<py::class_<Person>> reflector(cls);

    // cls.def(py::init<>());
    // // reflector.wrap<Person>();
    // }

    // // {
    // // py::class_<Information> cls(m, "Information");
    // // PythonReflector<py::class_<Information>> reflector(cls);

    // // cls.def(py::init<>());
    // // reflector.wrap<Information>();
    // // }   
}
