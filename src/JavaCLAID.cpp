
#include <string>
#include "ClassFactory/ClassFactory.hpp"

#include "jbind11/jbind11.hpp"
namespace java = jbind11;

#include "CLAID.hpp"
#include "XML/XMLDocument.hpp"

#include "JavaWrapper/ChannelDataWrapper.hpp"
#include "JavaWrapper/ChannelWrapper.hpp"

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

        static void loadFromXMLString(std::string xmlString)
        {
            claid::XMLDocument claidDocument;
            claidDocument.loadFromString(xmlString);
            CLAID_RUNTIME->loadFromXML(claidDocument);
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

        static void enableLoggingToFile(std::string path)
        {
            CLAID_RUNTIME->enableLoggingToFile(path);
        }

        static void disableLoggingToFile()
        {
            CLAID_RUNTIME->disableLoggingToFile();
        }
};

JBIND11_PACKAGE(JavaCLAID, p)  
{
    java::JavaClass<JavaCLAID> cls(p, "CLAID");
    cls.def_static("loadFromXML", &JavaCLAID::loadFromXML);
    cls.def_static("loadFromXMLString", &JavaCLAID::loadFromXMLString);
    cls.def_static("start", &JavaCLAID::start);
    cls.def_static("startInSeparateThread", &JavaCLAID::startInSeparateThread);
    cls.def_static("startNonBlockingWithoutUpdates", &JavaCLAID::startNonBlockingWithoutUpdates);
    cls.def_static("connectTo", &JavaCLAID::connectTo);
    cls.def_static("enableLoggingToFile", &JavaCLAID::enableLoggingToFile);
    cls.def_static("disableLoggingToFile", &JavaCLAID::disableLoggingToFile);

    JavaModule::addToJbindPackage(p);
    ChannelWrapper::addToJbindPackage(p);
    ChannelDataWrapper::addToJbindPackage(p);
}
