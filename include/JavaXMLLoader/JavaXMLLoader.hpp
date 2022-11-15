#pragma once


#include "RunTime/RunTime.hpp"
#include "XML/XMLDeserializer.hpp"
#include "JavaWrapper/JavaModuleFactory.hpp"
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
					
					JavaVM* jvm = JNIUtils::getJavaVM();

					if(jvm == nullptr)
					{
						CLAID_THROW(claid::Exception, "Error, failed to load JavaModule from XML file. Unable to get JavaVM. Was JNIUtils::onLoad() called in JNI_OnLoad?");
					}

					std::string className;
					if (!node->getAttribute("class", className))
					{
						CLAID_THROW(claid::Exception, "Error, failed to load JavaModule when parsing XML configuration. The \"class\" attribute is missing for the <JavaModule> tag."
						"Make sure to specify JavaModule as follows:\n"
						"<JavaModule class=\"com.example.MyModule\">\n"
						"\t...\n"
						"</JavaModule>");
						return nullptr;
					}   
						
					className = Signatures::Class::classNameToSignature(className);

					JavaModuleFactory factory(className, jvm);
					JavaModule* javaModule = factory.getInstance();
					
					XMLDeserializer deserializer(node);
					deserializer.deserializeFromNode("JavaModule", *javaModule);
					std::string id;
					if (node->getAttribute("id", id))
					{
						javaModule->setID(id);
					}

					return javaModule;
				}


		};
	}
}

