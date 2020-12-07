
#include <iostream>
#include <unordered_map>
#include <map>
#include <string>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <algorithm> 
#include <chrono> 
using namespace std;
using namespace std::chrono;

class MoneyBot {

    
    //this unordered map takes in a the daily percentage change of a stock as a float (we round to one digit past the decimal)
    unordered_map<float, int> plusMinusData;

    // [0] keeps count of the number of positive days 
    // [1] keeps count of the number of negative days
    // [2] keeps count of the number of neutral days (unlikely for there to be 0.0% change in a day)
    int counter[3] = { 0, 0, 0 };

    //these variables allow us to get the current price difference from the first day called to current day
    float firstOpeningPrice = 0.0f;
    float currentPriceDifference = 0.0f;
    bool firstEntry = true;
    

    public:
        
        //inserts data points into the class data structure and variables
        void insert(float openingPrice, float closingPrice);
        
        //determines whether to buy or not based on pre-set arbitrary conditions
        void doWeBuy();

        //useful for debuging
        void debug();
};

void MoneyBot::insert(float openingPrice, float closingPrice) {

    //calculates the percent change for a specific date
    float val = ((closingPrice / openingPrice) - 1.0f) * 100.0f;
    
    //rounds to the nearest tenths place by multiplying float by 10, making an int, then dividing by ten and making it a float
    float b = val * (10.0f);
    int c = b;
    float d = c / 10.0f;
    
    //inserts the percent change into the map to keep count
    plusMinusData[d]++;

    //put the change in its respective index
    //positive = 0, negative = 1, neutral = 2
    if (val > 0.0f) {
        counter[0]++;
    }
    else if (val < 0.0f) {
        counter[1]++;
    }
    else {
        counter[2]++;
    }

    //set opening price and calculate the final difference
    if (this->firstEntry == true) {

        firstOpeningPrice = openingPrice;
        this->firstEntry = false;
    }
    else {

        this->currentPriceDifference = closingPrice - this->firstOpeningPrice;
    }

}

void MoneyBot::doWeBuy() {
    
    //calculates the sum of the all the plus minuses
    float finalSumOfDailyPlusMinus = 0.0f;

    for (auto it = plusMinusData.begin(); it != plusMinusData.end(); it++) {

        finalSumOfDailyPlusMinus = finalSumOfDailyPlusMinus + (it->first * ((float)it->second));
    }
    
    //these conditions I have picked are entirely arbitrary and chosen because I think they are the conditions that would
    //warrant a good time to buy
    if (finalSumOfDailyPlusMinus > 5.0f && currentPriceDifference > 25.0f && ( counter[0] + counter[2] > counter[1] )) {

        cout << "MoneyBot says to buy this stock!\n";
    }
    else {

        cout << "MoneyBot says to not buy this stock.\n";
    }
}

void MoneyBot::debug() {

    /*for (auto it = plusMinusData.begin(); it != plusMinusData.end(); it++) {
        cout << it->first << " : " << it->second << endl;
    }

    cout << endl;*/

    for (int i = 0; i < 3; i++) {
        
        if (i == 0) {
            cout << "positive : ";
        }
        if (i == 1) {
            cout << "negative : ";
        }
        if (i == 2) {
            cout << "neutral : ";
        }

        cout << counter[i] << endl;
    }

    cout << "currentPriceDifference : " << currentPriceDifference << endl;
}

int main() {

    MoneyBot bot;


    string userInput;

    ifstream tickerFile("tickers.csv");

    string stockName;
    string stockTicker;

    //stock name, stock ticker
    map<string, string> stocks;

    cout << "What stock shall MoneyBot help you with today?" << endl << endl;
    cout << "----------------------------------------------" << endl;
    cout << "****Please select one of the TICKERS below****" << endl;
    cout << "----------------------------------------------" << endl << endl;

    while (tickerFile.good()) {

        getline(tickerFile, stockName, ',');
        getline(tickerFile, stockTicker, '\n');

        //cout << stockName << ":" << stockTicker << endl;
        stocks[stockTicker] = stockName;
    }

    tickerFile.close();

    //goes from 0-29
    int index = 1;
    for (auto it = stocks.begin(); it != stocks.end(); it++) {
        cout << it->second;
        if (index % 5 == 0) {
            cout << "(" << it->first << ")" << endl << endl;
        }
        else {
            cout << "(" << it->first << ")" << ", ";
        }
        index++;
    }

    bool go = true;

    while (go) {
        cin >> userInput;

        for (int i = 0; i < userInput.length(); i++) {
            userInput[i] = toupper(userInput[i]);
        }

        if (stocks[userInput] == "") {

            cout << "Ticker selected does not exist. Please try again\n";
        }
        else {
            
            go = false;
        }
    }

    string temp = "stockdata/" + userInput + ".csv";

    cout << "You chose, " << stocks[userInput] << endl << endl;

    ifstream myFile(temp);
    //MCD has a crazy final sumDailyplusminus

    string date;
    string open;
    string high;
    string low;
    string close;
    string adjClose;
    string volume;
    
    cout << "MoneyBot is thinking...\n";

    //to get the time it takes to run program
    auto start = high_resolution_clock::now();

    index = 0;
    while (myFile.good()) {

        
        getline(myFile, date, ',');
        getline(myFile, open, ',');
        getline(myFile, high, ',');
        getline(myFile, low, ',');
        getline(myFile, close, ',');
        getline(myFile, adjClose, ',');
        getline(myFile, volume, '\n');

        if (index > 0 && open != "null") {
            //cout << "Date : " << date << ", Open : " << open << ", Close : " << close << endl;
            bot.insert(stof(open), stof(close));
        }
        
        //cout << index << endl;
        index++;
    }
    
    myFile.close();

    bot.doWeBuy();
    
    //calculate time elapsed and print result
    auto end = high_resolution_clock::now();
    duration<float> duration = end - start;
    cout << "\nIt took MoneyBot " << duration.count() << " seconds to think for you.\n";

    return 0;
}