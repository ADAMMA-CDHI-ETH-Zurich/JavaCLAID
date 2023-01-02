#include "JavaWrapper/JavaModule.hpp"
#include "JavaWrapper/JavaWrapperMaster.hpp"
#include "JavaWrapper/Signatures.hpp"
#include "JavaWrapper/ChannelDataWrapper.hpp"

namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        

        void JavaModule::initialize()
        {
            Logger::printfln("JavaModule init");
            std::cout  << std::flush;
            
            Logger::printfln("JavaModule init 2");
            std::cout << std::flush;
            JNIEnv* env = java::JNIUtils::getEnv();
                        Logger::printfln("JavaModule init 2.5");

            jobject self = java::cast(this);
            Logger::printfln("JavaModule init 3");
            std::cout << std::flush;


            jclass cls = java::JNIUtils::getClassOfObject(env, self);
            Logger::printfln("JavaModule init 4");

            jmethodID mid =
                    env->GetMethodID(cls, "initialize", "()V");
            Logger::printfln("JavaModule init  5");

            if(mid == nullptr)
            {
                // CLAID_THROW(Exception, "Error, function initialize with signature void () not found for class "
                //     << JNIUtils::getClassName(env, cls));
            }
            else
            {
                env->CallVoidMethod(self, mid);
            }


            env->DeleteLocalRef(cls);
        }

        void JavaModule::postInitialize()
        {
            // jclass cls = JNIUtils::getClassOfObject(env, javaObject);

            // jmethodID mid =
            //         env->GetMethodID(cls, "postInitialize", "()V");

            // if(mid == nullptr)
            // {
            //     // CLAID_THROW(Exception, "Error, function initialize with signature void () not found for class "
            //     //     << JNIUtils::getClassName(env, cls));
            // }
            // else
            // {
            //     // Return should be of type java class "Channel" (portaible.JavaWrappers.Channel)
            //     env->CallVoidMethod(javaObject, mid);

            // }

            // env->DeleteLocalRef(cls);
        }

        ChannelWrapper JavaModule::publish(jclass dataType, std::string channelID)
        {
            std::string canonicalClassName = java::JNIUtils::getClassName(java::JNIUtils::getEnv(), dataType);
          
            if(!JavaWrapperMaster::getInstance()->isWrapperRegisteredForJavaClass(canonicalClassName))
            {
                CLAID_THROW(Exception, "Error, JavaModule tried to publish channel\"" << channelID << "\" with data type \"" << canonicalClassName << "\", however"
                << "no JavaWrapper is available for this data type. Please implement a wrapper for this type using DECLARE_SERIALIZATION and"
                << "REGISTER_SERIALIZATION (invasively or non-invasively).");
            }
            JavaWrapperBase* javaWrapper = JavaWrapperMaster::getInstance()->getWrapperForJavaClass(canonicalClassName);
            return javaWrapper->publish(this, channelID);
        }

        ChannelWrapper JavaModule::subscribe(jclass dataType, std::string channelID, std::string functionCallbackName)
        {
            std::string canonicalClassName = java::JNIUtils::getClassName(java::JNIUtils::getEnv(), dataType);
          
            if(!JavaWrapperMaster::getInstance()->isWrapperRegisteredForJavaClass(canonicalClassName))
            {
                CLAID_THROW(Exception, "Error, JavaModule tried to subscribe to channel\"" << channelID << "\" with data type \"" << canonicalClassName << "\", however"
                << "no JavaWrapper is available for this data type. Please implement a wrapper for this type using DECLARE_SERIALIZATION and"
                << "REGISTER_SERIALIZATION (invasively or non-invasively).");
            }

            JavaWrapperBase* javaWrapper = JavaWrapperMaster::getInstance()->getWrapperForJavaClass(canonicalClassName);
            return javaWrapper->subscribe(this, channelID, functionCallbackName);
        }

        void JavaModule::callCallbackFunction(const std::string& functionName, jobject dataObject)
        {
            JNIEnv* env = java::JNIUtils::getEnv();

            // Looks up the handle associated with this JavaModule from the global table.
            jobject self = java::cast(this);


            jclass cls = java::JNIUtils::getClassOfObject(env, self);
            jmethodID mid =
                    env->GetMethodID(cls, functionName.c_str(), Signatures::Function::functionSignatureVoid({Signatures::Class::ChannelData}).c_str());
            
            if(mid == nullptr)
            {
                claid::Logger::printfln("NULL");
                CLAID_THROW(Exception, "Error, could not call callback function. Function " << functionName << " with signature void " << Signatures::Class::ChannelData << " not found for class "
                    << java::JNIUtils::getNameOfClassOfObject(env, dataObject));
            }
            else
            {
                ChannelDataWrapper channelData(dataObject);
                
                jobject javaChannelData = java::cast(channelData);

                env->CallVoidMethod(self, mid, javaChannelData);
                env->DeleteLocalRef(javaChannelData);
                
            }

            env->DeleteLocalRef(cls);
        }
    
        void JavaModule::registerPeriodicFunction(std::string identifier, std::string functionName, int32_t periodInMilliseconds)
        {
            JNIEnv* env = java::JNIUtils::getEnv();
            

            std::function<void()> function = std::bind(&JavaModule::invokeJavaPeriodicFunction, this, functionName);
            Module::registerPeriodicFunction(identifier, function, static_cast<size_t>(periodInMilliseconds));
        }

        void JavaModule::unregisterPeriodicFunction(jstring identifier)
        {
            JNIEnv* env = java::JNIUtils::getEnv();
            std::string stdIdentifier = java::JNIUtils::toStdString(env, identifier);
            Module::unregisterPeriodicFunction(stdIdentifier);
        }

        void JavaModule::invokeJavaPeriodicFunction(std::string functionName)
        {
            jobject self = java::cast(this);

            JNIEnv* env = java::JNIUtils::getEnv();
            jclass cls = java::JNIUtils::getClassOfObject(env, self);
            jmethodID mid =
                    env->GetMethodID(cls, functionName.c_str(), Signatures::Function::functionSignatureVoid({/* No parameter, hence empty vector */}).c_str());
            
            if(mid == nullptr)
            {
                CLAID_THROW(Exception, "Error, registered periodic function could not be called. Function \"" << functionName << "\" with signature void () not found for class "
                    << java::JNIUtils::getClassName(env, cls));
            }
            else
            {
                env->CallVoidMethod(self, mid);
            }

            env->DeleteLocalRef(cls);
        }

        JNIEnv* JavaModule::getEnv()
        {
            return java::JNIUtils::getEnv();
        }

        const std::string JavaModule::getModuleName()
        {
            return this->moduleName;
        }


    }
}