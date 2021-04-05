/*****************************************************************************/
/* 1st version: Excluding maximum possibles by getting minimum type variance */
/*****************************************************************************/

#include <array>
#include <bitset>
#include <iostream>
#include <vector>
#include <iomanip>
#include <random>
#include <functional>
#include <limits>

using namespace std;

constexpr int numTypes = 14;
constexpr array<int, numTypes> TYPES = {{0, 1, 2, 3, 4, 10, 11, 12, 13, 20, 21, 22, 30, 40}};
constexpr int TEST_ROUND = 100;
constexpr int minPossibleValue = 123;
constexpr int maxPossibleValue = 9876;
vector<int> possibles;

int getType(int ans, int num);
bool hasRepeatingDigits(int num);
void initializePossibleAnswers(int num, int type);
double getVariance(const array<int, 41>& set);
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
    cout << "-----------------------\n"
         << "1st guess: ";
    cin >> guessNum;
    cout << "Input feedback: ";
    cin >> type;

    initializePossibleAnswers(guessNum, type);
    cout << "Remain possibles: " << possibles.size() << endl
         << "-----------------------"
         << endl;

    while (possibles.size() > 1)
    {
        turn++;
        cout << turn <<
          (turn == 2 ? "nd" :
          (turn == 3 ? "rd" : "th")) <<
          " round" << endl;
        guessNum = getBestNumber();
        cout << "suggestion: " << setfill('0') << setw(4) << guessNum << endl
             << "Input feedback: ";
        cin >> type;
        filterPossiblesAns(guessNum, type);
        cout << "Remain possibles: " << possibles.size() << endl
             << "-----------------------"
             << endl;
    }
    if (possibles.size())
        cout << "Ans: " << setfill('0') << setw(4) << possibles[0] << endl;
    else
        cout << "No possible answers.\n";
    possibles.clear();
}

int getType(int ans, int num)
{
    int type = 0;
    constexpr int numDigits = 4;
    array<int, numDigits> digitsForAns;
    array<int, numDigits> digitsForNum;
    for (int i = 0; i < numDigits; i++)
    {
        digitsForAns[i] = ans % 10;
        digitsForNum[i] = num % 10;
        ans /= 10;
        num /= 10;
    }

    for (int i = 0; i < numDigits; i++) {
        const int digit = digitsForNum[i];
        const auto firstEqualIt = find(digitsForAns.begin(), digitsForAns.end(), digit);
        if (firstEqualIt == digitsForAns.end())
            continue;

        const bool same_digit_pos = i == distance(digitsForAns.begin(), firstEqualIt);
        type += same_digit_pos ? 10 : 1;
    }

    return type;
}

bool hasRepeatingDigits(int num)
{
    bitset<10> usedDigit;
    if (num < 1000)
        usedDigit.set(0);

    while (num) {
        if (usedDigit[num % 10])
            return true;

        usedDigit.set(num % 10);
        num /= 10;
    }
    return false;
}

void initializePossibleAnswers(int num, int type)
{
    for (int i = minPossibleValue; i <= maxPossibleValue; i++) {
        if (hasRepeatingDigits(i))
            continue;

        if (getType(i, num) == type)
            possibles.push_back(i);
    }
}

double getVariance(const array<int, 41>& set)
{
    const double mean = accumulate(TYPES.begin(), TYPES.end(), 0,
                                  [&set](int acc, int type){ return acc + set[type]; })
      / static_cast<double>(numTypes);

    double sum = 0;
    for (const int type : TYPES) {
        const int distanceFromMean = set[type] - mean;
        sum += distanceFromMean * distanceFromMean;
    }

    return sum / static_cast<double>(numTypes-1);
}

int getBestNumber()
{
    double minVariance = numeric_limits<double>::max();
    // Could use std::optional<int> if using C++17
    int bestNumber;

    for (int i = minPossibleValue; i <= maxPossibleValue; i++)
    {
        if (hasRepeatingDigits(i))
            continue;

        array<int, 41> typeFrequency{};
        for (const int possibility : possibles)
            typeFrequency[getType(i, possibility)]++;
        const double variance = getVariance(typeFrequency);
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
    // Erase-remove idiom performs an in-place filtering of elements according to a predicate function.
    // No need for a copy as a result.
    possibles.erase(remove_if(possibles.begin(), possibles.end(),
                    [guessNum, type](const int possibility){ return getType(possibility, guessNum) != type; }),
        possibles.end());
}

void startTest()
{
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(minPossibleValue, maxPossibleValue);
    auto dice = std::bind(distribution, generator);
    int count = 0;
    int ans, firstGuess, round;
    for (int i = 0; i < TEST_ROUND; i++)
    {
        while (hasRepeatingDigits(ans = dice()));
        while (hasRepeatingDigits(firstGuess = dice()));
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
    initializePossibleAnswers(guessNum, type);

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
