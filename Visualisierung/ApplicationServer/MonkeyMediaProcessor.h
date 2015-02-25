#ifndef __MONKEY_MEDIA_PROCESSOR_H__
#define __MONKEY_MEDIA_PROCESSOR_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <cstdint>
#include <cstdlib>

/*	OpenCV Includes.	*/
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

/*	XML Includes.	*/
#include "tinyxml2.h"

using namespace cv;
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

		






		static inline bool is_base64(unsigned char c) {
			return (isalnum(c) || (c == '+') || (c == '/'));
		}
		char *base64decode( char *p )
		{
			static const std::string b64_charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			std::string ret;
			char * str = p;
			int in_len,
				i = 0,
				j = 0,
				in_ = 0;
			unsigned char block_4[4], block_3[3];

			in_len = strlen(str);

			while( in_len-- && ( str[in_] != '=') && is_base64(str[in_]) ){
				block_4[i++] = str[in_];
				in_++;
				if( i == 4 ){
					for( i = 0; i < 4; i++ ){
						block_4[i] = b64_charset.find(block_4[i]);
					}
					block_3[0] = (block_4[0] << 2) + ((block_4[1] & 0x30) >> 4);
					block_3[1] = ((block_4[1] & 0xf) << 4) + ((block_4[2] & 0x3c) >> 2);
					block_3[2] = ((block_4[2] & 0x3) << 6) + block_4[3];

					for( i = 0; (i < 3); i++ ){
						ret += block_3[i];
					}
					i = 0;
				}
			}

			if(i){
				for( j = i; j <4; j++ ){
					block_4[j] = 0;
				}

				for( j = 0; j < 4; j++ ){
					block_4[j] = b64_charset.find(block_4[j]);
				}

				block_3[0] = (block_4[0] << 2) + ((block_4[1] & 0x30) >> 4);
				block_3[1] = ((block_4[1] & 0xf) << 4) + ((block_4[2] & 0x3c) >> 2);
				block_3[2] = ((block_4[2] & 0x3) << 6) + block_4[3];

				for( j = 0; (j < i - 1); j++ ){
					ret += block_3[j];
				}
			}

			return (char*) ret.c_str();
		}












char encoding_table[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
char *decoding_table = NULL;
int mod_table[3] = {0, 2, 1};


unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();
    if (input_length % 4 != 0) return NULL;
    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = (unsigned char *)malloc(*output_length);
    if (decoded_data == NULL) return NULL;
    for (unsigned int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}


void build_decoding_table() {

    decoding_table = (char *)malloc(256);

    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}


void base64_cleanup() {
    free(decoding_table);
}












		int get_number_of_rendering_client() const
		{
			return NUMBER_OF_RENDERING_CLIENTS;
		}

		void process_picture(const char *data)
		{
			size_t output = 0;
			//std::cout << "Daten: " << data << std::endl << std::endl;
			string temp_data = string(data);
			std::cout << temp_data << std::endl << std::endl << strlen(data) << std::endl;
			unsigned char *picture_raw = base64_decode(data, strlen(data), &output);
			

			std::fstream imgout("./test.png", std::fstream::out | std::fstream::binary);
			imgout.write(reinterpret_cast<char*>(picture_raw), output);
			imgout.close();

			Mat img = imdecode(*picture_raw, CV_LOAD_IMAGE_COLOR);
			if (img.total() > 0)
				img = img.reshape(0, 1);
			else
				std::cout << "Kacka Sir!" << std::endl;
			//std::cout << "Hier sind die Daten: " << std::endl << img.data << std::endl << "Daten Ende!" << std::endl;
		}

		void process_monkey_data(char *xml_string)
		{
			XMLDocument monkey_document;
			monkey_document.Parse(xml_string);
process_picture(monkey_document.FirstChildElement("package")->FirstChildElement("data")->GetText()) ;/*
			std::string mime = monkey_document.FirstChildElement("package")->FirstChildElement("type")->GetText();
			if (mime.compare("picture"))
			{
				
			}*/

			/*int image_dimensions[2] = {0};
			document.FirstChildElement( "package" )->FirstChildElement( "width" )->QueryIntText( &image_dimensions[0] );
			document.FirstChildElement( "package" )->FirstChildElement( "height" )->QueryIntText( &image_dimensions[1] );
			std::cout << image_dimensions[0] << " " << image_dimensions[1] << std::endl;*/
		}
};

#endif