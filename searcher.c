#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/*
    COSC431 Assignment 1: Parser
    Author: Max Edwards
    Date Created: 30/4/2020
    Last Date Modified: 2/5/2020
*/

//struct made for use during quicksort
struct searchResult{
    int posting;
    float frequency;
};

//searches dictionary for a word and returns its index
//if it is not in the dictionary it returns -1
int searchDictionary(char** dictionary, char* word, int size) {
    int i, result, min, max;
    if(size == 0) {
        return -1;
    }
    i = size/2;
    min = 0;
    max = size-1;
    while((result = strcmp(dictionary[i], word)) != 0) {
        if(result<0) {
            min = i+1;
        } else {
            max = i-1;
        }
        if(max<min) {
            return -1;
        }
        i = (max+min)/2;
    }
    return i;
}

//merges 2 posting lists together and adds frequencies when required
int merge(int* arr1, int* arr2, float* freq1, float* freq2, int size1, int size2, int* newPostings, float* newFrequencies) {
    int length = 0;
    int i=0;
    int j=0;
    while(i!=size1 && j != size2) {
        if(arr1[i] == arr2[j]) {
            newPostings[length] = arr1[i];
            newFrequencies[length] = freq1[i] + freq2[j];
            i++;
            j++;
        } else if(arr1[i] < arr2[j]) {
            newPostings[length] = arr1[i];
            newFrequencies[length] = freq1[i];
            i++;
        } else {
            newPostings[length] = arr2[j];
            newFrequencies[length] = freq2[j];
            j++;
        }
        length++;
    }
    while(i<size1) {
        newPostings[length] = arr1[i];
        newFrequencies[length] = freq1[i];
        length++;
        i++;
    }
    while(j<size2) {
        newPostings[length] = arr2[j];
        newFrequencies[length] = freq2[j];
        length++;
        j++;
    }
    return length;
}

//Used during quicksort for comparing structs
int compare(const void *a, const void *b) {
    struct searchResult* s1 = (struct searchResult*) a;
    struct searchResult* s2 = (struct searchResult*) b;
    if(s1->frequency > s2->frequency) {
        return -1;
    } else if(s1->frequency < s2->frequency) {
        return 1;
    } else {
        return 0;
    }
}

//Runs the search engine
int main(int argc, char *argv[]) {
    //getting index_size values
    char currentWord[100];
    char path[255];
    strcpy(path, argv[1]);
    strcpy(&path[strlen(argv[1])], "/index_size");
    FILE *fp = fopen(path, "r");
    fgets(currentWord, 100, fp);
    int index_size = atoi(currentWord);
    fgets(currentWord, 100, fp);
    int docCount = atoi(currentWord);
    fclose(fp);
    

    //allocating memory for index
    char** dictionary = (char**) malloc(sizeof(char **) * index_size);
    float** frequencies = (float**) malloc(sizeof(float *) * index_size);
    int** postings = (int**) malloc(sizeof(int *) * index_size);
    int* lengths = (int*) malloc(sizeof(int) * index_size);

    //reading from index and storing it
    strcpy(&path[strlen(argv[1])], "/index");
    fp = fopen(path, "r");
    int index = 0;
    int postingIndex = 0;
    int letterIndex = 0;
    int phase = 0;
    int c;
    for(;(c=fgetc(fp)) != EOF; letterIndex++) {
        //finished reading either word or postingLength
        if(c == ':') {
            currentWord[letterIndex] = '\0';
            //finished reading word
            if(phase == 0) {
                dictionary[index] = (char *) malloc((letterIndex+1)*sizeof(char));
                strcpy(dictionary[index], currentWord);
                phase = 1;
            //finished reading postingLength
            } else {
                int postingLength = atoi(currentWord);
                postings[index] = (int*) malloc(postingLength*sizeof(int));
                frequencies[index] = (float*) malloc(postingLength*sizeof(float));
                lengths[index] = postingLength;
                phase = 0;
            }
            letterIndex = -1;
        //sees end of docID
        } else if(c == ',') {
            currentWord[letterIndex] = '\0';
            int docID = atoi(currentWord);
            postings[index][postingIndex] = docID;
            letterIndex = -1;
        //sees end of frequency
        } else if(c == ' ') {
            currentWord[letterIndex] = '\0';
            float frequency = atof(currentWord);
            frequencies[index][postingIndex] = frequency;
            letterIndex = -1;
            postingIndex++;
        //sees end of postings list
        } else if(c == '\n') {
            letterIndex = -1;
            postingIndex = 0;
            index++;
        } else {
            //append character to current word
            currentWord[letterIndex] = (char) c;
        }
    }
    fclose(fp);
    
    //reading from IDlist and storing it
    strcpy(&path[strlen(argv[1])], "/IDlist");
    fp = fopen(path, "r");
    char** docIDList = (char**) malloc(index_size*sizeof(char*));
    letterIndex = 0;
    index = 0;
    for(;(c=fgetc(fp)) != EOF; letterIndex++) {
        if(c == '\n') {
            currentWord[letterIndex] = '\0';
            docIDList[index] = (char*) malloc((strlen(currentWord)+1)*sizeof(char));
            strcpy(docIDList[index], currentWord);
            index++;
            letterIndex = -1;
        } else {
            currentWord[letterIndex] = (char) c;
        }
    }
    fclose(fp);

    //generic variable for loops
    int i;
    //waiting for input to search
    letterIndex = 0;
    int* currentPostings;
    float* currentFrequencies;
    //length of current postings list
    int length = 0;
    //flag for running words at end of file
    int lastWord = 0;
    //words in query
    int wordCount = 0;
    
    for(;(c=getchar()) != EOF; letterIndex++) {
        c = (char) c;
        //label to go back to when EOF is reached and words havent been processed
        Adding_Word:
        //Reached end of a word
        if(c == ' ' || c == '\n' || c == '\r') {
            //checks word isnt empty
            if(letterIndex!=0) {
                wordCount++;
                currentWord[letterIndex] = '\0';
                index = searchDictionary(dictionary, currentWord, index_size);
                //this means it exists in the dictionary
                if(index != -1) {
                    //If first word in query, just grab postings lists
                    if(length==0) {
                        currentPostings = postings[index];
                        currentFrequencies = frequencies[index];
                        length = lengths[index];
                    //need to perform a merge if 2nd word in query or later
                    } else {
                        int* newPostings = (int*) malloc((lengths[index] + length) * sizeof(int));
                        float* newFrequencies = (float*) malloc((lengths[index] + length)*sizeof(float));
                        
                        length = merge(currentPostings, postings[index], currentFrequencies, frequencies[index], length, lengths[index], newPostings, newFrequencies);
                        //in this case, this is at least our 2nd merge so previously malloc'd memory needs ot be freed
                        if(wordCount>=3) {
                            free(currentPostings);
                            free(currentFrequencies);
                        }
                        currentPostings = newPostings;
                        currentFrequencies = newFrequencies;
                    }
                //word is not in dictionary so lwoer wordCount since we ignore it
                } else {
                    wordCount--;
                }
                //resetting currentWord index to start
                letterIndex = -1;
            } else {
                letterIndex = -1;
            }
        }
        //Reached end of query
        if(c == '\n' || c == '\r') {
            letterIndex = -1;
            //sorting our results
            struct searchResult* results = (struct searchResult*) malloc(sizeof(struct searchResult) * length);
            for(i=0;i<length;i++) {
                results[i].posting = currentPostings[i];
                results[i].frequency = currentFrequencies[i];
            }
            qsort(results, length, sizeof(struct searchResult), compare);
            
            //printing out our results
            for(i=0;i<length;i++) {
                printf("%s %.2f\n", docIDList[results[i].posting], results[i].frequency);
            }
            free(results);

            //deallocating memory only if we had more than 1 word in query 
            //since this results in allocating memory for use during merge function
            if(wordCount > 1) {
                free(currentPostings);
                free(currentFrequencies);
            }
            //resetting variables
            wordCount = 0;
            length = 0;
        //appendign char to word
        } else if(c !=' ') {
            currentWord[letterIndex] = (char) c;
        }
        //if lastWord == 1 we had reached end of file already
        if(lastWord == 1) {
            break;
        }
    }
    //sets flag to run thru loop one last time with word that was just before the End of File
    if(lastWord == 0) {
        lastWord = 1;
        //setting c to newline to appear like normal end of query instead of EOF
        c = '\n';
        goto Adding_Word;
    }

    //deallocating memory
    for(i=0;i<index_size;i++) {
        free(postings[i]);
        free(frequencies[i]);
        free(dictionary[i]);
    }
    for(i=0;i<docCount;i++) {
        free(docIDList[i]);
    }
    free(docIDList);
    free(postings);
    free(frequencies);
    free(dictionary);
    free(lengths);
    return 0;
}