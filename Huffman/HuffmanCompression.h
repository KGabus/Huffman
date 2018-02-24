#pragma once
#include <string>;

using namespace std;

struct HuffmanNode 
{
	unsigned int weight = 0;
	char symbol;
	HuffmanNode *LeftChild = nullptr;
	HuffmanNode *RightChild = nullptr;
};

class HuffmanCompression
{
public:
	string treeBuildingString = "";

	HuffmanCompression();
	void createPathArray();
	bool createHuffmanTreeFromFile(string fileName);											//returns true if input file opens properly
	bool createEncodedFile(ofstream &outputFile, string inputFileName, string outputFileName);	//returns true if both files open properly
	bool createDecodedFile(ofstream &outputFile, string inputFileName, string outputFileName);	//returns true if both files open properly
	bool createHuffmanTree(ofstream &outputFile, string inputFileName, string outputFileName);	//returns true if both files open properly

private:
	bool treeInfoWritten = false;
	string pathArray[256];
	HuffmanNode *treeRoot;
	HuffmanNode *nodePointerArray[256];
	
	bool generateCounts(string fileName);																	//returns true if input file opens properly
	bool generateTreeFromFile(string fileName);																//returns true if input file opens properly
	void generatePathArray(HuffmanNode *node, string path);
	bool generateAndWriteEncodedFile(ofstream &outputFile, string inputFileName, string outputFileName);	//returns true if both files open properly
	bool generateAndWriteDecodedFile(ofstream &outputFile, string inputFileName, string outputFileName);	//returns true if both files open properly
	bool countArrayToTree(ofstream &outputFile, string fileName);											//returns true if both files open properly
};

