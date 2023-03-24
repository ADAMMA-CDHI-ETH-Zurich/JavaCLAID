#pragma once
#include "jbind11/jbind11.hpp"
#include "JbindWrapperGeneratorBase.hpp"
namespace java = jbind11;


namespace claid
{
    namespace JavaWrapper
    {
        class JavaModule;
        class ChannelWrapper;

        class JavaWrapperBase
        {
            protected:
                std::shared_ptr<JbindWrapperGeneratorBase> wrapperGenerator;
                virtual void forwardReflector(const char* memberFieldName, const std::string& reflectorName, void* reflectorPtr, jobject javaObject, jobject defaultValue) = 0;

            public:
                virtual void generateWrapper(java::JavaPackage& package) = 0;

                // Why data type not specified here? Well, it's the wrapper already
                // JavaWrapper<T> already knows the data type. The crucial part is looking up the correct wrapper in Python::Module.
                virtual ChannelWrapper subscribe(JavaModule* module, std::string channelID, std::string callbackFunctionName) = 0;
                virtual ChannelWrapper publish(JavaModule* module, std::string channelID) = 0;
                
                virtual void post(std::shared_ptr<void> channelReference, jobject data) = 0;
                virtual void postWithTimestamp(std::shared_ptr<void> channelReference, jobject data, int64_t timestamp) = 0;

                void overrideJbindWrapperGenerator(std::shared_ptr<JbindWrapperGeneratorBase> newWrapperGenerator)
                {
                    this->wrapperGenerator = newWrapperGenerator;
                }

                template<typename T>
                void overrideJbindWrapperGenerator(T* newWrapperGenerator)
                {
                    this->wrapperGenerator = std::static_pointer_cast<JbindWrapperGeneratorBase>(
                        std::shared_ptr<T> (newWrapperGenerator));
                }

                virtual const std::string& getFullyQualifiedCppClassName() const = 0;
                virtual const std::string& getCanonicalJavaClassName() const = 0;


                template<typename Reflector>
                void forwardReflectorToNativeObject(const char* memberFieldName, Reflector& reflector, jobject javaObject, jobject defaultValue)
                {
                    std::string reflectorName = reflector.getReflectorName();
                    void* untypedReflector = static_cast<void*>(&reflector);
                    this->forwardReflector(memberFieldName, reflectorName, untypedReflector, javaObject, defaultValue);
                }
                
                
        };
    }
}