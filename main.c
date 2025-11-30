/*
Program 3: This project is inspired by the New York Times Spelling Bee game. The user enters a word or the computer chooses
a word through a random mode but the word will have unique letters and then they also enter a required letter. 
These unique letters will be added to a hive and the user will enter words only made up of the letters in the hive,
and it also MUST contain the required letter. This version has a twist where the computer also solves the game
by coming up with a list of words, if they are pangrams or perfect pangrams the users score will go up, and a their score
along with many other factors such as if its bingo, the amount of valid words, amount of pangrams and more will be displayed.
Course: CS 211, Fall 2025, UIC
Author: Manasvi Sharma
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

const int MIN_HIVE_SIZE = 2;
const int MAX_HIVE_SIZE = 12;
const int MIN_WORD_LENGTH = 4;

typedef struct WordList_struct {
    char** words;
    int numWords;
    int capacity;
} WordList;

WordList* createWordList() {
    WordList* newList = malloc(sizeof(WordList));
    newList->capacity = 4;
    newList->numWords = 0;
    newList->words = malloc(newList->capacity * sizeof(char*));

    return newList;
}

/*
The purpose of this function is to add the given word to the end of the word list. This function takes in
the parameters of a pointer list from struct worldList and a character pointer of the new word.
Since this is a void function it doesn't return anything. It just updates the wordList by adding the
new word to the end.
*/
void appendWord(WordList* thisWordList, char* newWord) {
    //This resizes the wordlist if the amount of words is equivalent to the capacity.
    if(thisWordList->numWords == thisWordList->capacity){
        thisWordList->capacity*=2;
        char** newWords = malloc(thisWordList->capacity * sizeof(char*));
        for(int i = 0; i < thisWordList->numWords; i++){
            newWords[i] = thisWordList->words[i];
        }
        free(thisWordList->words);
        thisWordList->words = newWords;
    }

    char* newStringWord = malloc(strlen(newWord) + 1);
    strcpy(newStringWord, newWord);
    thisWordList->words[thisWordList->numWords] = newStringWord;
    thisWordList->numWords++;
}

/*
This function is for the open ended section and it checks if the given word is in the list.
The parameters it recieves the list of words from the WordList struct, an existing word string.
The return type is bool so  by utilizing strcmp string.h function it goes through each word in the list
provided and compares it to existingWord and if it is equal to 0 then it will return true. Otherwise false.
*/
bool wordCheck(WordList* thisWordList, char* existingWord){
    for(int i = 0; i < thisWordList->numWords; i++){
        if(strcmp(thisWordList->words[i], existingWord) == 0){
            return true;
        }
    }
    return false;
}

/*
This function reads the word from the dictionary file then adds it to the dictionary list if it has the
correct amount of minimum length. This function takes in the parameters of a character pointer file name, 
pointer dictionary list from struct wordList, and an integer minimum length. The function's return type
is an integer so it returns the length of the longest word, or -1 if there is an error.
*/
int buildDictionary(char* filename, WordList* dictionaryList, int minLength) {
    FILE* myfile = fopen(filename, "r");
    if(myfile == NULL){
        return -1;
    } 
    char aWord[100];
    int longestLength = 0;
    while(!feof(myfile)){
        fscanf(myfile, "%s", aWord);
        if(strlen(aWord) >= minLength){
            appendWord(dictionaryList, aWord);
            if(strlen(aWord) > longestLength){
                longestLength = strlen(aWord);
            }
        }
    }

    fclose(myfile);
    return longestLength;
}

/*
This function frees the memory used by the list provided in the parameter, and it frees both the words character
pointer arrays, the double pointer character array, so the actual string as well as the arry of string pointers.
The parameters for this function is the word list pointer from the WordList struct. 
This is a void function so it doesn't return anything but updates the word list by freeing everything in it.
*/
void freeWordList(WordList* list) {
    for(int i = 0; i < list->numWords; i++){
        if(list->words[i] != NULL){
            free(list->words[i]);
        }
    }
    if(list->words != NULL){
        free(list->words);
    }

    free(list);
}

/*
This function returns the index of the first instance of the character that is in the string
It takes in the parameters of a character pointer string and a character. If the character is found
it returns the index of the first instance of the character. If the character is not found in the string 
-1 is returned. 
*/
int findLetter(char* str, char aLet) {
    for(int i = 0; i < strlen(str); i++){
        if(str[i] == aLet){
            return i;
        }
    }
    return -1;
}

/*
This function adds each string to the hive in alphabetical order and makes sure to not add a string twice.
It takes in the parameters of a character pointer string and character pointer hive.
This is a void function so it doesn't return anything, instead it updates the hive parameter.
*/
void buildHive(char* str, char* hive) {
    hive[0] = '\0';
    int i = 0;
    while(str[i] != '\0'){
        if(findLetter(hive, str[i]) == -1){
            int hiveIndex = strlen(hive);
            for(int j = 0; j < strlen(hive); j++){
                if(str[i] < hive[j]){
                    hiveIndex = j;
                    break;
                }
            }
            for(int k = strlen(hive); k >= hiveIndex; k--){
                hive[k+1] = hive[k];
            }
            hive[hiveIndex] = str[i];
        }
        i++;
    }
}

/*
This function counts all the unique letters in a string. 
It takes in the parameters of character pointers, essentially a string and counts all the unique 
letters in it through a for loop that compares each letter to every letter in the string.
This function returns integer and it returns the integer count of unique letters in a string.
*/
int countUniqueLetters(char* str) {
    int uniqueCount = 0;
    for(int i = 0; i < strlen(str); i++){
        bool notUnique = false;
        for(int j = 0; j < i; j++){
            if(str[i] == str[j]){
                notUnique = true;
                break;
            }
        }

        if(notUnique == false){
            uniqueCount++;
        }
    }
    return uniqueCount;
}

/*
This function goes through the dictionary and creates a word list of all the words that fit based on the hive size.
The parameters are a dictionaryList from the Wordlist and this is a pointer, and integer hiveSize.
This return type is WorldList* for this function and it returns the updated Word list that was made from the WordList struct.
*/
WordList* findAllFitWords(WordList* dictionaryList, int hiveSize) {
    WordList* fitWords = createWordList();
    int indexCount = 0;
    for(int i = 0; i < dictionaryList->numWords; i++){
        if(countUniqueLetters(dictionaryList->words[i]) == hiveSize){
            appendWord(fitWords, dictionaryList->words[i]);
        }
    }
    return fitWords;
}

/*
This function checks the validity for a word making sure it is made up of only letters that are in the hive 
and has to have used the required letter. The parameters for this function is the character pointer word so a string word,
character pointer hive so the string hive, and the required character letter. This function returns of type bool and if the character 
pointer word string meets the requirements, it returns true. If it doesn't it returns false.
*/
bool isValidWord(char* word, char* hive, char reqLet) {
    for(int i = 0; i < strlen(word); i++){
        if(findLetter(hive, word[i]) == -1){
            return false;
        }
    }
    if(findLetter(word, reqLet) == -1){
        return false;
    }
    return true;
}

/*
This function is to check if the given string is a pangram or not, which happens when the string uses all the letter
in the hive at least once. The parameters are a character pointer so essentially a string, and a character pointer hive so another string representing the hive.
The return type is bool so if the string fits the criteria of being a pangram it returns true and if it doesn't then the function returns false.
*/
bool isPangram(char* str, char* hive) {
    int pangramCount = 0;
    for(int i = 0; i < strlen(hive); i++){
        if(findLetter(str, hive[i]) == -1){
            return false;
        }
    }
    return true;
}

/*
This function is to check if the given string is a perfect pangram which means it uses the letters in hive EXACTLY once.
This takes in the parameters of 2 strings, the first one representing the string to check if its a pangram and the
other represents the string for the letters in hive. The return type is a boolean, so if the string meets the requirements
of a perfect pangram, it returns true. Otherwise it returns false.
*/
bool isPerfectPangram(char* str, char* hive){
    if(isPangram(str, hive) && strlen(str) == strlen(hive)){
        return true;
    }

    return false;
}

/*
This function prints the hive string and points to the required letter so the user can know while making words
which letter HAS to be in the word. It takes in a parameter of a string that represents the hive, and then the index
of the required letter. The return type is void so this doesn't return anything, instead it prints the hive string and
the an arrow to the required letter.
*/
void printHive(char* hive, int reqLetInd) {
    printf("  Hive: \"%s\"\n", hive);
    printf("         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^");
    for (int i = reqLetInd + 1; i < strlen(hive); i++) {
        printf(" ");
    }
    printf(" (all words must include \'%c\')\n\n", hive[reqLetInd]);
}


/*
This function is to see if the wordlist found bingo or not which is when all of the letters in the hive
are at the starting index (0) for a word at least once. This takes in the word list from struct WordList which 
contains the words, and then the string that represents the hive. The return type is a boolean so it goes through
the word list and if every letter of the hive was used to start a word at least once, thsi function will return true.
Otherwise false.
*/
bool bingoFound(WordList* thisWordList, char* hive){
    bool bingoLetter[strlen(hive)];
    int letterBingoCount = 0;
    for(int i = 0; i < strlen(hive); i++){
        bingoLetter[i] = false;
    }

    for(int i = 0; i < thisWordList->numWords; i++){
        char* currentWord = thisWordList->words[i];
        if(findLetter(hive, currentWord[0]) != -1){
            int bingoIndex = findLetter(hive, currentWord[0]);
            if(bingoLetter[bingoIndex] == false){
                bingoLetter[bingoIndex] = true;
                letterBingoCount++;
            }
        }
    }
    if(letterBingoCount == strlen(hive)){
        return true;
    }

    return false;
}

/*
This function is to print the list showing the score of each word and showing if it's a pangram or not without showing
the total score, and this is used in task 5-B. This takes in the parameter of a word list from the struct Wordlist, and the
string representing the hive. This function's return type is void so it doesn't return anything but it just prints out the words,
their individual scores, and if they are a pangram or not by using an astericks.
*/
void printListNoTotal(WordList* thisWordList, char* hive){
    int scoreCurrent = 0;
    for(int i = 0; i < thisWordList->numWords; i++){
        scoreCurrent = 0;
        if(isPerfectPangram(thisWordList->words[i], hive) == true){
            printf("  *** ");  
        }else if(isPangram(thisWordList->words[i], hive) == true){
            printf("   *  "); 
        }else{
            printf("      ");
        }

        if(strlen(thisWordList->words[i]) == 4){
            scoreCurrent = 1;
        }else if(isPangram(thisWordList->words[i], hive) == true){
            scoreCurrent = strlen(thisWordList->words[i]) + strlen(hive);
        }else{
            scoreCurrent = strlen(thisWordList->words[i]);
        }

        if(scoreCurrent < 10){
            printf("( %d) %s\n", scoreCurrent, thisWordList->words[i]);
        }else{
            printf("(%d) %s\n", scoreCurrent, thisWordList->words[i]);
        }    
    }
}

/*
This function outputs the words in the list, marks pangrams with asterick and perfect pangram with triple astericks.
This also displays the individual score per word and the total score in the end. This takes in the parameters
of a word list pointer from the struct word list and a string representing the hive. This function has a return type
of void so it returns nothing but it prints out the words, their individual scores, the total score, and if it is a pangram,
perfect pangram or neither.
*/
void printList(WordList* thisWordList, char* hive) {
    printf("  Word List:\n");
    int totScore = 0;
    int currentScore = 0;
    for(int i = 0; i < thisWordList->numWords; i++){
        currentScore = 0;
        if(isPerfectPangram(thisWordList->words[i], hive) == true){
            printf("  *** ");  
        }else if(isPangram(thisWordList->words[i], hive) == true){
            printf("   *  "); 
        }else{
            printf("      ");
        }

        if(strlen(thisWordList->words[i]) == 4){
            currentScore = 1;
        }else if(isPangram(thisWordList->words[i], hive) == true){
            currentScore = strlen(thisWordList->words[i]) + strlen(hive);
        }else{
            currentScore = strlen(thisWordList->words[i]);
        }

        totScore += currentScore;
        if(currentScore < 10){
            printf("( %d) %s\n", currentScore, thisWordList->words[i]);
        }else{
            printf("(%d) %s\n", currentScore, thisWordList->words[i]);
        }
    }
    printf("\nTotal Score: %d\n", totScore);
}

/*
This function goes through each word in the dictionary and determines if the word 
only uses letters in the hive and includes the required letters. Then it adds it to the solved list.
The parameters are a WordList pointer dictionaryList, a WorldList pointer solved list, character pointer hive, and the a character for the letter required.
This function is void so it doesn't return anything, it just updates the solvedList parameter. 
*/
void bruteForceSolve(WordList* dictionaryList, WordList* solvedList, char* hive, char reqLet) {
    for(int i = 0; i < dictionaryList->numWords; i++){
        if(isValidWord(dictionaryList->words[i], hive, reqLet) == true){
            appendWord(solvedList, dictionaryList->words[i]);
        }
    }
}

/*
This function should show if a string is a perfect prefix of the second string or not.
The parameters are character pointers partWord and character pointers fullWord which are 2 strings.
If the partWord is a perfect prefix of the fullWord then the function return true because it is type boolean, 
if not then it return false.
*/
bool isPrefix(char* partWord, char* fullWord) {
    if(strlen(fullWord) < strlen(partWord)){
        return false;
    }
    int prefixCount = 0;
    for(int i = 0; i < strlen(partWord); i++){
        if(partWord[i] == fullWord[i]){
            prefixCount++;
        }
    }
    if(prefixCount == strlen(partWord)){
        return true;
    }

    return false;
}

/*
This function is to check if any words in the word list either partially match or fully match the input given word.
The parameters are a WorldList pointer word list, character pointer aWord which is a string, integer for lower index and integer for higher index.
The return type is an integer and if aWord is found in the word list, the function returns the index of that word in the array.
If aWord isn't in the wordlist but its a root word (so its partially match) to any word in words array then -1 is returned. Any other scenerio
returns -99.
*/
int findWord(WordList* thisWordList, char* aWord, int loInd, int hiInd) {
    if (hiInd < loInd) { // Base case 2: aWord not found in words[]

        if (loInd < thisWordList->numWords && (isPrefix(aWord, thisWordList->words[loInd]) == true)) { //TODO: fix the second part of this if statement
            return -1; //words match this root (partial match)
        }
        else {
            return -99; //no more words matching this root (no match)
        }
    }

    int mdInd = (hiInd + loInd) / 2;

    if (strcmp(thisWordList->words[mdInd], aWord) == 0) { // Base case 1: found tryWord at midInd
        return mdInd;
    }

    if (strcmp(thisWordList->words[mdInd], aWord) < 0) { // Recursive case: search upper half
        return findWord(thisWordList, aWord, mdInd + 1, hiInd);
    }

    // Recursive case: search lower half
    return findWord(thisWordList, aWord, loInd, mdInd - 1);
}
/*
This function builds potential words from the hive letters and then checks if those words are in the dictionary.
This takes in the parameter of a WorldList pointer dictionaryList, another WorldList pointer solvedList, a character pointer tryWord, character pointer hive which are both strings, 
and the character letter for the required letter.
This function's return type is void so it doesn't return anything but it adds valid words to the solved list in 3
scenerios where findWord() finds an exact match, findWord() doesn't find an exact match but finds a prefix, or 
findWord() find NO match and NO prefix. And it does this by recursivley calling itself so it only adds valid words.
*/
void findAllMatches(WordList* dictionaryList, WordList* solvedList, char* tryWord, char* hive, char reqLet) {
    int curLen = strlen(tryWord);
    int index = findWord(dictionaryList, tryWord, 0, dictionaryList->numWords - 1);
    if (index >= 0) {
        if(isValidWord(tryWord, hive, reqLet)){
            appendWord(solvedList, tryWord);
        }
        char nextWord[MAX_HIVE_SIZE];
        strcpy(nextWord, tryWord);
        int nextLen = strlen(nextWord);
        nextWord[nextLen] = hive[0];
        nextWord[nextLen + 1] = '\0';
        findAllMatches(dictionaryList, solvedList, nextWord, hive, reqLet);
        return;
    }
    if (index == -1) {
        char nextWord[MAX_HIVE_SIZE];
        strcpy(nextWord, tryWord);
        int nextWordLen = strlen(nextWord);
        nextWord[nextWordLen] = hive[0];
        nextWord[nextWordLen + 1] = '\0';
        findAllMatches(dictionaryList, solvedList, nextWord, hive, reqLet);
        return;
    }
    else if (index == -99) {
        char nextWord[MAX_HIVE_SIZE];
        strcpy(nextWord, tryWord);
        int wordLength = strlen(nextWord);
        while(wordLength > 0){
            int hiveInd = 0;
            for(int i = 0; i < strlen(hive); i++){
                if(hive[i] == tryWord[wordLength - 1]){
                    hiveInd = i;
                    break;
                }
            }
            if(hiveInd < strlen(hive) - 1){
                nextWord[wordLength - 1] = hive[hiveInd + 1];
                break;
            }
            wordLength--;
            nextWord[wordLength] = '\0';
        }
        if(wordLength > 0){
            findAllMatches(dictionaryList, solvedList, nextWord, hive, reqLet);
        }
        return;
    }
}

/*
This function does the settings for the program like for things like random mode, number of letters, dictionary
play mode, brute force mode, and seed selection. It takes in the parameters of the integer argument counts, character pointer
array representing the argument values, boolean pointer for random mode, bool pointer for num letters, the dictionary file, 
bool pointer for play mode, bool pointer for brute force mode, and bool pointer for seed selection.
*/
bool setSettings(int argc, char* argv[], bool* pRandMode, int* pNumLets, char dictFile[100], bool* pPlayMode, bool* pBruteForceMode, bool* pSeedSelection) {
    //initializes all of the values.
    *pRandMode = false;
    *pNumLets = 0;
    strcpy(dictFile, "dictionary.txt");
    *pPlayMode = false;
    *pBruteForceMode = true;
    *pSeedSelection = false;
    srand((int)time(0));
    //--------------------------------------
    //goes through the argument count.
    for (int i = 1; i < argc; ++i) {
        //checks if the current argument is -r so random mode 
        if (strcmp(argv[i], "-r") == 0) {
            ++i;
            //this checks for any errors
            if (argc == i) {
                return false;
            }
            //sets the random mode to true.
            *pRandMode = true;
            //converts from string to integers
            *pNumLets = atoi(argv[i]);
            //if the number of letters isn't in the range for the hive size its an error so return false.
            if (*pNumLets < MIN_HIVE_SIZE || *pNumLets > MAX_HIVE_SIZE) {
                return false;
            }
        }
        //if the argument is -d so dictionary.
        else if (strcmp(argv[i], "-d") == 0) {
            ++i;
            if (argc == i) {
                return false;
            }
            //performs strcpy of the dictionary filename to the variable at index i.
            strcpy(dictFile, argv[i]);
            //opens that file for reading purposes.
            FILE* filePtr = fopen(dictFile, "r");
            //checks if file is NULL for any errors.
            if (filePtr == NULL) {
                return false;
            }
            fclose(filePtr);
        }
        //if the argument is -s so for seed slection 
        else if (strcmp(argv[i], "-s") == 0) {
            ++i;
            if (argc == i) {
                return false;
            }
            //makes the boolean for seed selection true.
            *pSeedSelection = true;
            //converts argument at index i from string to integer.
            int seed = atoi(argv[i]);
            //randomizes based on that number.
            srand(seed);
        }
        //if the argument is -p so for play mode.
        else if (strcmp(argv[i], "-p") == 0) {
            //sets the play boolean to true.
            *pPlayMode = true;
        }
        //if the argument is -o so for brute force mode.
        else if (strcmp(argv[i], "-o") == 0) {
            //makes brute force mode false.
            *pBruteForceMode = false;
        }
        else {
            return false;
        }
    }
    return true;
}

/*
This function is to see if on or off should be printed.
It takes in the parameter of a boolean mode.
The return type is void so it doesn't return anything but prints ON if mode is true and OFF
if mode is false.
*/
void printONorOFF(bool mode) {
    if (mode) {
        printf("ON\n");
    }
    else {
        printf("OFF\n");
    }
}

/*
This function is to see if YES should be printed or NO.
The parameter for this function is a boolean mode.
The return type is void so it doesn't return anything but it prints YES
if mode is equal to true, and NO if mode is equal to false.
*/
void printYESorNO(bool mode) {
    if (mode) {
        printf("YES\n");
    }
    else {
        printf("NO\n");
    }
}

/*
This function prints the extra variables for printing such as amount of valid words, amount of pangrams, amount of perfect pangrams, 
if there was a bingo or not, and the total score possible. The paramaters taken are the Wordlist struct pointer list of words, string representing the hive,
integer for valid count, integer for pangram count, integer for perfect pangram count, boolean bingo, and the total score count.
The return type is void so it doesn't return anything but just goes through the word list and checks how many
valid words, pangrams, perfect pangrams, bingo or not, and total score from the words.
*/
void scoreVariablesPrint(WordList* thisWordList, char* hive, int valid, int pangramC, int perfectC, bool bingoB, int totalC){
    int currentC = 0;
    for(int i = 0; i < thisWordList->numWords; i++){
        if(isPerfectPangram(thisWordList->words[i], hive) == true){
            perfectC++;
            pangramC++;
        }else if(isPangram(thisWordList->words[i], hive) == true){
            pangramC++;
        }
        valid++;
            
        if(strlen(thisWordList->words[i]) == 4){
            currentC = 1;
        }else if(isPangram(thisWordList->words[i], hive) == true){
            currentC = strlen(thisWordList->words[i]) + strlen(hive);
        }else{
            currentC = strlen(thisWordList->words[i]);
        }

        totalC += currentC;
    }

    //This is the printing for all the variables.
    bingoB = bingoFound(thisWordList, hive);
    printf("    Number of Valid Words: %d\n", valid);
    printf("    Number of ( * ) Pangrams: %d\n", pangramC);
    printf("    Number of (***) Perfect Pangrams: %d\n", perfectC);
    printf("    Bingo: ");
    printYESorNO(bingoB);
    printf("    Total Score Possible: %d\n", totalC);

}



int main(int argc, char* argv[]) {
    printf("\n----- Welcome to the CS 211 Spelling Bee Game & Solver! -----\n\n");
    //all of the arguments are initialized below to integer, characyer, boolean, etc.
    bool randMode = false;
    int hiveSize = 0;
    char dict[100] = "dictionary.txt";
    bool playMode = false;
    bool bruteForce = true;
    bool seedSelection = false;
    char hive[MAX_HIVE_SIZE + 1] = {};
    hive[0] = '\0';
    int reqLetInd = -1;
    char reqLet = '\0';


    //if the setSettings function is not true then the command lines are invalid so it terminates the program.
    if (!setSettings(argc, argv, &randMode, &hiveSize, dict, &playMode, &bruteForce, &seedSelection)) {
        printf("Invalid command-line argument(s).\nTerminating program...\n");
        return 1;
    }
    //if its true then it prints ON or OFF based on the mode and it's boolean value that was initialized before.
    else {
        printf("Program Settings:\n");
        printf("  random mode = ");
        printONorOFF(randMode);
        printf("  play mode = ");
        printONorOFF(playMode);
        printf("  brute force solution = ");
        printONorOFF(bruteForce);
        printf("  dictionary file = %s\n", dict);
        printf("  hive set = ");
        printYESorNO(randMode);
        printf("\n\n");
    }

    // build word array (only words with desired minimum length or longer) from dictionary file
    printf("Building array of words from dictionary... \n");
    WordList* dictionaryList = createWordList();
    int maxWordLength = buildDictionary(dict, dictionaryList, MIN_WORD_LENGTH);
    if (maxWordLength == -1) {
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of valid words.\n");
        printf("Terminating program...\n");
        return -1;
    }
    printf("   Word array built!\n\n");


    printf("Analyzing dictionary...\n");

    //analyzes the dictionary to see if program should terminate or not.
    if (dictionaryList->numWords < 0) {
        printf("  Dictionary %s not found...\n", dict);
        printf("Terminating program...\n");
        return -1;
    }

    // end program if file has zero words of minimum desired length or longer
    if (dictionaryList->numWords == 0) {
        printf("  Dictionary %s contains insufficient words of length %d or more...\n", dict, MIN_WORD_LENGTH);
        printf("Terminating program...\n");
        return -1;
    }
    else {
        printf("  Dictionary %s contains \n  %d words of length %d or more;\n", dict, dictionaryList->numWords, MIN_WORD_LENGTH);
    }


    //this runs if the mode is random.
    if (randMode) {
        printf("==== SET HIVE: RANDOM MODE ====\n");
        //find number of words in words array that use hiveSize unique letters
        WordList* fitWords = findAllFitWords(dictionaryList, hiveSize);
        int numFitWords = fitWords->numWords;
        //pick one at random
        int pickOne = rand() % numFitWords;
        char* chosenFitWord = fitWords->words[pickOne];

        //and alaphabetize the unique letters to make the letter hive
        buildHive(chosenFitWord, hive);
        freeWordList(fitWords);

        reqLetInd = rand() % hiveSize;
        reqLet = hive[reqLetInd];

    }
    else {
        //this is when random mode is off.
        printf("==== SET HIVE: USER MODE ====\n");
        //initializes variables.
        bool isDupilcate = false;
        bool isInvalid = true;
        reqLetInd = -1;
        reqLet = '\0';

        //this while loop is for checking if the hive string the user will enter is valid or not.
        //it will keep looping until the user enters a valid string.
        while(isInvalid == true){
            printf("  Enter a single string of lower-case,\n  unique letters for the letter hive... ");
            scanf("%s", hive);

            //this is for checking of the length of letters in hive string is within the minimum hive size and maximum hive size.
            //if not then the loop starts again.
            hiveSize = strlen(hive);
            if(hiveSize < MIN_HIVE_SIZE || hiveSize > MAX_HIVE_SIZE){
                printf("  HIVE ERROR: \"%s\" has invalid length;\n  valid hive size is between %d and %d, inclusive\n\n",hive, MIN_HIVE_SIZE, MAX_HIVE_SIZE);
                continue;
            }

            //this for loop goes through each letter in the hive string because it should be the valid size now.
            //if the letter is not lowercase and its not a letter then it's invalid and the loop will start again for making sure hive string is valid.
            isInvalid =  false;
            for(int i = 0; i < hiveSize; i++){
                if(!(islower(hive[i]) && isalpha(hive[i]))){
                    printf("  HIVE ERROR: \"%s\" contains invalid letters;\n  valid characters are lower-case alpha only\n\n",hive);
                    isInvalid = true;
                    break;
                }
            }

            if(isInvalid == true){
                continue;
            }

            //this checks if there is a duplicate letter in the string hive.
            isDupilcate = false;
            //it loops through the string and compares each letter with the other letters and if it is 
            //equal to even one that mean's the whole string is invalid.
            for(int i = 0; i < hiveSize; i++){
                for(int j = 0; j < hiveSize; j++){
                    if(hive[i] == hive[j] && i != j){
                        isDupilcate = true;
                        break;
                    }
                }
                if(isDupilcate == true){
                    break;
                }  
            }
            
            //prints error message for being duplicate.
            if(isDupilcate == true){
                printf("  HIVE ERROR: \"%s\" contains duplicate letters\n\n", hive);
                isInvalid = true;
            }

            //this makes sure the word is valid because if isInvalid is false and isDupilcate is false
            //that means theres no invalidness in the string nor duplicates.
            if(isInvalid == false && isDupilcate == false){
                break;
            }
        }
        //creates input to hold the value of hive then uses buildhive to create the hive with the valid letters.
        char input[100];
        strcpy(input, hive);
        buildHive(input, hive);

        //this invalid loop is for making sure the required letter is valid.
        isInvalid = true;
        while(isInvalid == true){
            printf("  Enter the letter from \"%s\"\n  that is required for all words: ", hive);
            scanf(" %c", &reqLet);
            reqLet = tolower(reqLet);
            //utilizes findletter function to see if the reqlet the user input is even in the hive. If it's not then the loop
            //will exit by setting isInvalid to false.
            if(findLetter(hive, reqLet) == -1){
                printf("  HIVE ERROR: \"%s\" does not contain the character \'%c\'\n\n",hive,reqLet);
            }else{
                isInvalid = false;
            }
        }
        //sets the index to what findLetter's index is.
        reqLetInd = findLetter(hive, reqLet);
    }

    printHive(hive, reqLetInd);

    if (playMode) {
        printf("==== PLAY MODE ====\n");

    //---------------------------------------------------------------------
    //              BEGINNING OF OPEN-ENDED GAMEPLAY SECTION
    //---------------------------------------------------------------------
    
            /* TODO (Task 4, OPEN-ENDED GAMEPLAY): develop a creative gameplay extension.
                The code is currently a framework for repeatedly taking in words from the user, 
                until they enter DONE. Extend/replace/modify this section to implement
                your creative gameplay extension.

            */

        /*
        This open ended task will be a modification and addition to the game created already. 
        I have created a streak counter that will keep track of how many valid words the user has done 
        and also use their word's last letter to make them start the new word with that letter.
        So if the user enters the word computer, the last letter is r so they will need to start their next word
        with the letter r. Once they enter an invalid word the streak will be gone and it will show them what their streak was.
        Once they enter DONE it will show their final streak and end the game. There is also an incentive for pangrams
        and perfect pangrams because if they get either, they can CHOOSE the next starting letter for the next word.
        */
        //this allocates memory for the word the user will input.
        char* userWord = (char*)malloc((maxWordLength + 1) * sizeof(char));
        strcpy(userWord, "default");
        //creates variable for streak counter and the a character for the first letter.
        int streak = 0;
        char startLetter = '\0';
        //creates a word list using the struct Wordlist.
        WordList* userWordList = createWordList();

        printf("  Welcome to the last letter game mode! \n");
        printf("  All words MUST start with the last letter of the previous word \n");
        printf("  If it is a pangram you can choose the next letter you want to start with! \n");
        printf("............................................\n");
        printHive(hive, reqLetInd);
        printf("  Enter a word (enter DONE to quit): ");
        scanf("%s", userWord);
        printf("\n");
        while (strcmp(userWord, "DONE")) {
            //creates booleans for function usages of isValidWord and  wordCheck, and a bool reqCheck
            bool validWord = isValidWord(userWord, hive, reqLet);
            bool ifExist =  wordCheck(userWordList, userWord);
            bool reqCheck = true;
            //if the start letter is null and the first letter of the user's word is not start letter
            //there is no need for the chain to start now so reqCheck is false.
            if(startLetter != '\0' && userWord[0] != startLetter){
                reqCheck = false;
            }
            
            if(validWord == true && ifExist == false && reqCheck == true){
                //appends the word and increases the streak if the word is valid.
                //1) this checks for the validity of the word.
                appendWord(userWordList, userWord);
                streak++;
                //2) this regularily displays the users words and and score of words they entered.
                printList(userWordList, hive);

                //3) performs check for pangrams and perfect pangrams (every perfect pangram is a pangram)
                //this is a check for the pangram and allows the user to pick their next letter starting.
                if(isPerfectPangram(userWord, hive) == true){
                    printf("  You found a Perfect Pangram! You can choose the next starting letter. \n");
                    printf("  Enter the letter from \"%s\" for the next word, if invalid it will be skipped:  \n", hive);
                    char nextLet;
                    scanf(" %c", &nextLet);
                    if(findLetter(hive, nextLet) == -1){
                        startLetter = userWord[strlen(userWord) - 1];
                        
                        printf("  Letter is invalid, this is skipped and next word will start with the last letter of the perfect pangram. \n");
                    }
                    else{
                        startLetter = nextLet;
                        printf("  Next word MUST start with the letter %c. \n", nextLet);
                    }
                //this is a check for a perfect pangram and allows the user to pick their next starting letter.
                }else if (isPangram(userWord, hive) == true){
                    printf("  You found a Pangram! You can choose the next starting letter. \n");
                    printf("  Enter the letter from \"%s\" for the next word, if invalid it will be skipped:  \n", hive);
                    char nextLet;
                    scanf(" %c", &nextLet);
                    if(findLetter(hive, nextLet) == -1){
                        startLetter = userWord[strlen(userWord) - 1];
                        
                        printf("  Letter is invalid, this is skipped and next word will start with the last letter of pangram. \n");
                    }
                    else{
                        startLetter = nextLet;
                        printf("  Next word MUST start with the letter %c. \n", nextLet);
                    }
                //this is if its not a pangram or a perfect pangram.
                }else{
                    if(strlen(userWord) > 0){
                        startLetter = userWord[strlen(userWord) - 1];
                    }
                }
            //performs the breakage and reset of streak once an invalid word is inputted by the user.
            } else{
                printf("  This word is invalid, you had a streak of %d. \n", streak);
                streak = 0;
                startLetter = '\0';
            }

            //prints the list for the score and each words individual score as well as the pangrams and perfect pangrams.
            printf("\n");
            if(validWord == false || ifExist == true || reqCheck == false){
                printList(userWordList, hive);
                printf("\n");
            }

            printf("............................................\n");

            //this is for for after the first word and beyond so the next words HAVE to start with the last letter of the previous word.
            if(startLetter != '\0'){
                printf("  Next word MUST start with the letter %c. \n", startLetter);
            //otherwise it is the starting word and they enter the required letter each word must have.
            }else{
                printf("  Enter the letter from \"%s\" for the next word \n", hive);
            }
            printHive(hive, reqLetInd);
            //enters the word and DONE if the user wants to quit.
            printf("  Enter a word (enter DONE to quit): ");
            scanf("%s", userWord);
            printf("\n");

        }

        //while loop is over and the user prints the final streak.
        printf("  Your final streak was %d. \n", streak);

        //frees userList and userWord because that was allocated at the beginning.
        freeWordList(userWordList);
        free(userWord);

    //---------------------------------------------------------------------    
    //                 END OF OPEN-ENDED GAMEPLAY SECTION
    //---------------------------------------------------------------------
    }
    //computer solver aspect.
    printf("==== SPELLING BEE SOLVER ====\n");

    //prints the arrow to the required letter from the hive that needs to be in every word
    printf("  Valid words from hive \"%s\":\n", hive);
    printf("                         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^\n");

    //creates a solved list from the Wordlist struct.
    WordList* solvedList = createWordList();

    if (bruteForce) { //find all words that work... (1) brute force
        bruteForceSolve(dictionaryList, solvedList, hive, reqLet);
    }
    else {
        //allocates memory for the tryWord and utilizes the recursive function created to add
        //all the valid words to the solved list.
        char* tryWord = (char*)malloc(sizeof(char) * (maxWordLength + 1));
        tryWord[0] = hive[0];
        tryWord[1] = '\0';
        findAllMatches(dictionaryList, solvedList, tryWord, hive, reqLet);
        free(tryWord);
    }

    int longestSolvedWordLen = 0;
    for (int i = 0; i < solvedList->numWords; i++) {
        if (strlen(solvedList->words[i]) > longestSolvedWordLen) {
            longestSolvedWordLen = strlen(solvedList->words[i]);
        }
    }

    // Helpful variables
    int numValidWords = 0;
    int numPangrams = 0;
    int numPerfectPangrams = 0;
    int totScore = 0;
    int score = 0;
    bool isBingo = true;

    //uses the print list with no total score to print each word in the hive, their individual score
    //and if the word is a pangram, perfect pangram, or neither.
    printListNoTotal(solvedList, hive);
    printf("\n");
    printf("  Total counts for hive \"%s\":\n", hive);
    printf("                         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^\n");
    //prints the total counts for valid words, number of pangrams, number of perfect pangra,s, if it is bingo, and the total score.
    scoreVariablesPrint(solvedList, hive, numValidWords, numPangrams, numPerfectPangrams, isBingo, totScore);
    freeWordList(dictionaryList);
    freeWordList(solvedList);
    printf("\n\n");
    return 0;
}
