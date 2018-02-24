// Huffman.cpp : Defines the entry point for the console application.
// Author	   : Kaylee Gabus
// Date		   : 2-28-17

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#include "HuffmanCompression.h"

using namespace std;
string command;
string file1 = "";
string file2 = ""; 
string file3 = "";

HuffmanCompression Huff = HuffmanCompression();

void outputHelp()
{
	cout << "This command takes multiple command line arguments. Arguments specidied as [file] are optional output files and will be created if not specified." << endl;
	cout << "Input and output files cannot match." << endl;
	cout << "Command" << "\t" << "Arguments" << "\t\t" << "Description" << endl;
	cout << "-t" << "\t" << "file1 [file2]" << "\t\t" << "Creates a Huffman tree from file1" << endl;
	cout << "-e" << "\t" << "file1 [file2]" << "\t\t" << "Encodes file1" << endl;
	cout << "-et" << "\t" << "file1 file2 [file3]" << "\t" << "Encodes file1 using tree in file2" << endl;
	cout << "-d" << "\t" << "file1 file2" << "\t\t" << "Decodes file1 ans writes to file2" << endl;
	cout << "-h | -? | -help\t\tOutputs this help menu" << endl;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		outputHelp();
		return -1;
	}

	try	
	{

		clock_t startTime = clock();						//Start timer

		//check for no file commands
		if (strcmp(argv[1], "-h") == 0) { outputHelp(); return 0; }
		else if (strcmp(argv[1], "-?") == 0) { outputHelp(); return 0; }
		else if (strcmp(argv[1], "-help") == 0) { outputHelp(); return 0; }


		if (argc == 1)
		{	//if the user doesn't enter any arguments, display help
			outputHelp();
			return 0;
		}

		//adjust arguments
		for (int counter = 0; counter < command.length(); counter++)//convert the command to lower case for easier comparisons
			command[counter] = tolower(command[counter]);
		
		file1 = argv[2];									//at least one file was entered because we got passed the prev if

		if (argc >= 4)
		{//if a second file was provided
			file2 = argv[3];
			if (file1 == file2)
			{	//verify the entered files are not the same
				cout << "ERROR: The entered file names cannot match." << endl;
				outputHelp();
				return 0;
			}
			if (argc == 5)
			{	//if a third file was entered
				file3 = argv[4];
				if (file1 == file3 || file2 == file3)
				{	//check the third file isn't the same as one of the first two
					cout << "ERROR: The entered file names cannot match." << endl;
					outputHelp();
					return 0;
				}
			}
		}
										

		if (strcmp(argv[1], "-t") == 0)
		{	//command line code for creating an encoding tree
			if (argc > 4)
			{	//if more than two files are entered
				cout << "ERROR: Command not recognized." << endl;
				outputHelp();
				return 0;
			}
			
			if (file2 == "")
			{	//only one file was entered, generate tree output file
				if (file1.find_last_of(".") != string::npos)						//if file1 has an extension, remove it and replace
					file2 = file1.substr(0, file1.find_last_of(".")) + ".htree";
				else file2 = file1 + ".htree";										//else just append
			}
			
			ofstream outfile(file2, ios::binary | ios::trunc);

			if (Huff.createHuffmanTree(outfile, file1, file2))
				cout << "Tree Created at " << file2 << endl;
			else cout << "ERROR: One or more of the entered files could not be properly accessed." << endl;
		}	//end -t
		else if (strcmp(argv[1], "-e") == 0)	
		{	//command to create tree and encode file
			if (argc > 4)
			{	//more than two files were entered
				cout << "ERROR: Command not recognized." << endl;
				outputHelp();
				return 0;
			}
			if (file2 == "")	
			{	//an output file was not specified
				if (file1.find_last_of(".") != string::npos)						//if file1 has an extension, remove it and replace
					file2 = file1.substr(0, file1.find_last_of(".")) + ".huf";
				else file2 = file1 + ".huf";										//else just append
			}

			ofstream outputFile(file2, ios::binary | ios::trunc);

			if (Huff.createHuffmanTree(outputFile, file1, file2))
			{
				Huff.createPathArray();
				if (Huff.createEncodedFile(outputFile, file1, file2))
					cout << "Encoded File Sucessfully Created At " << file2 << endl;
				else cout << "ERROR: The output file could not be properly accessed." << endl;
			}
			else cout << "ERROR: The input file could not be properly accessed." << endl;	
		}	//end -e
		else if (strcmp(argv[1], "-et") == 0)	
		{	//encode a file from an existing tree
			if (argc > 5)
			{	//more than three files were entered
				cout << "ERROR: Command not recognized." << endl;
				outputHelp();
				return 0;
			}
			else if (argc < 4)
			{	//verify the at least 2 files were entered
				cout << "ERROR: Not enough files." << endl;
				outputHelp();
				return 0;
			}
			
			if (file3 == "")
			{	//an output file was not specified
				if (file1.find_last_of(".") != string::npos)						//if file1 has an extension, remove it and replace
					file3 = file1.substr(0, file1.find_last_of(".")) + ".huf";
				else file3 = file1 + ".huf";										//else just append
			}

			ofstream outputFile(file3, ios::binary | ios::trunc);


			if (Huff.createHuffmanTreeFromFile(file2))
			{
				Huff.createPathArray();
				if (Huff.createEncodedFile(outputFile, file1, file3))
					cout << "Encoded File Sucessfully Created At " << file3 << endl;
				else cout << "ERROR: One or more files could not be properly accessed." << endl;
			}
			
			else cout << "ERROR: The tree file could not be properly accessed." << endl;

		}	//end -et
		else if (strcmp(argv[1], "-d") == 0)
		{	//argument to decode an encoded file
			if (argc == 4)
			{	//two files were entered
				ifstream inputFile(file1, ios::binary);
				ofstream outputFile(file2, ios::binary, ios::trunc);

				if (Huff.createDecodedFile(outputFile, argv[2], argv[3]))
					cout << "Decoded File Created At " << file2 << endl;
				else cout << "There was an error reading one or more of the entered files." << endl;
			}
			else 
			{
				cout << "ERROR: Incorrect number of files" << endl;
				outputHelp();
				return 0;
			}
		}	//end -d
		else if (strcmp(argv[1], "-h") == 0) {outputHelp(); return 0;}
		else if (strcmp(argv[1], "-?") == 0) { outputHelp(); return 0; }
		else if (strcmp(argv[1], "-help") == 0) { outputHelp(); return 0; }
		else
		{
			cout << "Command not reconized." << endl;
			outputHelp();
			return 0;
		}
		
		cout << "Elapsed Time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "s";					//if this line is hit, a sucessful operation was performed
	}
	catch (const std::exception&)
	{
		cout << "The program has thrown an excpetion";
	}

    return 0;
}

