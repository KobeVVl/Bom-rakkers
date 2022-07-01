#ifndef Question_h
#define Question_h

#include "Arduino.h"

#ifndef HARD
#define HARD "hard"
#endif
#ifndef MEDIUM
#define MEDIUM "medium"
#endif
#ifndef EASY
#define EASY "easy"
#endif

class Question
{
private:
    String _difficulty;
    String _question;

    bool isOperator(char c);
    String calculateArray(String *array, int length);
    int positionString(String *array, int length, String string);
    void subArray(String *parentArray, int minIndex, int maxIndex, String *childArray);
    void solvedArray(String *parentArray, int length, int minIndex, int maxIndex, String solved, String *childArray);
public:
    Question(String diff, String quest);
    Question(String diff);
    Question();
    int getAnswer();
    bool checkAnswer(int answer);
    String getQuestion();
    String getDifficulty();
};

#endif