#include <iostream>
using namespace std;

int getInt(char const* const prompt = "") {
    //cout << prompt;
    int result; 
    cin >> result;
    return result;
}

double getDouble(char const* const prompt = "") {
    //cout << prompt;
    double result;
    cin >> result;
    return result;
}
struct Account {
    double balance;

    void add(double const amount){
        balance += amount;
    }
    bool remove(double const amount){
        balance -= amount;
        if (balance < 0) {
            return true;
        }
        return false;
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

    bool remove(double const amount){
        if (duration > 0) {
            rejectAmount += amount;
            ++rejectCount;
            return false;
        }
        else {
            return Deposit::remove(amount);    
        }
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
    int depositCount = 0;       // кол-во обработанных счетов
    bool hasError = false;

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
    system.card.balance = getDouble("Введите начальный баланс банковской карты (продукт 1): ");
    system.deposit1.balance = getDouble("Введите начальный накопительный счёт a (продукт 2): ");
    system.deposit2.balance = getDouble("Введите начальный накопительный счёт b (продукт 3): ");
    system.longDeposit.balance = getDouble("Введите начальный баланс вклада (продукт 4): ");

    system.deposit1.rate = getDouble("Введите процент годовой ставки накопительного счёта a (продукт 2): ");
    system.deposit2.rate = getDouble("Введите процент годовой ставки накопительного счёта b (продукт 2): ");
    system.longDeposit.rate = getDouble("Введите процент годовой ставки накопительного вклад (продукт 4)");

    system.longDeposit.duration = getDouble("Введите положительное целое число — срок вклада (продукт 4) в месяцах");


    system.monthsCount = getInt("Months count: ");
    for (int i = 0; i < system.monthsCount; ++i) {
        system.depositCount = 0;
        system.card.add(getDouble("Money to add: "));
        system.hasError = system.card.remove(getDouble("Money to remove: "));
        if (system.hasError) {
            break;
        }
        ++system.depositCount;
        system.deposit1.add(getDouble("Money to add: "));
        system.hasError = system.deposit1.remove(getDouble("Money to remove: "));
        if (system.hasError) {
            break;
        }
        ++system.depositCount;
        system.deposit2.add(getDouble("Money to add: "));
        system.hasError = system.deposit2.remove(getDouble("Money to remove: "));
        if (system.hasError) {
            break;
        }
        ++system.depositCount;
        system.longDeposit.add(getDouble("Money to add: "));
        system.hasError = system.longDeposit.remove(getDouble("Money to remove: "));
        if (system.hasError) {
            break;
        }
        ++system.depositCount;
        system.update();
    }
    if (system.hasError) {
        cout << (system.monthsDone + 1) << '\n';
        cout << (system.depositCount + 1) << '\n';
        cout << system.monthsDone << '\n';
        cout << system.card.balance << ' ' << system.deposit1.balance << ' ' << system.deposit2.balance << ' ' << system.longDeposit.balance << '\n';
        cout << system.longDeposit.rejectCount << '\n';
        cout << system.longDeposit.rejectAmount << '\n';
    }
    else {
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