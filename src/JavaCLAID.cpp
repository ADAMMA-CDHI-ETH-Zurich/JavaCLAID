
#include <string>
#include "ClassFactory/ClassFactory.hpp"

#include "jbind11/jbind11.hpp"
namespace java = jbind11;

#include "JavaWrapper/Android/Caster/ContextCaster.hpp"
#include "JavaWrapper/Android/Context.hpp"
#include "JavaWrapper/Android/AssetUtils.hpp"

#include "CLAID.hpp"
#include "XML/XMLDocument.hpp"

#include "JavaWrapper/ChannelDataWrapper.hpp"
#include "JavaWrapper/ChannelWrapper.hpp"
#include "JavaWrapper/GenericJavaReflector.hpp"
#include "JavaWrapper/JavaModule.hpp"

#include "JavaExtras/StaticBlock.hpp"


using namespace claid;
using namespace claid::JavaWrapper;

class JavaCLAID
{
    public:
        static Context context;

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

        static void addModule(claid::JavaWrapper::JavaModule* module)
        {
            // The jobject reference currently stored in the handle of the java module is only
            // valid as long as we are in this function.
            // Since it is a Module, we need to make sure it will not get destroyed later.
            // Hence, we create a new global ref and override the pointer stored in the handle.

            JNIEnv* env = java::JNIUtils::getEnv();
            jobject javaModuleObject = java::cast(module);
            javaModuleObject = env->NewGlobalRef(javaModuleObject);
            
            java::JavaHandle handle = java::JavaHandle::getHandleFromObject(env, javaModuleObject);
            handle.getData()->setJavaObjectReference(javaModuleObject);
            
            CLAID_RUNTIME->addModule(static_cast<claid::Module*>(module));
        }

        #if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)
            static void setContext(Context context)
            {  
                JNIEnv* env = java::JNIUtils::getEnv();
                JavaCLAID::context = Context(env->NewGlobalRef(context.getJavaObject()));
            }

            static Context getContext()
            {
                if(JavaCLAID::context.getJavaObject() == nullptr)
                {
                    CLAID_THROW(Exception, "Error in JavaCLAID: Cannot return context in getContext().\n"
                    "Context is not available, since no context has been set.\n"
                    "Please use CLAID.setContext(...) to set the context.\n"
                    "E.g., if you are starting CLAID from within MainActivity, simply do CLAID.context(this.getBaseContext()) BEFORE CLAID.start()");
                }   
           
                return JavaCLAID::context;
            }    

            static bool isContextSet()
            {
                return JavaCLAID::context.isValid();
            }     

            static void loadFromAssets(std::string fileName)
            {
                if(!JavaCLAID::context.isValid())
                {
                    CLAID_THROW(Exception, "Error in JavaCLAID.loadFromAssets(...). Cannot load file \"" << fileName << "\" from assets as no valid context was set.\n"
                    << "Did you use CLAID.setContext() and specified the application context ? (Do not pass an activity, at this might be only temporarily.)");
                }

                std::string fileContent;
                if(!AssetUtils::loadAssetsFileToString(JavaCLAID::context, fileName, fileContent))
                {
                    CLAID_THROW(Exception, "Error in JavaCLAID.loadFromAssets(...). Cannot load file \"" << fileName << "\" from assets.\n"
                        << "The file does not exist or cannot be read.");
                }

                claid::Logger::printfln("Loaded: %s\n", fileContent.c_str());

                JavaCLAID::loadFromXMLString(fileContent);      

            }
        #endif
};

JBIND11_PACKAGE(JavaCLAID, p)  
{
    java::JavaClass<JavaCLAID> cls(p, "CLAID", java::StaticBlock("System.loadLibrary(\"JavaCLAID\");"));
    cls.def_static("loadFromXML", &JavaCLAID::loadFromXML);
    cls.def_static("loadFromXMLString", &JavaCLAID::loadFromXMLString);
    cls.def_static("start", &JavaCLAID::start);
    cls.def_static("startInSeparateThread", &JavaCLAID::startInSeparateThread);
    cls.def_static("startNonBlockingWithoutUpdates", &JavaCLAID::startNonBlockingWithoutUpdates);
    cls.def_static("connectTo", &JavaCLAID::connectTo);
    cls.def_static("enableLoggingToFile", &JavaCLAID::enableLoggingToFile);
    cls.def_static("disableLoggingToFile", &JavaCLAID::disableLoggingToFile);
    cls.def_static("addModule", &JavaCLAID::addModule);


    #if defined __JBIND11_CROSSPLATFORM_ANDROID__ || defined(__ANDROID__)
    cls.def_static("setContext", &JavaCLAID::setContext);
    cls.def_static("getContext", &JavaCLAID::getContext);
    cls.def_static("isContextSet", &JavaCLAID::isContextSet);
    cls.def_static("loadFromAssets", &JavaCLAID::loadFromAssets);
    #endif

    JavaModule::addToJbindPackage(p);
    ChannelWrapper::addToJbindPackage(p);
    ChannelDataWrapper::addToJbindPackage(p);
    GenericJavaReflector::addToJbindPackage(p);
}

Context JavaCLAID::context = nullptr;
