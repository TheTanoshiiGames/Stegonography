/****************************************
Author: Noah Judge
Summary: Allows the user to encode a
message into a PNG image and then
decode it to get the message back in the
form of a text file. This is all done
through a command prompt.
Date Created: Nov. 17th, 2019
Last Edited: Nov. 22nd, 2019
****************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <winsock.h>
#include "LodePNG.h"

using namespace std;
using namespace lodepng;

int main(int argc, char **argv) {
	if (strcmp(argv[1], "-e") == 0) {
		//Encoder Variables
		string originalFile = argv[2];
		string newFileName  = argv[3];
		string lineOne;
		string lineTwo;
		fstream txtStream;
		char* message;
		unsigned width;
		unsigned height;
		vector<unsigned char> image;
		vector<unsigned char> newImage;
		int msgBits[8];
		int imgBits[8];
		const unsigned char EMPTY = 0;
		unsigned char imageChar;
		unsigned char messageChar;
		
		//Gets the image data
		unsigned errorDec = decode(image, width, height, originalFile);

		//Sends out an error if there was one
		if (errorDec) {
			cerr << "Decoder error: " << errorDec << ": " << lodepng_error_text(errorDec) << endl;
			return -1;
		}
		cout << image[1];
		//Fill txtFile in preperation for bit manipulation
		char* txtInput = new char[width * height * 3];
		if (argc == 5) {
			int fileLength = 0;
			txtStream.open(argv[4]);
			if (!txtStream.is_open()) {
				cerr << "Error occured while trying to open text file!" << endl;
				return -2;
			}
			while (getline(txtStream, lineOne)) {
				lineTwo += (lineOne + '\n');
			}
			lineTwo += '\0';
			message = (char*)lineTwo.c_str();
		}
		else {
			cout << "Please input text to place in PNG: " << endl;
			cin >> txtInput;
			message = txtInput;
		}

		//Set newImage vector for new picture
		int counter = -1;
		for (unsigned int i = 0; i < strlen(message); i++) {
			messageChar = (unsigned char)message[i];
			if (messageChar != '\0') {
				//Put the bits of the message character into an array
				for (int j = 0; j < 8; j++) {
					msgBits[j] = (messageChar >> j) & 1;
				}
				for (int j = 0; j < 8; j++) {
					//Put the bits of the image character into an array and add message bit
					++counter;
					imageChar = (unsigned char)image.at(counter);
					for (int k = 0; k < 7; k++) {
						imgBits[k] = (imageChar >> (7 - k)) & 1;
					}
					imgBits[7] = msgBits[j];
					//Set unsigned char in the new image
					for (int k = 0; k < 8; k++) {
						newImage.push_back(EMPTY);
						newImage.at(counter) |= (imgBits[k] >> (7 - k));
					}
				}
			}
		}
		//Fill In the rest of the picture
		for (unsigned int i = (counter + 1); i < image.size(); i++) {
			newImage.push_back(EMPTY);
			newImage.at(i) = image.at(i);
		}
		//Create new image
		unsigned errorEnc = encode(newFileName, newImage, width, height);

		//Send out error if there is one
		if (errorEnc) {
			cerr << "Encoder error: " << errorEnc << ": " << lodepng_error_text(errorEnc) << endl;
			return -4;
		}

		//Close text file
		txtStream.close();
		if (txtStream.is_open()) {
			cerr << "Error occured while trying to close text file!" << endl;
			return -5;
		}
	}
	else if (strcmp(argv[1], "-d") == 0) {
		//Decoder Variables
		string modifiedFile = argv[2];
		string txtFile;
		string text;
		vector<unsigned char> image;
		ofstream message;
		unsigned width;
		unsigned height;
		int bits[8];
		const unsigned char EMPTY = 0;
		unsigned char messageChar;
		unsigned char imageChar;

		//Gets the image data
		unsigned errorDec = decode(image, width, height, modifiedFile);

		//Sends out an error if there was one
		if (errorDec) {
			cerr << "Decoder error: " << errorDec << ": " << lodepng_error_text(errorDec) << endl;
			return -1;
		}

		if (argc == 4) {
			txtFile = argv[3];
			message.open(txtFile);
			if (!message.is_open()) {
				cerr << "Error occured while trying to open text file!" << endl;
				return -2;
			}
		}

		//Decode image
		int counter = -1;
		for (unsigned int i = 0; i < image.size(); i++) {
			for (int j = 0; j < 8; j++) {
			++counter;
			imageChar = (unsigned char)image.at(counter);
			bits[7 - j] = imageChar & 1;
			}
			messageChar = EMPTY;
			for (int j = 0; j < 8; j++) {
				messageChar |= (bits[j] << (7 - j));
			}
			if (messageChar != '\0') {
				if (!message.is_open()) {
					text += messageChar;
				}
				else {
					message << messageChar;
				}
			}
			else {
				break;
			}
		}

		if (message.is_open()) {
			//Close text file
			message.close();
			if (message.is_open()) {
				cerr << "Error occured while trying to close text file!" << endl;
				return -5;
			}
		}
		else {
			cout << text;
		}
	}
	else {
	cerr << "Usage: steg.exe -e <original file name> <modified file name> [text file name]\n"
		<< "OR\n"
		<< "steg.exe -d <modified file name> [text file name]\n";
	return -6;
	}
}
