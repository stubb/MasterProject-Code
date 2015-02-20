#ifndef __MONKEY_MEDIA_PROCESSOR_H__
#define __MONKEY_MEDIA_PROCESSOR_H__

#include <vector>
#include <string>

/*	XML Includes.	*/
#include "tinyxml2.h"

using namespace tinyxml2;

class MonkeyMediaProcessor
{
	private:
		int NUMBER_OF_RENDERING_CLIENTS;
		//std::vector<> v;

	public:
		MonkeyMediaProcessor()
		{
			NUMBER_OF_RENDERING_CLIENTS = 1;
		}
		MonkeyMediaProcessor(int num_rc)
		{
			NUMBER_OF_RENDERING_CLIENTS = num_rc;
		}

		int get_number_of_rendering_client() const
		{
			return NUMBER_OF_RENDERING_CLIENTS;
		}
		void process_picture(XMLDocument monkey_doc)
		{

		}

		void process_monkey_data(char *xml_string)
		{
			XMLDocument monkey_document;
			monkey_document.Parse(xml_string);

			std::string mime = monkey_document.FirstChildElement("package")->FirstChildElement("type")->GetText();
			if (mime.compare("picture"))
			{
					//process_picture(monkey_document);
			}

			/*int image_dimensions[2] = {0};
			document.FirstChildElement( "package" )->FirstChildElement( "width" )->QueryIntText( &image_dimensions[0] );
			document.FirstChildElement( "package" )->FirstChildElement( "height" )->QueryIntText( &image_dimensions[1] );
			std::cout << image_dimensions[0] << " " << image_dimensions[1] << std::endl;*/
		}
};

#endif