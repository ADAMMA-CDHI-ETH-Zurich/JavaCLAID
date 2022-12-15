#include "JavaWrapper/JavaWrapperMaster.hpp"

namespace claid
{
    namespace JavaWrapper
    {
   

        bool JavaWrapperMaster::isWrapperRegisteredForNativeClass(const std::string& cppClassName)
        {
            std::string name_replaced = cppClassName; 
            claid::stringReplaceAll(name_replaced, "::", "_");
            auto it = this->registeredWrappers.find(name_replaced);
            return it != this->registeredWrappers.end();
        }

        JavaWrapperBase* JavaWrapperMaster::getWrapperForNativeClass(std::string className)
        {
            auto it = this->registeredWrappers.find(className);

            if(it == this->registeredWrappers.end())
            {
                return nullptr;
            }

            return it->second;
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