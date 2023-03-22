
#include <string>
#include "ClassFactory/ClassFactory.hpp"

#include "jbind11/jbind11.hpp"
namespace java = jbind11;

#include "CLAID.hpp"
#include "XML/XMLDocument.hpp"

#include "JavaWrapper/ChannelDataWrapper.hpp"
#include "JavaWrapper/ChannelWrapper.hpp"
#include "JavaWrapper/GenericJavaReflector.hpp"
using namespace claid;
using namespace claid::JavaWrapper;

class JavaCLAID
{
    public:
        static jobject context;
        static jobject activity;

        static void loadFromXML(std::string path)
        {
            claid::Logger::printfln("JavaCLAID: Loading from XML %s", path.c_str());
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

        #if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)
            static void setContext(jobject context)
            {  
                JNIEnv* env = java::JNIUtils::getEnv();
                JavaCLAID::context = env->NewGlobalRef(context);
            }

            static jobject getContext()
            {
                if(JavaCLAID::context == nullptr)
                {
                    CLAID_THROW(Exception, "Error in JavaCLAID: Cannot return context in getContext().\n"
                    "Context is not available, since no context has been set.\n"
                    "Please use CLAID.setContext(...) to set the context.\n"
                    "E.g., if you are starting CLAID from within MainActivity, simply do CLAID.context(this.getBaseContext()) BEFORE CLAID.start()");
                }   
           
                return JavaCLAID::context;
            }         

            static void setActivity(jobject activity)
            {  
                JNIEnv* env = java::JNIUtils::getEnv();
                JavaCLAID::activity = env->NewGlobalRef(activity);
            }

            static jobject getActivity()
            {
                if(JavaCLAID::activity == nullptr)
                {
                    CLAID_THROW(Exception, "Error in JavaCLAID: Cannot return activity in getActivity().\n"
                    "Activity is not available, since no activity has been set.\n"
                    "Please use CLAID.setActivity(...) and provide an activity.\n"
                    "E.g., if you are starting CLAID from within MainActivity, simply do CLAID.context(this)) BEFORE CLAID.start()");
                }   
           
                return JavaCLAID::context;
            }   
        #endif
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
;

    #if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)
    cls.def_static("setContext", &JavaCLAID::setContext);
    cls.def_static("getContext", &JavaCLAID::getContext);
    cls.def_static("setActivity", &JavaCLAID::setActivity);
    cls.def_static("getActivity", &JavaCLAID::getActivity);
    #endif

    JavaModule::addToJbindPackage(p);
    ChannelWrapper::addToJbindPackage(p);
    ChannelDataWrapper::addToJbindPackage(p);
    GenericJavaReflector::addToJbindPackage(p);
}

jobject JavaCLAID::context = nullptr;
jobject JavaCLAID::activity = nullptr;
