

#include <bits/stdc++.h>
using namespace std;

struct Item {
    double value;
    double weight;
    double density;
    int index;
};

bool cmp(const Item &a, const Item &b) {
    return a.density > b.density;
}

pair<double, vector<double>> fractionalKnapsack(vector<Item> items, double W) {
    for (auto &it : items) it.density = it.value / it.weight;
    sort(items.begin(), items.end(), cmp);

    double remaining = W;
    double totalValue = 0.0;
    vector<double> take(items.size(), 0.0);

    for (size_t i = 0; i < items.size() && remaining > 0; ++i) {
        if (items[i].weight <= remaining) {
            take[i] = 1.0;
            totalValue += items[i].value;
            remaining -= items[i].weight;
        } else {
            double fraction = remaining / items[i].weight;
            take[i] = fraction;
            totalValue += items[i].value * fraction;
            remaining = 0;
            break;
        }
    }
    return {totalValue, take};
}

int main() {
    vector<Item> items = {
        {60, 10, 0.0, 1},
        {100, 20, 0.0, 2},
        {120, 30, 0.0, 3}
    };
    double W = 50;
    auto res = fractionalKnapsack(items, W);
    cout << "Max value = " << res.first << "\n";
    return 0;
}



