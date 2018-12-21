#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include <bitset>

using namespace std;

int main(int argc, char *argv[])
{
	std::cout << "N+ 16-bit ABGR 4444 Texture Converter\n\n\n";

	//DDS 64x64 ARGB 4444 Pre-Created HEADER (128 Bytes)
	int dds64[32] =
	{ 0x20534444, 0x0000007C, 0x0000100F, 0x00000040, 0x00000040, 0x00000080, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000041,
	0x00000000, 0x00000010, 0x00000F00, 0x000000F0, 0x0000000F, 0x0000F000, 0x00001000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 };

	//DDS 512x512 ARGB 4444 Pre-Created HEADER (128 Bytes)
	int dds512[32] =
	{ 0x20534444, 0x0000007C, 0x0000100F, 0x00000200, 0x00000200, 0x00000400, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000041,
	0x00000000, 0x00000010, 0x00000F00, 0x000000F0, 0x0000000F, 0x0000F000, 0x00001000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 };

	//Argument Counter
	if (argc<2)
		cout << "Drag and drop files to the exe!" << endl;
	else
	{
		for (int i = 1; i<argc; i++)
		{
			std::string fn(argv[i]);											//Filename in String
			cout << "\nFilename: " << fn << endl;

			ifstream fi(fn.c_str(), ifstream::binary);							//File Input-Stream

			//Calculate the length of file
			streampos begin, end;												
			begin = fi.tellg();													//Returns the current position 
			fi.seekg(0, ios::end);												//Sets the position
			end = fi.tellg();													//Returns the current position 
			fi.seekg(0);														//Sets the position
			long fs = (end - begin);


			//RAW TO DDS CONVERTION

			if (fn.substr(fn.find_last_of(".") + 1) == "raw")					//If File has an .raw Datatype
			{
				int lix = fn.find_last_of(".");
				string fno = fn.substr(0, lix) + ".dds";

				cout << "Filesize: " << fs << " bytes --> " << fs + 128 << " bytes\n" << endl; //Filesize before and after injected Header

					if ((fs == 8192 || fs == 524288))							//Filesize Check (Two Types of Raw Files convertable)
					{

						char* buffer = new char[fs];							//Create Buffer

						ofstream fo(fno.c_str(), ios::out | ios::binary);			//Create File Output-Stream
						fo.seekp(0x0, ios_base::beg);								//Set Pointer at the begin of fo (Fileoutput)
						fi.read(buffer, fs);										//Write Input-Stream into Buffer

						int pixel = fs;
						cout << pixel << endl << endl;
						
						for (int i3 = 0; i3<pixel; i3++)							//Buffer Bitshifting 16-bit ABGR 4444 to ARGB 4444
						{
							uint8_t tempintB = (uint8_t)buffer[i3];
							uint8_t tempintR = (uint8_t)buffer[i3 + 1];
							int tempswitch;
							int tempswitch2;

							for (int i2 = 0; i2<4; i2++)
							{
								tempswitch = tempintB & (1 << i2);
								tempswitch2 = tempintR & (1 << i2);

								//COPY B TO R
								if (tempswitch & (1 << i2))
								{
									tempintR |= 1 << i2;
								}
								else
								{
									tempintR &= ~(1 << i2);
								}

								//COPY R TO B
								if (tempswitch2 & (1 << i2))
								{
									tempintB |= 1 << i2;
								}
								else
								{
									tempintB &= ~(1 << i2);
								}
								//G not needed
							}

							buffer[i3] = tempintB;
							buffer[i3 + 1] = tempintR;

							i3 = i3 + 1;

						}
						
						fo.seekp(0x0, ios_base::beg);									//Set Pointer Back to begin (because DDS Header Injection)
						for (int i = 0; i<32; i++)										//Inject DDS Header!
						{
							if (fs == 8192)
								fo.write((char*)&dds64[i], sizeof(dds64[i]));
							if (fs == 524288)
								fo.write((char*)&dds512[i], sizeof(dds512[i]));
						}

						fo.write(buffer, fs);											//Write Buffer back to Output Stream

						delete[] buffer;												//Clearup and Close Stream
						fo.close();

						cout << "Conversion Successfull!\n" << endl;
					}

					else
					{
						cout << "ERROR: Filesize does not corresponding to n+ Texture files!\nSkipping...\n" << endl;
					}

				}


			//DDS TO RAW CONVERSION (Back to Headerless 16-bit ABGR 4444)

			else
			if (fn.substr(fn.find_last_of(".") + 1) == "dds")									//If File has an .raw Datatype
			{
				int lix = fn.find_last_of(".");
				string fno2 = fn.substr(0, lix) + ".raw";

				cout << "Filesize: " << fs << " bytes --> " << fs - 128 << " bytes\n" << endl;	//Filesize before and after injected Header

				ifstream fi2(fn.c_str(), ifstream::binary);						//File Input-Stream
				ofstream fo2(fno2.c_str(), ios::out | ios::binary);				//Create File Output-Stream

				char* buffer = new char[fs - 128];								//Create Buffer

				fi2.seekg(128, ios::beg);										//Set Pointer after DDS Header (128 Bytes)
				fi2.read(buffer, fs);											//Write Input-Stream into Buffer

				int pixel = fs - 128;											//Filesize - Header for Bitshifting
				cout << pixel << endl << endl;
				
				for (int i3 = 0; i3<pixel; i3++)								//Buffer Bitshifting 16-bit ARGB 4444 to ABGR 4444
				{
					uint8_t tempintB = (uint8_t)buffer[i3];
					uint8_t tempintR = (uint8_t)buffer[i3 + 1];
					int tempswitch;
					int tempswitch2;

					for (int i2 = 0; i2<4; i2++)
					{
						tempswitch = tempintB & (1 << i2);
						tempswitch2 = tempintR & (1 << i2);

						//COPY R TO B
						if (tempswitch & (1 << i2))
						{
							tempintR |= 1 << i2;
						}
						else
						{
							tempintR &= ~(1 << i2);
						}

						//COPY B TO R
						if (tempswitch2 & (1 << i2))
						{
							tempintB |= 1 << i2;
						}
						else
						{
							tempintB &= ~(1 << i2);
						}

					}

					buffer[i3] = tempintB;
					buffer[i3 + 1] = tempintR;

					i3 = i3 + 1;

				}
				
				cout << endl;
				fo2.write(buffer, fs - 128);										//Write Buffer to Stream

				delete[] buffer;													//Cleanup and Close Stream
				fo2.close();

				cout << "Conversion Successfull!\n" << endl;

			}

			else
			{
				cout << "\nERROR: File format not recognized!\nSkipping...\n" << endl;
			}

			fi.close();

			}

			cout << "All Done!" << endl;
			//return 0;
		}

		cin.get();
		return 0;

	}


