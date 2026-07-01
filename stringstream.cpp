#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
vector<string> tokenize(string query){
    stringstream ss(query);
    vector<string> tokens;
    string word;
    while(ss>>word){
        tokens.push_back(word);
    }
    return tokens;
}
int main(){
    vector<string> tokens = tokenize("INSERT INTO students VALUES Amol 21");
    for(string t: tokens){
        cout<<t<<"\n";
    }
    return 0;
}