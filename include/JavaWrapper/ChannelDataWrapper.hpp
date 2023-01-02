#pragma once

#include "jbind11/jbind11.hpp"
namespace java = jbind11;

namespace claid
{
    namespace JavaWrapper
    {
        // Not really a wrapper, merely mimics the functionality of ChannelDataWrapper.
        class ChannelDataWrapper
        {
            private:
                jobject data;

            public:
                ChannelDataWrapper()
                {

                }

                ChannelDataWrapper(jobject data) : data(data)
                {

                }

                static void addToJbindPackage(java::JavaPackage& p)
                {
                    java::JavaClass<ChannelDataWrapper> cls(p, "ChannelData", java::GenericClass());

                    cls.def("value", &ChannelDataWrapper::value, java::GenericReturn());
                }

                jobject value()
                {
                    return data;
                }
        };
    }
}