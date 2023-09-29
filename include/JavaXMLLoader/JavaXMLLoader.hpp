/***************************************************************************
* Copyright (C) 2023 ETH Zurich
* Core AI & Digital Biomarker, Acoustic and Inflammatory Biomarkers (ADAMMA)
* Centre for Digital Health Interventions (c4dhi.org)
* 
* Authors: Patrick Langer
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*         http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***************************************************************************/

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
						
					Logger::printfln("Trying to load Module %s", className.c_str());
					JavaModuleFactory factory(className);

					JavaModule* javaModule = factory.getInstance();

					if(javaModule == nullptr)
					{
						CLAID_THROW(claid::Exception, "Error, failed to load JavaModule when parsing XML configuration.\n"
						<< "Module of class \"" << className << "\" could not be instantiated.");
					}

					XMLDeserializer deserializer(node);
					deserializer.deserializeExistingPolymorphicObject("claid::JavaWrapper::JavaModule", javaModule);
					std::string id;
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

