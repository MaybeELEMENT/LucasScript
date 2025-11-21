#include <iostream>
using namespace std;

const double MARKUP_RATE = 0.60; // 60% markup rate

double readCostPrice() {
    double costPrice;
    cout << "Enter the cost price of the item (RM): ";
    cin >> costPrice;
    return costPrice;
}

double calculateSellingPrice(double costPrice) {
    return costPrice * (1 + MARKUP_RATE);
}

void displaySellingPrice(double sellingPrice) {
    cout << "The selling price of the item is: RM " << sellingPrice << endl;
}

int main() {
    double costPrice = readCostPrice();
    double sellingPrice = calculateSellingPrice(costPrice);
    displaySellingPrice(sellingPrice);
    return 0;
}