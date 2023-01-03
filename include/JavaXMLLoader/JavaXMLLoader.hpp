#pragma once


#include "RunTime/RunTime.hpp"
#include "XML/XMLDeserializer.hpp"
#include "JavaWrapper/JavaModuleFactory.hpp"
#include "JavaWrapper/Signatures.hpp"

namespace claid
{
	namespace JavaWrapper
	{
		class JavaXMLLoader : public XMLLoader::XMLLoaderBase
		{
			DECLARE_XML_LOADER(JavaXMLLoader)

			public:
				JavaXMLLoader() : XMLLoader::XMLLoaderBase("JavaModule")
				{

				}

				claid::Module* instantiateModuleFromNode(std::shared_ptr<XMLNode> node) 
				{
					// node already has tag <JavaModule>
					


					std::string className;
					if (!node->getAttribute("class", className))
					{
						CLAID_THROW(claid::Exception, "Error, failed to load JavaModule when parsing XML configuration. The \"class\" attribute is missing for the <JavaModule> tag."
						"Make sure to specify JavaModule as follows:\n"
						"<JavaModule class=\"com.example.MyModule\">\n"
						"\t...\n"
						"</JavaModule>");
					}   
						
					className = Signatures::Class::classNameToSignature(className);
					Logger::printfln("Trying to load Module %s", className.c_str());
					JavaModuleFactory factory(className);
					printf("Module 0!\n");

					JavaModule* javaModule = factory.getInstance();
					printf("Module instantiated!\n");

					if(javaModule == nullptr)
					{
						CLAID_THROW(claid::Exception, "Error, failed to load JavaModule when parsing XML configuration.\n"
						<< "Module of class \"" << className << "\" could not be instantiated.");
					}

			// 		XMLDeserializer deserializer(node);
			// 		deserializer.deserializeFromNode("JavaModule", *javaModule);
			// 		std::string id;
			// 		if (node->getAttribute("id", id))
			// 		{
			// 			javaModule->setID(id);
			// 		}
			// 		printf("Module instantiated 3!\n");
            // std::cout << std::flush;

					// Only return valid javaModules!
					// Do NOT return nullptr!
					return javaModule;
				}


		};
	}
}

