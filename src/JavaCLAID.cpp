
#include <string>
#include "ClassFactory/ClassFactory.hpp"

#include "jbind11/jbind11.hpp"
namespace java = jbind11;

#include "CLAID.hpp"

using namespace claid;
using namespace claid::JavaWrapper;

class JavaCLAID
{
    public:
        static void loadFromXML(std::string path)
        {
            claid::Logger::printfln("Loading from XML %s", path.c_str());
            CLAID_RUNTIME->loadFromXML(path);
        }

        static void start()
        {
            CLAID_RUNTIME->start();
        }

        static void startInSeparateThread()
        {
            CLAID_RUNTIME->startInSeparateThread();
        }

        static void startNonBlockingWithoutUpdates()
        {
            CLAID_RUNTIME->startNonBlockingWithoutUpdates();
        }

        static void connectTo(std::string ipAddress, int32_t port)
        {
            CLAID_RUNTIME->connectTo(ipAddress, port);
        } 
};

JBIND11_PACKAGE(JavaCLAID, p)  
{
    java::JavaClass<JavaCLAID> cls(p, "CLAID");
    cls.def_static("loadFromXML", &JavaCLAID::loadFromXML);
    cls.def_static("start", &JavaCLAID::start);
    cls.def_static("startInSeparateThread", &JavaCLAID::startInSeparateThread);
    cls.def_static("startNonBlockingWithoutUpdates", &JavaCLAID::startNonBlockingWithoutUpdates);
    cls.def_static("connectTo", &JavaCLAID::connectTo);

    JavaModule::addToJbindPackage(p);
    ChannelWrapper::addToJbindPackage(p);
}
