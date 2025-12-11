#include <iostream>
using namespace std;

int getInt(char const* const prompt = "") {
    cout << prompt;
    int result; 
    cin >> result;
    return result;
}

double getDouble(char const* const prompt = "") {
    cout << prompt;
    double result;
    cin >> result;
    return result;
}
struct Account {
    double balance;

    void add(double const amount){
        balance += amount;
    }
    void remove(double const amount){
        balance -= amount;
    }
};
struct Deposit : Account {
    double rate;

    void update(){
        balance += rate / 12.0 * (1.0 / 100.0) * balance;
    }
};

struct LongDeposit : Deposit {
    int duration;                   // кол-во месяцев вклада 
    double rejectAmount = 0;        // сумма отказанных операций 
    int rejectCount = 0;            // кол-во отклоненных операций 

    void remove(double const amount){
        if (duration > 0) {
            rejectAmount += amount;
            ++rejectCount;
        }
        else {
            Deposit::remove(amount);    
        }
        balance -= amount;
    }
    void update() {
        if (duration == 0) {
            return;
        }
        Deposit::update();
        --duration; 
    }
};


struct System {
    Account card;
    Deposit deposit1;
    Deposit deposit2;
    LongDeposit longDeposit;
    int monthsCount;
    int monthsDone = 0;         // кол-во пройденных месяцев


    void update(){
        deposit1.update();
        deposit2.update();
        longDeposit.update();
        ++monthsDone;
    }
    double total(){
        return card.balance + deposit1.balance + deposit2.balance + longDeposit.balance;
    }
};

int main(){ 
    System system;
    system.card.balance = 1000;
    system.deposit1.balance = 2000;
    system.deposit2.balance = 3000;
    system.longDeposit.balance = 5000;
    system.deposit1.rate = 6;
    system.deposit2.rate = 7;
    system.longDeposit.rate = 8;
    system.longDeposit.duration = 6;


    system.monthsCount = getInt("Months count: ");
    for (int i = 0; i < system.monthsCount; ++i) {
        system.card.add(getDouble("Money to add: "));
        system.card.remove(getDouble("Money to remove: "));
        system.deposit1.add(getDouble("Money to add: "));
        system.deposit1.remove(getDouble("Money to remove: "));
        system.deposit2.add(getDouble("Money to add: "));
        system.deposit2.remove(getDouble("Money to remove: "));
        system.longDeposit.add(getDouble("Money to add: "));
        system.longDeposit.remove(getDouble("Money to remove: "));
        system.update();
        cout << system.monthsDone << '\n';
        cout << system.card.balance << ' ' << system.deposit1.balance << ' ' << system.deposit2.balance << ' ' << system.longDeposit.balance << '\n';
        cout << system.total() <<'\n';   
        cout << system.longDeposit.rejectCount << '\n';
        cout << system.longDeposit.rejectAmount << '\n';
    }
}


//100 = 100 + 24% / 12 * 100 = 100 + 2% * 100 = 100 + 2 * % * 100 = 100 + 2 * (1 / 100) * 100 = 100 + 2 * 1 = 102






// Пример входных данных
	
// Пример выходных данных
// 1000 2000 3000 5000
// 6 7 8
// 6
// 4
// 200 100 300 100 400 200 500 100
// 150 100 200 50 300 100 400 200
// 100 50 100 50 100 50 100 50
// 50 20 50 20 50 20 50 20
	
// 4
// 1181.44 2374.73 3624.47 6081.54
// 13262.2
// 4
// 1400