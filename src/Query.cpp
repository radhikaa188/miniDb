#include "../include/Query.h"

namespace Query{
    vector<string> tokenize(string query){
        vector<string> tokens;
        string current = "";
        for(int i=0; i<query.size(); i++){
            char c = query[i];
            if(c== ' '){
                if(current!=""){
                    tokens.push_back(current);
                    current="";
                }
            }else if(c == ',' || c == ')' || c == '('){
                if(current!=""){
                    tokens.push_back(current);
                    current="";
                }
                tokens.push_back(string(1,c));
            }else{
                current+=c;
            }
        }
        if(current!=""){
            tokens.push_back(current);
        }
        return tokens;
    }
}