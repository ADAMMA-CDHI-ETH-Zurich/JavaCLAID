#include "JavaWrapper/WrapperMaster.hpp"

namespace claid
{
    namespace JavaWrapper
    {
        bool WrapperMaster::isWrapperAssignedToJavaClass(const std::string& javaClassName)
        {
            auto it = javaCppClassNamesMap.find(javaClassName);
            return it != javaCppClassNamesMap.end();
        }

        bool WrapperMaster::isWrapperRegisteredForNativeClass(const std::string& cppClassName)
        {
            auto it = registeredWrappers.find(cppClassName);
            return it != registeredWrappers.end();
        }

        WrapperBase* WrapperMaster::getWrapperForJavaClass(std::string javaClassName)
        {
            auto it = javaCppClassNamesMap.find(javaClassName);

            if(it == javaCppClassNamesMap.end())
            {
                return nullptr;
            }

            auto it2 = registeredWrappers.find(it->second);

            if(it2 == registeredWrappers.end())
            {
                return nullptr;
            }

            return it2->second;
        }
    }
}