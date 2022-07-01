#include "Arduino.h"
#include "Question.h"

char operators[] = {')', '(', '^','*', '/', '+', '-'};

const unsigned int easyMax = 25;
const unsigned int mediumMax = 50;
const unsigned int hardMax = 50;
const unsigned int expMax = 3;

Question::Question() {}

//Generates a question with given difficulty and question.
Question::Question(String diff, String quest)
{
    // if (diff != EASY || diff != MEDIUM || diff != HARD) {
    //     throw "Difficulty must be EASY, MEDIUM or HARD";
    // }
    _difficulty = diff;
    _question = quest;
}

//Generates a question with given difficulty and random question.
Question::Question(String diff)
{
    // if (diff != EASY || diff != MEDIUM || diff != HARD) {
    //     throw "Difficulty must be EASY, MEDIUM or HARD";
    // }
    _difficulty = diff;
    String quest = "";
    int amOperations; 
    //Geeft een berekening terug van 1 tot 3 operaties, met getallen tot 26. Operaties: *, /, +, -
    if (diff == EASY){
        amOperations = random(1,3);
        quest = quest + String(random(0,easyMax+1));
        for (int i=0; i<amOperations;i++){
            char op = operators[random(3,sizeof(operators)/sizeof(operators[0]))];
            quest = quest + op + String(random(0,easyMax+1));
        }
        _question = quest;
    }
    //Geeft een berekening terug van 2 tot 5 operaties, met getallen tot 51. Operaties: ^, *, /, +, -
    else if (diff == MEDIUM) {
        amOperations = random(2,5);
        quest = quest + String(random(0, mediumMax+1));
        for (int i=0; i<amOperations; i++){
            char op = operators[random(2,sizeof(operators)/sizeof(operators[0]))];
            if (op == '^') {quest = quest + op + String(random(0,expMax+1));}
            else {quest = quest + op + String(random(0, mediumMax+1));}
        }
        _question = quest;
    }
    //Geeft een berekening terug van 3 tot 7 operaties, met getallen tot 51. Operaties: ^, *, /, +, - en haakjes (TODO)
    else if (diff == HARD) {
        amOperations = random(3,7);
        quest = quest + String(random(0,hardMax+1));
        for (int i=0; i<amOperations; i++){
            char op = operators[random(2,sizeof(operators)/sizeof(operators[0]))];
            if (op == '^') {quest = quest + op + String(random(0,expMax+1));}
            else {quest = quest + op + String(random(0, hardMax+1));}
        }
        _question = quest;
    }
    else {
        _question = "";
    }
}

//Returns the question as a string type.
String Question::getQuestion()
{
    return _question;
}

//Returns the difficulty of this question.
String Question::getDifficulty()
{
    return _difficulty;
}

//Returns the answer to this question.
int Question::getAnswer()
{
    int arrayIndex = 0;
    int prevIndex = 0;
    String splitQuestion[_question.length()];
    for (int index = 0; index < _question.length(); index++)
    {
        if (isOperator(_question[index]))
        {
            if (index != prevIndex)
            {
                splitQuestion[arrayIndex] = _question.substring(prevIndex, index);
                arrayIndex++;
            }
            splitQuestion[arrayIndex] = _question[index];
            arrayIndex++;
            prevIndex = index + 1;
        }
        if (index == _question.length() - 1)
        {
            splitQuestion[arrayIndex] = _question.substring(prevIndex);
        }
    }
    String finalArray[arrayIndex+1];
    subArray(splitQuestion, 0, arrayIndex+1, finalArray);
    String answerStr = calculateArray(finalArray, arrayIndex+1);
    int answer = answerStr.toInt();
    return answer;
}

//Checks if a given input (answer) is the answer to this question.
bool Question::checkAnswer(int answer)
{
    return getAnswer() == answer;
}

String Question::calculateArray(String *array, int length) {
    int index;
    String solved;
    //Reken haakjes uit
    if (length == 1) {
        return array[0];
    }
    else if (positionString(array, length, "(") != -1){
        index = positionString(array, length, "(");
        int backIndex = positionString(array, length, ")");
        String subArr[backIndex-index-1];
        subArray(array, index+1, backIndex, subArr);

        solved = calculateArray(subArr, backIndex-index-1);

        String newArray[length-(backIndex-index)];
        solvedArray(array, length, index, backIndex, solved, newArray);
        return calculateArray(newArray, length-(backIndex-index));
    }
    else if (positionString(array, length, "^") != -1){
        index = positionString(array, length, "^");
        solved = String(round(pow(array[index-1].toInt(),array[index+1].toInt())));

        String newArray[length-2];
        solvedArray(array, length, index-1, index+1, solved, newArray);
        return calculateArray(newArray, length-2);
    }
    else if (positionString(array, length, "*") != -1){
        index = positionString(array, length, "*");
        solved = String(array[index-1].toInt()*array[index+1].toInt());

        String newArray[length-2];
        solvedArray(array, length, index-1, index+1, solved, newArray);
        return calculateArray(newArray, length-2);
    }
    else if (positionString(array, length, "/") != -1){
        index = positionString(array, length, "/");
        solved = String(round(array[index-1].toInt()/array[index+1].toInt()));

        String newArray[length-2];
        solvedArray(array, length, index-1, index+1, solved, newArray);
        return calculateArray(newArray, length-2);
    }
    else if (positionString(array, length, "+") != -1){
        index = positionString(array, length, "+");
        solved = String(array[index-1].toInt()+array[index+1].toInt());

        String newArray[length-2];
        solvedArray(array, length, index-1, index+1, solved, newArray);
        return calculateArray(newArray, length-2);
    }
    else if (positionString(array, length, "-") != -1){
        index = positionString(array, length, "-");
        solved = String(array[index-1].toInt()-array[index+1].toInt());

        String newArray[length-2];
        solvedArray(array, length, index-1, index+1, solved, newArray);
        return calculateArray(newArray, length-2);
    }
    return "NaN";
}

//Returns the first accurance of a string in an array of strings (if the string is ")" it returns the first starting from the back)
int Question::positionString(String *array, int length, String string) {
    if (string == ")"){
        for (int index=length; index > 0; index--){
            if (array[index-1] == string) return index-1;
        }
    }
    else {
        for (int index=0; index < length; index++) {
            if (array[index] == string) return index;
        }
    }
    return -1;
}

bool charInString(String string, char c)
{
    for (int i = 0; i < string.length(); i++)
    {
        if (string[i] == c)
            return true;
    }
    return false;
}

bool Question::isOperator(char c)
{
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
    {
        if (operators[i] == c)
            return true;
    }
    return false;
}

//Puts a subarray from the parrentArray from minIndex (included) to maxIndex (excluded) in childArray
void Question::subArray(String *parentArray, int minIndex, int maxIndex, String *childArray){
    for (int i=0; i < maxIndex-minIndex; i++) {
        childArray[i] = parentArray[minIndex+i];
    }
}

//Replaces part from minIndex (included) to maxIndex (included) with one String
void Question::solvedArray(String *parentArray, int length, int minIndex, int maxIndex, String solved, String *childArray){
    for (int i = 0; i < minIndex; i++) {
        childArray[i] = parentArray[i];
    }
    childArray[minIndex] = solved;
    for (int i = maxIndex+1; i < length;i++){
        childArray[i-maxIndex+minIndex] = parentArray[i];
    }
}