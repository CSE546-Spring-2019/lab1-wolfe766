//Author: Sam Wolfe
//CSE 5461 - Ogle 9:35am T/Th
//Project 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define true 1
#define false 0
    
const short BUFFER_SIZE = 1;
const short MAX_SEARCH_STRING_SIZE = 20;

int potentialMatchFound = false;
int searchIndex;
int fileSize = 0;

int completeMatchFound(int searchIndex, char *searchString);
int togglePhaseTwo(FILE *inputFile, int index);

int main(int argc, char *argv[]){
    unsigned char buffer[BUFFER_SIZE];
        
    char *inputPath = argv[1];
    char *searchString = argv[2];
    char *outputPath = argv[3];

    long storedMatches[MAX_SEARCH_STRING_SIZE]; //will never exceed 100 bytes with 20 byte search string
    int storedMatchIndex = 0;
    int returnIndex = -1;
    int totalMatches = 0;

    char firstByte;
    int itemsRead;
    FILE *outputFile;
    FILE *inputFile;

    if(argc != 4){
        printf("Error: Unsupported number of arguments.");
    }
    if((inputFile = fopen(inputPath,"rb")) == NULL){
        printf("Error opening input file");
    }; 

    if((outputFile = fopen(outputPath,"w")) == NULL){
        printf("Error opening output file");
    }; 

    //File is open, update firstByte to be the first byte in our search string 
    firstByte = searchString[0]; 
    searchIndex = 0;

    //read the whole file to determine size since performance isn't an issue
    //(Difficult to do in main loop because we shift the stream position indicator frequently)
    while(fread(buffer, 1, 1, inputFile) > 0){
        fileSize++;
    }

    fclose(inputFile);

    if((inputFile = fopen(inputPath,"rb")) == NULL){
        printf("Error opening input file");
    }; 

    //Read until EOF
    while(true){
        //Read next bytes and retrieve number of bytes read
        itemsRead = fread(buffer, BUFFER_SIZE, 1, inputFile);

        //If we got less than 1 items, we're done reading
        if(itemsRead < 1){break;}

        //PHASE 1: locate new occurrence of search string
        for(int i = 0; i < itemsRead; i++){
            if (buffer[i] == firstByte){
                if(potentialMatchFound == false){
                    potentialMatchFound = true; 
                }else{
                    //Get the current file position with ftell, use it to calculate position of the matching byte
                    //we go back to this later to check if it is a complete match
                    storedMatches[storedMatchIndex] = ftell(inputFile) - (BUFFER_SIZE - i);
                    storedMatchIndex++;
                }
            }
            if(potentialMatchFound == true){
                if(strlen(searchString) == 1){
                    //Single character means instant match
                    totalMatches++;
                    returnIndex = togglePhaseTwo(inputFile, i);
                    break;
                }else if(buffer[i] == searchString[searchIndex]){
                    //next byte matches, continue
                    searchIndex++;
                    if(completeMatchFound(searchIndex, searchString)){
                        totalMatches++;
                        returnIndex = togglePhaseTwo(inputFile, i);
                        break;
                    }
                }else{
                    //No match, end the search.
                    potentialMatchFound = false;
                    searchIndex = 0;
                    storedMatchIndex = 0;
                }
            }
        }

        //PHASE 2: check child matches
        //If returnIndex is set, then we stored some children last run
        if(returnIndex != -1){
            unsigned char childBuffer[strlen(searchString)];

            for(int i = 0; i < storedMatchIndex; i++){
                //For each stored location, seek to that point in the file
                fseek(inputFile, storedMatches[i], SEEK_SET);
                //Then read out the next strlen(search string) of chars
                fread(childBuffer, strlen(searchString), 1, inputFile);

                //Since we read only this # of chars, we can compare the strings directly
                if(strcmp(childBuffer, searchString) == 0){
                    totalMatches++;
                }
            }
            //Return back to where we were before checking child elements.
            fseek(inputFile, returnIndex, SEEK_SET);
            storedMatchIndex = 0;
            returnIndex = -1;
        }
    }

    printf("File Size: %i\n", fileSize);
    printf("Total Matches: %i\n",totalMatches);
    fclose(inputFile);
    fprintf(outputFile, "Size of file is: %d\nNumber of matches = %d\n",fileSize,totalMatches);
    fclose(outputFile);
}

//Returns the last checked index by the main loop
int togglePhaseTwo(FILE *inputFile, int index){
    potentialMatchFound = false;
    searchIndex = 0;
    return ftell(inputFile) - (BUFFER_SIZE - index);
}

int completeMatchFound(int searchIndex, char *searchString){
    if(searchIndex == strlen(searchString)){
        return true;
    }else{
        return false;
    }
}
