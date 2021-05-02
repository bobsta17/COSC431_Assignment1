/*
    COSC431 Assignment 1: Parser
    Author: Max Edwards
    Date Created: 18/4/2020
    Last Date Modified: 2/5/2020
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
int main() {
    char currentWord[100];
    int c;
    int letterIndex = 0;
    int isTag = 0;
    
    for(;(c=getchar()) != EOF; letterIndex++) {
        //if word is longer than currentWord limit, reset it.
        if(letterIndex == (sizeof(currentWord)/sizeof(currentWord[0]))) {
            letterIndex = -1;
            continue;
        }
        c = tolower(c);
        //Checks if we are in the middle of a tag
        if(isTag) {
            currentWord[letterIndex] = (char) c;
            if(c == '>') {
                currentWord[letterIndex] = '\0';
                //checks we are at end of a document
                if(strcmp("/doc", currentWord) == 0) {
                    printf("\n");
                }
                isTag = 0;
                letterIndex = -1;
            }
            continue;
        }
        //Checks if we start looking at a tag
        if(c == '<') {
            currentWord[letterIndex] = '\0';
            //checks current word isnt empty
            if(letterIndex != 0) {
                //if it ends in a fullstop ignore the fullstop
                if(currentWord[letterIndex-1] == '.') {
                    currentWord[letterIndex-1] = '\0';
                    //if words was only a fullstop word is now empty so dont print it
                    if(letterIndex != 1) {
                        printf("%s ", currentWord);
                    }
                } else {
                    printf("%s ", currentWord);
                }
            }
            isTag=1;
            letterIndex=-1;
        //If any of these characters show up treat as end of word and reset word
        } else if(c == ' ' || c== '\n' || c == '"' || c == ',' || c == '?' || c == '_' ||
                  c == '[' || c == ']' || c == '{' || c == '}' || c == ':' || c == ';' ||
                  c == '?' || c == '>' || c == '(' || c == ')' || c == '&' || c == '!' ||
                  c == '$' || c == '\'' || c == '#' || c == '*' || c == '$' || c == '%' ||
                  c == '+' || c == '-' || c == '/' || c == '`') {
            currentWord[letterIndex] = '\0';
            //checks word isnt empty
            if(letterIndex != 0) {
                //if word ends in fullstop ignore fullstop
                if(currentWord[letterIndex-1] == '.' || currentWord[letterIndex-1] == '-') {
                    letterIndex--;
                    while(letterIndex>=0 && (currentWord[letterIndex] == '.' || currentWord[letterIndex] == '-')) {
                        currentWord[letterIndex] = '\0';
                        letterIndex--;
                    }
                    //checks if word is empty after removing fullstop
                    if(letterIndex >= 0) {
                        printf("%s ", currentWord);
                    }
                } else {
                    printf("%s ", currentWord);
                }
            }
            letterIndex = -1;
        //appends character to word
        } else {
            currentWord[letterIndex] = (char) c;
        }
    }
    return 0;
}