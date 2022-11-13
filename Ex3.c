
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX 512

int spaceInTheBegin(char[]); // Check if the is spaces in in the begin or end .
void freeMatrix(char*[]); // Free the malloc from the system .
int checkingTheWordDone(char []); // If the word is done the function would return 1 .
int countWords(char []);
int checkingTheCommandCd(char []); //If the word is cd the function would return 1 .
int checkingTheCommandHistory(char []); //If the word is history the function would return 1 .
int checkingTheCharMark(char []); // If the first char is '!' , the function check the next chars and if its only number she would return the number after .
int countDig(int);
void switchFromNumberToCommand(int,char[]);
int findMark(char []);
int findPipe(char []);
void sig_handler(int);
int findAnpresent(char []);
int checkingTheWordNoHup(char []);

int main() {
    char arr[MAX]; // The array that receive the word from the scanner .
    char arrCountline[MAX];// Array that check how much line there is in the file.txt .
    char cwdTest[256];

    int counter = 0 ;
    int numOfAllWords = 0;
    int numOfWordInM2 = 0;
    int numOfWordInM3 = 0;
    int numOfCommands = 0;
    int totalWords = 0;
    int testExecvp = 0; // Testing if the command execvp succeed .
    int testHistory = 0 ;//Test if we insert the word history .
    int prevCommandWasMark = 0 ; //Would change if the command will be like "!+number" .

    int countThePipeMark = 0;

    FILE* fp1;
    fp1 = fopen("file.txt", "a+");
    if(fp1==NULL){
        printf("Cannot open the file .\n");
        exit(1);
    }
    while (fgets(arrCountline, MAX, fp1)!=NULL) {
        counter++; // Count the line in the file , if there is no file , the counter will be 0 .
    }
    fclose(fp1);
    int numOfRunningLoop = 1 + counter; // To know how much line there is in the file.txt .

    while (1) {// Stop if the scanner insert "done" .

        if (prevCommandWasMark == 0) {// If the prevCommandWasMark is not zero we already have command from the file .
            if (getcwd(cwdTest, sizeof(cwdTest)) == NULL)
                perror("getcwd() failed");
            else
                printf("\n%s>", cwdTest);
        }
        if (prevCommandWasMark == 0) {// If the prevCommandWasMark is not zero we already have command from the file .
            fgets(arr, MAX, stdin);
            if (spaceInTheBegin(arr) == 1) {
                while (spaceInTheBegin(arr) ==
                       1) { // Check if the string is correct , if there is no space in the start.
                    fprintf(stderr, "Invalid command ! Enter String, or done to end program:\n ");
                    fgets(arr, MAX, stdin);
                }
            }
        }

        char **arrMat;
        arrMat = (char **) malloc(MAX * sizeof(char *)); // The pointer for the matrix .
        if (arrMat == NULL) { // Checking malloc .
            perror("malloc failed ");
            exit(1);
        }

        char **arrMat2; // Creating another mat for the command after the pipe .
        arrMat2 = (char **) malloc(MAX * sizeof(char *)); // The pointer for the matrix .
        if (arrMat2 == NULL) { // Checking malloc .
            perror("malloc failed ");
            exit(1);
        }

        char **arrMat3; // Creating another mat for the command after the pipe .
        arrMat3 = (char **) malloc(MAX * sizeof(char *)); // The pointer for the matrix .
        if (arrMat3 == NULL) { // Checking malloc .
            perror("malloc failed ");
            exit(1);
        }

///--------------------------------------------SIGNAL--------------------------------------------------
        int checkTheSig = 0;
        int indexAnpresent = findAnpresent(arr);

        if (indexAnpresent != -1) {//If there is '&' then signal .
            signal(SIGCHLD, sig_handler);
            checkTheSig = 1;
            arr[indexAnpresent] = '\n';
            arr[indexAnpresent + 1] = '\0';
        }
        else
            signal(SIGCHLD, SIG_DFL); // Else regular .


///--------------------------------------------Inserting the sentence to the matrix , (arrMat , arrMat2 ,arrMat3)--------------------------------------------------
        int i;
        char arrWord[MAX]; //Array that save only one word .
        int indexArrayWord = 0;
        int numOfChars = 0;
        for (i = 0; arr[i] != '\0'; i++) {
            if (arr[i] != ' ' && arr[i] != '\n') {
                while (arr[i] != ' ' &&
                       arr[i] != '\n') { // The loop will check the word , and raise the word and chars respectively
                    if (arr[i] == '|') {//checking if there is pipe (|) in the command .
                        countThePipeMark++;
                        break;
                    }
                    arrWord[indexArrayWord] = arr[i];
                    i++;
                    numOfChars++;
                    indexArrayWord++;
                }

                arrWord[indexArrayWord] = '\0'; //Closing the array that contains one word .

                if (countThePipeMark == 0 || countThePipeMark == 1) {
                    arrMat[numOfAllWords] = (char *) malloc((numOfChars + 1) *
                                                            sizeof(char)); //Assigns a space in the array of the matrix according to the size we found .
                    if (arrMat[numOfAllWords] == NULL) { // Checking malloc .
                        perror("malloc failed ");
                        exit(1);
                    }
                    strcpy(arrMat[numOfAllWords],
                           arrWord);// copy the word from the array word into the matrix in the appropriate index .
                    numOfAllWords++;
                    numOfChars = 0;//reset the chars to find the next word .
                    indexArrayWord = 0; // reset the index in the array word .
                    if (countThePipeMark == 1 || arr[i] ==
                                                 '\n') {// For insert the word before the pipe to the first mat and move to the second mat .
                        countThePipeMark++;
                    }
                } else if (countThePipeMark == 2 || countThePipeMark == 3) {
                    arrMat2[numOfWordInM2] = (char *) malloc((numOfChars + 1) *
                                                             sizeof(char)); //Assigns a space in the array of the matrix according to the size we found .
                    strcpy(arrMat2[numOfWordInM2],
                           arrWord);// copy the word from the array word into the matrix in the appropriate index .
                    numOfWordInM2++;
                    numOfChars = 0;//reset the chars to find the next word .
                    indexArrayWord = 0; // reset the index in the array word .
                    if (countThePipeMark == 3 || arr[i] ==
                                                 '\n') {// For insert the word before the pipe to the first mat and move to the second mat .
                        countThePipeMark++;
                    }
                } else {
                    arrMat3[numOfWordInM3] = (char *) malloc((numOfChars + 1) *
                                                             sizeof(char)); //Assigns a space in the array of the matrix according to the size we found .
                    strcpy(arrMat3[numOfWordInM3],
                           arrWord);// copy the word from the array word into the matrix in the appropriate index .
                    numOfAllWords++;
                    numOfWordInM3++;
                    numOfChars = 0;//reset the chars to find the next word .
                    indexArrayWord = 0; // reset the index in the array word .
                }
            }
        }

        countThePipeMark = 0;// reset the pipe counting.
        int index = countWords(arr);
        arrMat[index] = NULL; // Closing the Matrix.
        arrMat2[numOfWordInM2] = NULL;
        arrMat3[numOfWordInM3] = NULL;


        ///--------------------------------------------DONE--------------------------------------------------

        if (checkingTheWordDone(arrWord) == 1 && numOfAllWords == 1) { // Check if the string is "done" .
            printf("Num of commands: %d\nTotal number of words in all commands: %d", numOfCommands + 1, totalWords);
            freeMatrix(arrMat);//Free the memory.
            freeMatrix(arrMat2);//Free the memory.
            freeMatrix(arrMat3);//Free the memory.

            exit(1);
        }

        ///--------------------------------------------CD--------------------------------------------------
        if (checkingTheCommandCd(arrWord) == 1 && numOfAllWords == 1) { // Check if the string is "cd" .
            printf("command not supported (Yet)\n");
            freeMatrix(arrMat);
            freeMatrix(arrMat2);
            freeMatrix(arrMat3);
            numOfAllWords = 0;
            continue;
        }


        ///--------------------------------------------Mark !--------------------------------------------------
        if (checkingTheCharMark(arr) != -1) {// Checking if the command looks like "!+number".
            //int finishInsertToArray = 0;
            int numAfterMarkInM1 = checkingTheCharMark(arr);// Number after the Mark .
            int numAfterMarkInM2 = checkingTheCharMark(arrMat2[0]);
            int numAfterMarkInM3 = checkingTheCharMark(arrMat3[0]);
            if (numAfterMarkInM1 > numOfRunningLoop - 1 || numAfterMarkInM2 > numOfRunningLoop - 1 ||
                numAfterMarkInM3 > numOfRunningLoop - 1) {
                printf("NOT IN HISTORY.");
                numOfAllWords = 0; // reset the number of words , for the next loop iteration .
                freeMatrix(arrMat);
                freeMatrix(arrMat2);
                freeMatrix(arrMat3);
                continue;
            }
            if (numOfWordInM2 > 0 && numOfWordInM3 == 0) {// If there is 2 pipes .
                switchFromNumberToCommand(numAfterMarkInM1, arr); // Switch the number in arr to command respectively .
                strcat(arr, arrMat2[0]);// Add the next mark with number to the arr .
                switchFromNumberToCommand(numAfterMarkInM2, arr);
                i = 0;
                while (arr[i] != '\n') {
                    i++;
                }
                arr[i] = '|';
                i = 0;
                while (arr[i] != '\n') {
                    i++;
                }
                arr[i + 1] = '\0'; // Closing arr .
            } else if (numOfWordInM2 > 0 && numOfWordInM3 > 0) { //If there is 3 pipes .
                switchFromNumberToCommand(numAfterMarkInM1, arr);
                strcat(arr, arrMat2[0]);
                switchFromNumberToCommand(numAfterMarkInM2, arr);
                i = 0;
                while (arr[i] != '\n') {
                    i++;
                }
                while (arr[i + 1] != '\n') {
                    i++;
                }
                arr[i + 2] = '\0';
                strcat(arr, arrMat3[0]);
                switchFromNumberToCommand(numAfterMarkInM3, arr);
                i = 0;
                while (arr[i] != '\n') {
                    i++;
                }
                arr[i] = '|';

                i = 0;
                while (arr[i] != '\n') {
                    i++;
                }
                arr[i] = '|';

                i = 0;
                while (arr[i] != '\n') {
                    i++;
                }
                arr[i + 1] = '\0';
            } else { //There is no pipe at all .
                switchFromNumberToCommand(numAfterMarkInM1, arr);
                while (arr[i] != '\n') {
                    i++;
                }
                arr[i + 1] = '\0';
            }
            prevCommandWasMark = 1;//The scanner would not need to insert string , the arr got it from the file .
            numOfAllWords = 0; // Reset the words for the next string from the scanner .
            numOfWordInM2 = 0;
            numOfWordInM3 = 0;
            freeMatrix(arrMat);
            freeMatrix(arrMat2);
            freeMatrix(arrMat3);
            continue;
        }

///--------------------------------------------history--------------------------------------------------
        if (checkingTheCommandHistory(arr) == 1 && numOfAllWords == 1) {//If the word is "history".
            testHistory = 1;
            char arrForHistory[MAX];

            fp1 = fopen("file.txt", "r");

            if (fp1 == NULL) {
                printf("Cannot open the file .\n");
                exit(1);
            }
            while (fgets(arrForHistory, MAX, fp1) != NULL) {
                printf("%s", arrForHistory); // Print the words from the file.
            }
            fclose(fp1);
            numOfCommands++;//For include the word history after insert the word done .
            totalWords++;//For include the word history after insert the word done .
        }




///--------------------------------------------nohup--------------------------------------------------
        if (checkingTheWordNoHup(arrMat[0]) == 1) {//If the first word is nohup
            int fd = open("nohup.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            pid_t y = fork();
            if (y == 0) {
                signal(SIGHUP, SIG_IGN);
                int value = dup2(fd, STDOUT_FILENO);
                if (value == -1) {
                    fprintf(stderr, "dup2 faild\n");
                    exit(1);
                }
                if (execvp(arrMat[1], arrMat+1) == -1 &&
                    checkingTheCommandHistory(arr) == 0) { // Check if the execvp succeed .
                    testExecvp = -1;
                    perror("\nexecvp() failed");
                }
            }

            freeMatrix(arrMat);
            freeMatrix(arrMat2);
            freeMatrix(arrMat3);
        }

///--------------------------------------------pipes--------------------------------------------------
        else {
            int pipe_fd[2];
            int pipe_fd1[2];


            pid_t x;
            pid_t x1;
            pid_t x2;

            if (pipe(pipe_fd) == -1) {
                perror("pipe failed");
                exit(EXIT_FAILURE);
            }
            if (pipe(pipe_fd1) == -1) {
                perror("pipe failed");
                exit(EXIT_FAILURE);
            }

            x = fork();//First son.

            if (x == 0) {
                if (numOfWordInM2 > 0) {
                    dup2(pipe_fd[1], STDOUT_FILENO);
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                    close(pipe_fd1[0]);
                    close(pipe_fd1[1]);
                }
                if (execvp(arrMat[0], arrMat) == -1 &&
                    checkingTheCommandHistory(arr) == 0) { // Check if the execvp succeed .
                    testExecvp = -1;
                    perror("\nexecvp() failed");
                }
                exit(0);
            }

            if (numOfWordInM2 > 0) {
                x1 = fork();//Second son.
                if (x1 == 0) {
                    if (numOfWordInM3 == 0) {

                        dup2(pipe_fd[0], STDIN_FILENO);
                        close(pipe_fd[1]);
                        close(pipe_fd[0]);
                        close(pipe_fd1[1]);
                        close(pipe_fd1[0]);
                        if (execvp(arrMat2[0], arrMat2) == -1 &&
                            checkingTheCommandHistory(arr) == 0) { // Check if the execvp succeed .
                            testExecvp = -1;
                            perror("\nexecvp() failed");
                        }
                        exit(0);
                    } else if (numOfWordInM3 > 0) { //If there is double pipe .);
                        dup2(pipe_fd[0], STDIN_FILENO);
                        dup2(pipe_fd1[1], STDOUT_FILENO);
                        close(pipe_fd[1]);
                        close(pipe_fd1[0]);
                         close(pipe_fd[0]);
                         close(pipe_fd1[1]);
                        if (execvp(arrMat2[0], arrMat2) == -1 &&
                            checkingTheCommandHistory(arr) == 0) { // Check if the execvp succeed .
                            testExecvp = -1;
                            perror("\nexecvp() failed");
                        }
                        exit(0);
                    }
                }
            }


            if (numOfWordInM3 > 0) {
                x2 = fork();//Third son .
                if (x2 == 0) {
                    dup2(pipe_fd1[0], STDIN_FILENO);
                    close(pipe_fd1[1]);
                    close(pipe_fd1[0]);
                    close(pipe_fd[1]);
                    close(pipe_fd[0]);
                    if (execvp(arrMat3[0], arrMat3) == -1 &&
                        checkingTheCommandHistory(arr) == 0) { // Check if the execvp succeed .
                        testExecvp = -1;
                        perror("\nexecvp() failed");
                    }
                    exit(0);
                }
            }


            if (x == -1) {
                perror("fork failed");
                freeMatrix(arrMat);
                exit(1);
            }

            if (numOfWordInM2 > 0 && numOfWordInM3 == 0) {

                close(pipe_fd[0]);
                close(pipe_fd[1]);
                close(pipe_fd1[0]);
                close(pipe_fd1[1]);
                if (checkTheSig == 0) {
                    waitpid(x, NULL, 0);
                    printf("\ntest1\n");
                    waitpid(x1, NULL, 0);
                    printf("\ntest2\n");
                }
            } else if (numOfWordInM3 > 0) {

                close(pipe_fd[0]);
                close(pipe_fd[1]);
                close(pipe_fd1[0]);
                close(pipe_fd1[1]);
                if (checkTheSig == 0) {
                    waitpid(x, NULL, 0);

                    waitpid(x1, NULL, 0);

                    waitpid(x2, NULL, 0);

                }
            }
            else {
                if (checkTheSig == 0) {
                    int status;
                    wait(&status);
                    if (WEXITSTATUS(status) == 1) { // Check what the son returned .
                        testExecvp = -1;
                    }
                }
            }
        freeMatrix(arrMat); //Free the memory if the execvp succeed
        freeMatrix(arrMat2);
        freeMatrix(arrMat3);

    }
///--------------------------------------------End Pipes--------------------------------------------------


        fp1 = fopen("file.txt", "a");
        if(fp1==NULL){
            printf("Cannot open the file .\n");
            exit(1);
        }
        if((numOfAllWords >0 && testExecvp != -1) || testHistory==1){ // Insert the commands into the file.txt .

            fprintf (fp1 , "%d " , numOfRunningLoop);//Insert the sentences from the scanner to the file .
            fputs(arr,fp1);
            numOfRunningLoop++; // To know how much sentences there is .
            testHistory=0;
        }

        fclose(fp1);

        if(testExecvp == -1) {
            numOfAllWords = 0;
            numOfWordInM2 = 0;
            numOfWordInM3 = 0;
            testExecvp = 0; //reset the testExecvp for the next strings .
            prevCommandWasMark = 0 ; //Reset the test command from the file .
            countThePipeMark = 0;//Reset the pipe count for the next command .
        }
        else {
            totalWords += numOfAllWords;
            numOfCommands++;
            numOfAllWords = 0; // Reset the words for the next string from the scanner .
            numOfWordInM2 = 0; // Reset the count for the next string from the scanner to insert mat 2 if there is a pipe.
            numOfWordInM3 = 0;// Reset the count for the next string from the scanner to insert mat 3 if there is a pipe.
            prevCommandWasMark = 0 ; //Reset the test command from the file .
            countThePipeMark = 0;//Reset the pipe count for the next command .
        }
    }
}

///--------------------------------------------FUNCTIONS--------------------------------------------------

int spaceInTheBegin(char array[]) {
    int countDig = 0;
    int i;
    for(i=0;array[i]!='\n';i++) {
        if (array[i] != ' ')
            countDig++;
    }
    if ( array[0] == ' ' && countDig!=0)   // Moving the pivot after the space if there is .
        return 1;
    else
        return 0;
}

void freeMatrix(char* matrix[]){
    int i;
    for(i=0;matrix[i]!=NULL;i++)
        free(matrix[i]);
    free(matrix[i]);//free the line that contain NULL.
    free(matrix);
}

int checkingTheWordDone(char array[]){
    if(strcmp(array,"done")==0){
        return 1;
    }
    else
        return 0;
}

int countWords(char array[] ) {
    int i=0;
    int count = 0;
    while (array[i] != '|' && array[i] != '\n') {
        if (array[i] != '|' && array[i] != '\n' && array[i] != ' ') {//ls -l
            while (array[i] != '|' && array[i] != '\n' && array[i] != ' ') {
                i++;
            }
            count++;
        }
        else
            i++;
    }
    return count;
}


int checkingTheCommandCd(char array[]){
    if(strcmp(array,"cd")==0){
        return 1;
    }
    else
        return 0;
}

int checkingTheCommandHistory(char array[]){
    if(strcmp(array,"history\n")==0 || strcmp(array,"history")==0){
        return 1;
    }
    else
        return 0;
}

int checkingTheCharMark(char array[]){
    int i;
    int numAfterMark = 0;
    if(array==NULL)
        return -1;
    if(array[0]=='!' && array[1] != '0' ){
        for(i=1; (array[i]!='\n' && array[i]!='\0') ; i ++){
            if(array[i]>=48 && array[i] <= 57 ) {
                numAfterMark = (numAfterMark * 10) + (array[i]-48);
            }
            else if(array[i] == '|' || array[i] == ' ')
                return numAfterMark;
            else
                return -1;
        }
        return numAfterMark;
    }
    else
        return -1;
}

int countDig(int num){
    int i;
    int count = 0;
    for(i = 0 ; num>0 ; i++){
        num = num / 10;
        count++;
    }
    return count;
}

void switchFromNumberToCommand (int numAfterMark , char arr [] ){
    FILE* fp1;
    char arrNum[MAX];
    char arrCommand[MAX];

    int countFile = 1; // the index that running all over the file .
    int countArrCommand = 0;

    fp1 = fopen("file.txt", "r");
    if (fp1 == NULL) {
        printf("Cannot open the file .\n");
        exit(1);
    }
    while (fgets(arrNum, MAX, fp1) != NULL) {

        if (countFile == numAfterMark) {

            int countArrNum = countDig(numAfterMark) + 1;//We want to start comparing after the number and after the space . (number (...)) .

            if(findPipe(arrNum)!=1) {
                while (arrNum[countArrNum] != '\n' && arrNum[countArrNum] != '|') {

                    arrCommand[countArrCommand] = arrNum[countArrNum];//Insert only the command into the array that will have only the command .
                    countArrCommand++;
                    countArrNum++;
                }
            }
            else
                while (arrNum[countArrNum] != '\n') {

                    arrCommand[countArrCommand] = arrNum[countArrNum];//Insert only the command into the array that will have only the command .
                    countArrCommand++;
                    countArrNum++;
                }

            arrCommand[countArrCommand] = '\n';
            int i = 0;
            int indexMark = findMark(arr);//Save the mark index in the main array , that the scanner insert string .

            while(arrCommand[i]!='\n'){
                arr[indexMark]=arrCommand[i];
                i++;
                indexMark++;
            }

            arr[indexMark]='\n';
            break;
        }
        else
            countFile++;//For running all over the File.
    }
    fclose(fp1);
}

int findMark(char array[]){
    int i = 0 ;
    int count = 0 ;
    while(array[i]!='!'){
        count++;
        i++;
    }
    return count;
}

int findPipe(char array[]){
    int i = 0 ;
    while(array[i]!='\n'){
        if(array[i]=='|')
            return 1;
        i++;
    }
    return 0;
}

void sig_handler(int sig){
        wait(NULL);
}

int findAnpresent(char array [] ) { //Search for & , returns the index or -1 if there is no & .
    int i = 0 ;
    while(array[i]!='\0'){
        if(array[i]=='&')
            return i;
        i++;
    }
    return -1;
}

int checkingTheWordNoHup(char array []){
    if(array==NULL)
        return 0;
    if(strcmp(array,"nohup\n")==0 || strcmp(array,"nohup")==0){
        return 1;
    }
    else
        return 0;
}


