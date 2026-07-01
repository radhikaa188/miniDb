#include<iostream>
using namespace std;
int main() {
    try {
        cout << "Before throw\n";
        throw runtime_error("Something went wrong!");
        cout << "After throw\n";   // ye kabhi nahi chalega
    } catch (exception& e) {
        cout << "Caught: " << e.what() << "\n";
    }

    cout << "Program continues normally\n";
    return 0;
}