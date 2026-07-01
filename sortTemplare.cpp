#include<iostream>
#include<vector>
using namespace std;
template <typename T>
void simpleSort(vector<T>& items, bool (*comparator)(T, T)) {
    for (int i = 0; i < items.size(); i++) {
        for (int j = 0; j < items.size() - 1 - i; j++) {
            if (comparator(items[j], items[j+1])) {
                swap(items[j], items[j+1]);
            }
        }
    }
}
bool ascending(int a, int b) {
    return a > b;   // agar 'a' 'b' se bada hai, swap karo (chhota pehle aana chahiye)
}
int main(){
    vector<int> nums = {5, 3, 8, 1, 4};
    simpleSort(nums, ascending);

    for (int n : nums) {
        cout << n << " ";
    }
    return 0;
}