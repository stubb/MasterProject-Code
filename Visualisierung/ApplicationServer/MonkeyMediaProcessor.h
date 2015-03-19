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

using namespace cv;
using namespace tinyxml2;

class MonkeyMediaProcessor
{
	private:
		int NUMBER_OF_RENDERING_CLIENTS;
		Mat *decoded_image;
		std::vector<Mat> *processed_images;

	public:
		MonkeyMediaProcessor()
		{
			NUMBER_OF_RENDERING_CLIENTS = 1;
			decoded_image = new Mat();
			processed_images = new vector<Mat>();
		}
		MonkeyMediaProcessor(int num_rc)
		{
			NUMBER_OF_RENDERING_CLIENTS = num_rc;
			decoded_image = new Mat();
			processed_images = new vector<Mat>();
		}

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
			// Decode the Image Data.
			size_t output = 0;
			unsigned char *picture_raw = base64_decode(data, strlen(data), &output);
			
			// OpenCV imdecode needs a vector instead of a unsigned char* array,
			// so we do a quick conversion.
			cv::vector<unsigned char> picture_raw_vector(picture_raw, picture_raw + output);

			imdecode(picture_raw_vector, 1, decoded_image);

			#if DEBUG
			std::fstream imgout("./test.png", std::fstream::out | std::fstream::binary);
			imgout.write(reinterpret_cast<char*>(picture_raw), output);
			imgout.close();
			
			imwrite( "./testMat.jpg", *decoded_image );
			#endif

			if (decoded_image->total() <= 0)
				std::cout << "Kacka Sir!" << std::endl;
		}
		
		void split_image()
		{
			processed_images->clear();
			for (int i = 0; i < NUMBER_OF_RENDERING_CLIENTS; ++i)
			{
				Rect rect(decoded_image->cols / NUMBER_OF_RENDERING_CLIENTS * i, 0, decoded_image->cols / NUMBER_OF_RENDERING_CLIENTS,decoded_image->rows);
				Mat temp_image = Mat(*decoded_image, rect);
				processed_images->push_back(temp_image);
			}
		}
		
		void reshape_images()
		{
			for (int i = 0; i < processed_images->size(); ++i)
			{
				processed_images->at(i).reshape(0, 1);
			}
		}

		void process_monkey_data(char *xml_string, int with_reshape)
		{
			XMLDocument monkey_document;
			monkey_document.Parse(xml_string);
			if (monkey_document.Error()) {
				std::cout << "XML Parse() ErrorID: " << monkey_document.ErrorID() << std::endl;
			}
			else {
				std::string mime = monkey_document.FirstChildElement("package")->FirstChildElement("type")->GetText();
				if (mime.compare("picture") == 0)
					process_picture(monkey_document.FirstChildElement("package")->FirstChildElement("data")->GetText());
				split_image();
				if (with_reshape)
					reshape_images();

			}
		}
};

#endif