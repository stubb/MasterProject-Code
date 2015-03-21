#ifndef __MONKEY_MEDIA_PROCESSOR_H__
#define __MONKEY_MEDIA_PROCESSOR_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>

#include <cstdint>
#include <cstdlib>

/*	OpenCV Includes.	*/
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

/*	XML Includes.	*/
#include "tinyxml2.h"

/*	Rendering Client Include	*/
#include "Rendering_Client.h"

using namespace cv;
using namespace tinyxml2;
using namespace std;

class MonkeyMediaProcessor
{
	private:
		//	====================================================
		//					PRIVATE ATTRIBUTES
		//	====================================================
		/*	Vector with all connected Rendering Clients.	*/
		vector<Rendering_Client*> *rendering_clients;
		
		/*	Image Data.	*/
		Mat *decoded_image;
		vector<Mat*> *processed_images;
		int last_image_height;
		int last_image_width;
		
		/*	BASE64.	*/
		char encoding_table[64] = {	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
									'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
									'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
									'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
									'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
									'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
									'w', 'x', 'y', 'z', '0', '1', '2', '3',
									'4', '5', '6', '7', '8', '9', '+', '/'};
		char *decoding_table = NULL;
		int mod_table[3] = {0, 2, 1};
		
		//	====================================================
		//					PRIVATE METHODS
		//	====================================================
		int process_picture(const char *data)
		{
			// Decode the Image Data.
			size_t output = 0;
			unsigned char *picture_raw = base64_decode(data, strlen(data), &output);
			
			// OpenCV imdecode needs a vector instead of a unsigned char* array,
			// so we do a quick conversion.
			vector<unsigned char> picture_raw_vector(picture_raw, picture_raw + output);

			imdecode(picture_raw_vector, 1, decoded_image);

			#if DEBUG
				fstream imgout("./test.png", std::fstream::out | std::fstream::binary);
				imgout.write(reinterpret_cast<char*>(picture_raw), output);
				imgout.close();
				
				imwrite( "./testMat.jpg", *decoded_image );
			#endif

			if (decoded_image->total() <= 0)
				cout << "Kacka Sir!" << endl;
			
			return 1;
		}
		
		void split_image()
		{
			processed_images->clear();
			for (unsigned int i = 0; i < rendering_clients->size() /*&& rendering_clients->size() > 1*/; ++i)
			{
				Rect rect(decoded_image->cols / rendering_clients->size() * i, 0, decoded_image->cols / rendering_clients->size(), decoded_image->rows);
				Mat *temp_image = new Mat(*decoded_image, rect);
				processed_images->push_back(temp_image);
			}
		}
		
		void reshape_images()
		{
			for (unsigned int i = 0; i < processed_images->size(); ++i)
			{
				processed_images->at(i)->reshape(0, 1);
			}
		}

	public:
		//	====================================================
		//					CONSTRUCTORS
		//	====================================================
		MonkeyMediaProcessor()
		{
			decoded_image = new Mat();
			processed_images = new vector<Mat*>();
			rendering_clients = new vector<Rendering_Client*>();
		}
		MonkeyMediaProcessor(vector<Rendering_Client*> *rcs)
		{
			decoded_image = new Mat();
			processed_images = new vector<Mat*>();
			rendering_clients = rcs;
		}

		//	====================================================
		//					BASE64 METHODS
		//	====================================================
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

		//	====================================================
		//						GETTER
		//	====================================================

		//	====================================================
		//					PUBLIC METHODS
		//	====================================================
		void process_monkey_data(char *xml_string, unsigned int xml_string_size, int with_reshape)
		{
			XMLDocument monkey_document;
			monkey_document.Parse(xml_string, xml_string_size);
			if (monkey_document.Error()) {
				cout << "XML Parse() ErrorID: " << monkey_document.ErrorID() << endl;
			}
			else
			{
				if (processed_images->size() > 0)
				{
					last_image_width = processed_images->at(0)->cols;
					last_image_height = processed_images->at(0)->rows;
				}
				int rc = 0;
				string mime = monkey_document.FirstChildElement("package")->FirstChildElement("type")->GetText();
				if (mime.compare("picture") == 0)
					rc = process_picture(monkey_document.FirstChildElement("package")->FirstChildElement("data")->GetText());
				if (rc)
				{
					split_image();
				}
			}
		}
		
		void send_to_renderers()
		{
			// Different Resolution -> Send new Metadata with Current Values.
			if (last_image_width != processed_images->at(0)->cols &&
				last_image_height != processed_images->at(0)->rows)
			{
				int meta_data[] = {1337, 1337, 1337, 1337, processed_images->at(0)->cols, processed_images->at(0)->rows};
				for (unsigned int i = 0; i < rendering_clients->size(); ++i)
					SDLNet_TCP_Send(rendering_clients->at(i)->get_socket(), (void*)meta_data, sizeof(meta_data));
			}
			for (unsigned int i = 0; i < rendering_clients->size(); ++i)
			{
				#if DEBUG
				cout << "Will send " << processed_images->at(i)->total() * processed_images->at(i)->channels() << " Bytes per RC." << endl;
				#endif
				SDLNet_TCP_Send(rendering_clients->at(i)->get_socket(), (void*)processed_images->at(i)->data, processed_images->at(i)->total() * processed_images->at(i)->channels());
			}
		}
};

#endif