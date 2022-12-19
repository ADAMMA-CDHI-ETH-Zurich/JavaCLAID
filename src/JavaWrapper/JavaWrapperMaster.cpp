#include "JavaWrapper/JavaWrapperMaster.hpp"

namespace claid
{
    namespace JavaWrapper
    {
   
        bool JavaWrapperMaster::isWrapperRegisteredForNativeClass(const std::string& cppClassName)
        {
            auto it = this->registeredWrappers.find(cppClassName);
            return it != this->registeredWrappers.end();
        }

        bool JavaWrapperMaster::isWrapperRegisteredForJavaClass(const std::string& canonicalClassName)
        {
            for(auto& entry : this->registeredWrappers)
            {
                if(entry.second->getCanonicalJavaClassName() == canonicalClassName)
                {
                    return true;
                }
            }
            return false;
        }

        JavaWrapperBase* JavaWrapperMaster::getWrapperForNativeClass(const std::string& className)
        {
            auto it = this->registeredWrappers.find(className);

            if(it == this->registeredWrappers.end())
            {
                return nullptr;
            }

            return it->second;
        }


        JavaWrapperBase* JavaWrapperMaster::getWrapperForJavaClass(const std::string& canonicalClassName)
        {
            for(auto& entry : this->registeredWrappers)
            {
                if(entry.second->getCanonicalJavaClassName() == canonicalClassName)
                {
                    return entry.second;
                }
            }
            return nullptr;
        }

        void JavaWrapperMaster::getRegisteredWrapperClasses(std::vector<std::string>& wrappers)
        {
            wrappers.clear();
            for(auto it : registeredWrappers)
            {
                wrappers.push_back(it.first);
            }
        }
    }
}