// Author	: Kaylee Gabus
// Date		: 2-28-17
// HuffmanCompression	: Uses a Huffman tree to encode or decode a file, depending on the functions called.


#pragma once
#include "stdafx.h";
#include <string>;
#include <iostream>;
#include <fstream>;
#include "HuffmanCompression.h";

using namespace std;

#pragma region Public Functions

HuffmanCompression::HuffmanCompression()
{
	treeRoot = nullptr;

	for (int counter = 0; counter <= 255; counter++)				//fill the node pointer array
	{
		nodePointerArray[counter] = new HuffmanNode();
		nodePointerArray[counter]->symbol = (unsigned char)counter;
	}	
}

void HuffmanCompression::createPathArray()
{
	return generatePathArray(treeRoot, "");
}

bool HuffmanCompression::createHuffmanTreeFromFile(string fileName)
{
	if (generateTreeFromFile(fileName))
		return true;
	return false;
}

bool HuffmanCompression::createEncodedFile(ofstream &outputFile, string inputFileName, string outputFileName)
{
	if (generateAndWriteEncodedFile(outputFile, inputFileName, outputFileName))
		return true;
	else return false;
}

bool HuffmanCompression::createDecodedFile(ofstream & outputFile, string inputFileName, string outputFileName)
{
	if (generateTreeFromFile(inputFileName))
		if (generateAndWriteDecodedFile(outputFile, inputFileName, outputFileName))
			return true;
	else return false;
}

bool HuffmanCompression::createHuffmanTree(ofstream &outputFile, string inputFileName, string outputFileName)
{
	if (generateCounts(inputFileName))
		if (countArrayToTree(outputFile, outputFileName))
			return true;
	return  false;
}

#pragma endregion 

#pragma region PrivateFunctions

bool HuffmanCompression::generateCounts(string fileName)
{	//returns true if file is able to be read from 
	char currentChar;
	ifstream inputFile(fileName, ios::binary);

	if (!inputFile.is_open())
		return false;

	while (inputFile.get(currentChar))
	{//while there are character left in the file, increment the count in the character's corresponding node
		nodePointerArray[(int)currentChar & 0xFF]->weight++;
	}
	return true;
}

bool HuffmanCompression::generateTreeFromFile(string fileName)
{	//returns true if file is able to be read from
	char firstChar;
	char secondChar;
	HuffmanNode *leftChildNode;
	HuffmanNode *rightChildNode;
	HuffmanNode *subroot;
	HuffmanNode *pointerArray[256] = {};

	ifstream inputFile(fileName, ios::binary);

	if (inputFile.is_open())
	{
		for (int counter = 0; counter < 255; counter++)
		{	//tree building strings are always 510 bytes for ASCII encoding and we are reading 2 bytes in a single pass
			inputFile.get(firstChar);
			treeBuildingString += (firstChar);
			if (pointerArray[(((int)firstChar) & 0xFF)] == nullptr)
			{	//if the spot in the array is null, create a node with the proper symbol
				leftChildNode = new HuffmanNode();
				leftChildNode->symbol = (((int)firstChar) & 0xFF);
			}
			else leftChildNode = pointerArray[(((int)firstChar) & 0xFF)];
			
			inputFile.get(secondChar);
			treeBuildingString += (secondChar);
			if (pointerArray[(((int)secondChar) & 0xFF)] == nullptr)
			{	//if the spot in the array is null, create a node with the proper symbol
				rightChildNode = new HuffmanNode();
				rightChildNode->symbol = (((int)secondChar) & 0xFF);
			}
			else rightChildNode = pointerArray[(((int)secondChar) & 0xFF)];

			subroot = new HuffmanNode();										//create a node to act as a root for the subtree
			subroot->LeftChild = leftChildNode;
			subroot->RightChild = rightChildNode;

			pointerArray[(((int)firstChar) & 0xFF)] = subroot;					//place the subtree into the peoper spot in the array
			pointerArray[(((int)secondChar) & 0xFF)] = nullptr;
		}
		
		treeRoot = pointerArray[(((int)firstChar) & 0xFF)];

		return true;
	}
	else return false;
}

void HuffmanCompression::generatePathArray(HuffmanNode *node, string path)
{	//traverses the tree to find the encoding paths for each character
	string nodePath = path;
	if (node->LeftChild != nullptr)
	{	//if following a left path, concat a 0
		generatePathArray(node->LeftChild, nodePath + "0");
	}

	if (node->LeftChild == nullptr && node->RightChild == nullptr)
	{	//if the node is a leaf, store the path in the array
		pathArray[(int)node->symbol & 0xFF] = nodePath;
	}
	
	if (node->RightChild != nullptr)
	{	//if following a right path, concat a 1
		generatePathArray(node->RightChild, nodePath + "1");
	}
}

bool HuffmanCompression::generateAndWriteEncodedFile(ofstream &outputFile, string inputFileName, string outputFileName)
{	//returns true if file writes sucessfully
	ifstream inputFile(inputFileName, ios::binary);
	string pathToChar = "";
	string paddingPath = "";

	if (!treeInfoWritten)
	{
		outputFile << treeBuildingString;									//write the tree building string to the file
		treeInfoWritten = true;
	}



		char currentChar;
		unsigned char outputChar;
		string buffer = "";
		unsigned char pow2[] = { 128, 64, 32, 16, 8, 4, 2, 1 };

		//outputFile << treeBuildingString;									//write the tree building string to the file

		while (inputFile.get(currentChar))
		{//while there are characters left in the input file
			pathToChar = pathArray[(int)currentChar & 0xFF];
			if (paddingPath == "" && pathToChar.length() > 8)				//finds a path long enough to use for padding purposes
				paddingPath = pathToChar;
			buffer += pathToChar;
			while (buffer.length() >= 8)
			{	//if there are at least 8 characters in the buffer
				outputChar = 0;
				for (int counter = 0; counter <= 7; counter++)
				{
					if (buffer[counter] == '1') 
						outputChar |= pow2[counter];
				}

				outputFile << outputChar;								//output the character

				buffer = buffer.substr(8, buffer.length() - 1);			//remove the part that was just written
			}
		}
		//at this point, the end of the file has been reached
		while (buffer.length() > 0)
		{	//if there are bits left to write but nothing left to read
			outputChar = 0;
			if (buffer.length() < 8)
			{	//add bits that would on their own result in reaching an internal node
				buffer += paddingPath.substr(0, 8 - buffer.length());
			}

			for (int counter = 0; counter <= 7; counter++)
			{
				if (buffer[counter] == '1')
					outputChar |= pow2[counter];
			}

			outputFile << outputChar;									//output the final chars
			buffer = buffer.substr(8, buffer.length() - 1);
		}
		return true;
}

bool HuffmanCompression::generateAndWriteDecodedFile(ofstream & outputFile, string inputFileName, string outputFileName)
{	//to be called after a tree has been constructed
	//returns true if files are able to be read from and write to, else returnes false
	ifstream inputFile(inputFileName, ios::binary);
	char inputChar;
	unsigned char currentChar;
	HuffmanNode *node = treeRoot;
	unsigned char pow2[] = { 128, 64, 32, 16, 8, 4, 2, 1 };

	inputFile.seekg(510);												//set the reading location to just after the tree

	if (inputFile.is_open() && outputFile.is_open())
	{
		while (inputFile.get(inputChar))
		{	//while there are still characters in the encoded file
			currentChar = (int)(inputChar & 0xFF);			//convert the read char to an unsigned char
			for (int counter = 0; counter <= 7; counter++)
			{	//for each bit in the character
				if ((currentChar & pow2[counter]) == pow2[counter])		//if the current bit is on	
					node = node->RightChild;
				else node = node->LeftChild;

				if (node->LeftChild == nullptr && node->RightChild == nullptr)
				{	//if you've hit a leaf, output the character and return to the root
					outputFile << node->symbol;
					node = treeRoot;
				}
			}	
		}
		return true;
	}
	else return false;
}

bool HuffmanCompression::countArrayToTree(ofstream &outputFile, string fileName)
{
	int smallestIndex = -1;
	int secondSmallestIndex = -1;
	HuffmanNode* subroot;

	if (outputFile.is_open() != true)					//if the output file cannot be written to, return false
		return false;

	outputFile.trunc;								//remove everything in the file

	for (int iterationCounter = 254; iterationCounter >= 0; iterationCounter--)
	{	//for every character in the extended ASCII table

		unsigned int smallestFreq = UINT_MAX;			//reset the reference freqs
		unsigned int secondSmallestFreq = UINT_MAX;

		for (int counter = 0; counter <= 255; counter++)
		{	//for every index in the array
			if (nodePointerArray[counter] != nullptr)
			{	//if the corresponding spot in the node pointer array isn't null
				if (nodePointerArray[counter]->weight <= smallestFreq)
				{	//move the smallest info into secondSmallest references
					secondSmallestFreq = smallestFreq;
					secondSmallestIndex = smallestIndex;

					smallestFreq = nodePointerArray[counter]->weight;
					smallestIndex = counter;
				}
				else if (nodePointerArray[counter]->weight <= secondSmallestFreq)
				{	//save current info in secondSmallest references
					secondSmallestFreq = nodePointerArray[counter]->weight;
					secondSmallestIndex = counter;
				}

				if (smallestFreq == 0 && secondSmallestFreq == 0)
				{	//there are a lot of unused characters and this prevents a bunch of unnecessary compares
					break;
				}
			}
		}

		subroot = new HuffmanNode;											//create new node to act as a root 
		subroot->weight = smallestFreq + secondSmallestFreq;
		
		if (smallestIndex < secondSmallestIndex)							//determine the smaller index
		{	//add the node with the smaller index as the left child
			subroot->LeftChild = nodePointerArray[smallestIndex];
			subroot->RightChild = nodePointerArray[secondSmallestIndex];
			nodePointerArray[smallestIndex] = subroot;
			nodePointerArray[secondSmallestIndex] = nullptr;
			
			treeBuildingString += (char)smallestIndex;						//add characters to the tree building string
			treeBuildingString += (char)secondSmallestIndex;
		}
		else
		{	//add the node with the larger index as the left child
			subroot->LeftChild = nodePointerArray[secondSmallestIndex];
			subroot->RightChild = nodePointerArray[smallestIndex];
			nodePointerArray[secondSmallestIndex] = subroot;
			nodePointerArray[smallestIndex] = nullptr;

			treeBuildingString += (char)secondSmallestIndex;				//add characters to the tree building string
			treeBuildingString += (char)smallestIndex;
		}
	}

	treeRoot = subroot;														//not really a subroot if it's the only root for the tree
	
	if (!treeInfoWritten)
	{
		outputFile << treeBuildingString;									//write the tree building string to the file
		treeInfoWritten = true;
	}
	
	return true;
}	

#pragma endregion
