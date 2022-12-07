#pragma once
#include "jbind11/jbind11.hpp"
#include "JbindWrapperGeneratorBase.hpp"
namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        class JavaModule;

        class JavaWrapperBase
        {
            protected:
                std::shared_ptr<JbindWrapperGeneratorBase> wrapperGenerator;

            public:
                virtual void generateWrapper(java::JavaPackage& package) = 0;

                // Why data type not specified here? Well, it's the wrapper already
                // JavaWrapper<T> already knows the data type. The crucial part is looking up the correct wrapper in Python::Module.
                virtual jobject subscribe(JavaModule* module, jstring channelID, jstring callbackFunctionName) = 0;
                virtual jobject publish(JavaModule* module, jstring channelID) = 0;
                
                virtual void post(std::shared_ptr<void> channelReference, jobject data) = 0;
               
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
                
        };
    }
}