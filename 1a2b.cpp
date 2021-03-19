/*****************************************************************************/
/* 1st version: Excluding maximum possibles by getting minimum type variance */
/*****************************************************************************/

#include <iostream>
#include <vector>
#include <iomanip>
#include <random>
#include <functional>

using namespace std;

const int TYPES[14] = {0, 1, 2, 3, 4, 10, 11, 12, 13, 20, 21, 22, 30, 40};
const int TEST_ROUND = 100;
vector<int> possibles;

int getType(int ans, int num);
bool checkRepeat(int num);
void getPossibleAns(int num, int type);
double getVariance(int set[]);
int getBestNumber();
void filterPossiblesAns(int guessNum, int type);
int testGame(int ans, int init);
void startGame();
void startTest();

int main(int argc, char *argv[])
{
    if (argc > 1 && string(argv[1]) == "test")
        startTest();
    else
        for (int flag = 0; flag != 120; flag = cin.get())
        {
            startGame();
            cout << "Enter \"x\" to leave" << endl;
            cin.ignore();
        }
    return 0;
}

void startGame()
{
    int guessNum, type;
    int turn = 1;
    cout << "-----------------------"
         << endl
         << "1th guess: ";
    cin >> guessNum;
    cout << "Input feedback: ";
    cin >> type;

    getPossibleAns(guessNum, type);
    cout << "Remain possibles: " << possibles.size() << endl
         << "-----------------------"
         << endl;

    while (possibles.size() > 1)
    {
        cout << ++turn << "th round" << endl;
        guessNum = getBestNumber();
        cout << "suggestion: " << setfill('0') << setw(4) << guessNum << endl
             << "Input feedback: ";
        cin >> type;
        filterPossiblesAns(guessNum, type);
        cout << "Remain possibles: " << possibles.size() << endl
             << "-----------------------"
             << endl;
    }
    cout << "Ans: " << setfill('0') << setw(4) << possibles[0] << endl;
    possibles.clear();
}

int getType(int ans, int num)
{
    int type = 0;
    int ansDigits[4];
    int numDigits[4];
    for (int i = 0; i < 4; i++)
    {
        ansDigits[i] = ans % 10;
        numDigits[i] = num % 10;
        ans /= 10;
        num /= 10;
    }

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (numDigits[i] == ansDigits[j])
            {
                if (i == j)
                    type += 10;
                else
                    type += 1;
                break;
            }
    return type;
}

bool checkRepeat(int num)
{
    int digits[10] = {0};
    if (num < 1000)
        digits[0]++;

    while (num)
        if (digits[num % 10] == 1)
            return true;
        else
        {
            digits[num % 10]++;
            num /= 10;
        }
    return false;
}

void getPossibleAns(int num, int type)
{
    for (int i = 123; i <= 9876; i++)
        if (checkRepeat(i))
            continue;
        else if (getType(i, num) == type)
            possibles.push_back(i);
}

double getVariance(int set[])
{
    double mean;
    double sum = 0;
    for (int i = 0; i < 14; i++)
        sum += set[TYPES[i]];
    mean = sum / 14;
    sum = 0;
    for (int i = 0; i < 14; i++)
        sum += (set[TYPES[i]] - mean) * (set[TYPES[i]] - mean);
    return sum / 14;
}

int getBestNumber()
{
    double minVariance = 9999999;
    double variance;
    int bestNumber = 9999;

    for (int i = 123; i <= 9876; i++)
    {
        if (checkRepeat(i))
            continue;
        int typeCounts[41] = {0};
        for (vector<int>::iterator it = possibles.begin(); it != possibles.end(); it++)
            typeCounts[getType(i, *it)]++;
        variance = getVariance(typeCounts);
        if (variance < minVariance)
        {
            bestNumber = i;
            minVariance = variance;
        }
    }
    return bestNumber;
}

void filterPossiblesAns(int guessNum, int type)
{
    vector<int> newPossibles;
    for (vector<int>::iterator it = possibles.begin(); it != possibles.end(); it++)
        if (getType(*it, guessNum) == type)
            newPossibles.push_back(*it);
    possibles = newPossibles;
}

void startTest()
{
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(123, 9876);
    auto dice = std::bind(distribution, generator);
    int count = 0;
    int ans, firstGuess, round;
    for (int i = 0; i < TEST_ROUND; i++)
    {
        while (checkRepeat(ans = dice()));
        while (checkRepeat(firstGuess = dice()));
        round = testGame(ans, firstGuess);
        count += round;
        cout << "(Ans, FirstGuess, Round) = (" << setfill('0') << setw(4) << ans << ", " << setfill('0') << setw(4) << firstGuess << ", " << round << ")" << endl;
    }
    cout << "-------------------------------------------" << endl
         << "Average: " << (double)count / (double)TEST_ROUND << " rounds" << endl;
}

int testGame(int ans, int guessNum)
{
    int turn = 1;
    int type = getType(ans, guessNum);
    getPossibleAns(guessNum, type);

    while (type != 40)
    {
        turn++;
        guessNum = getBestNumber();
        type = getType(ans, guessNum);
        filterPossiblesAns(guessNum, type);
        if (possibles.size() == 1)
            break;
    }
    possibles.clear();
    return turn+1;
}