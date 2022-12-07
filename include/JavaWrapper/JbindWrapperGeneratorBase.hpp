#pragma once
#include "jbind11/jbind11.hpp"

namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        class JbindWrapperGeneratorBase // No need to inherit from Deserializer or Serializer? Probably not, as we only care about static members.
        {
            public:
                virtual void generate(java::JavaPackage& package, const std::string& className) = 0;
        };
    }
}