#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <unordered_map>
using namespace std;

//helper function to check if a string (from the token list) is a valid number or not (integer or decimal OR operator).
bool is_number(const string& s){   //read the string character by character, counting dots and checking for digits.
    if (s.empty()) return false;
    int dots= 0; //counts the decimal points
    int start= 0; // starts checking characters from the beginning of the string, but if the first character is a minus sign, it starts from the next character.
    if (s[0]== '-') {
        if (s.size()== 1) return false;
        start= 1; // else skip the minus when its not just a minus sign.
    }
    for (int i = start; i< s.size(); i++){
        if (s[i] == '.'){
            dots++;
            if (dots> 1) return false; //a number can have ONLY ONE decimal point. 
        } else if (!isdigit(s[i])){ // if the character is not a digit, then it's not a valid number.
            return false;
        }
    }
    return true;
}
//convert string to vector of tokens.
vector<string> tokenize(string query){
    if (query.empty()){
        throw invalid_argument("Empty expression");
    }

    for (char c: query){
        if (isspace(c)){
            throw invalid_argument("Invalid expression");
        }
    }

    vector<string> tokens;
    string number= ""; // building a number character by character
    size_t i= 0;
// go through every character one at a time and check if it's a digit, a decimal point, an operator, or part of a function name.
    while (i< query.length()){
        char ch= query[i];
        if (isdigit((unsigned char)ch) || ch == '.'){
            number += ch;
            i++;
            continue;
        }
        if (query.substr(i, 11) == "reciprocal("){
            if (!number.empty()){
                tokens.push_back(number); //Before adding it, flush any number we were building..

                number = "";
            }
            tokens.push_back("reciprocal");
            tokens.push_back("(");
            i += 11;
            continue;
        }
        if (query.substr(i, 6) == "recip("){
            if (!number.empty()){
                tokens.push_back(number);
                number= "";
            }
            tokens.push_back("reciprocal");
            tokens.push_back("(");
            i += 6;
            continue;
        }
        //flush Number Before Operator
        if (!number.empty()){
            tokens.push_back(number);
            number= "";
        }
        // Handle unary minus: if the '-' is at the start of the expression or follows an operator or '(', it's unary.
        if (ch == '-'){
            bool unary= false;
            if (i == 0) {
                unary= true;
            } else if (!tokens.empty()){
                string last = tokens.back();
                if (last== "(" || last == "+" || last == "-" ||
                    last== "*" || last == "/" || last == "%" ||
                    last == "^" || last == "~") {
                    unary = true;
                }
            }
            if (unary){
                i++; //skip the '-' and check if it's followed by a parenthesis or a function.
                if (i < query.length() && query[i]== '('){
                    tokens.push_back("~");
                    continue;
                }
                if (query.substr(i, 11) == "reciprocal(" || query.substr(i, 6)== "recip(") {
                    tokens.push_back("~");
                    continue;
                }
// If it's a unary minus followed by a number, we need to read the number and store it as a single token with the minus sign.
                string negNum = "";
                while (i < query.length() &&
                       (isdigit((unsigned char)query[i]) || query[i] == '.')){
                    negNum += query[i];
                    i++;
                }

                if (negNum.empty()){
                    throw invalid_argument("Invalid expression");
                }

                tokens.push_back("-" + negNum);
                continue;
            }

            tokens.push_back("-");
            i++;
            continue;
        }
// If it's an operator or a parenthesis, add it as a token.
        if (ch == '+' || ch == '*' || ch == '/' || ch == '%' ||
            ch == '^' || ch == '(' || ch == ')') {
            tokens.push_back(string(1, ch));
            i++;
            continue;
        }

        throw invalid_argument(string("Invalid character: ") + ch);
    }

    if (!number.empty()){
        tokens.push_back(number);
    }

    return tokens;
}

// Handle implicit multiplication: if a number or a closing parenthesis is followed by a number, an opening parenthesis, or a function, we insert a multiplication operator between them.
vector<string> add_implicit_multiplication(const vector<string>& tokens){
    vector<string> out;
    for (size_t i = 0; i < tokens.size(); i++){
        out.push_back(tokens[i]);
        if (i + 1 >= tokens.size()) continue;

        string a= tokens[i]; //current token
        string b= tokens[i + 1]; //next token
//checkss if we need to insert a multiplication operator between a and b..
        bool left_ok  =  is_number(a) || a == ")";
        bool right_ok  =  is_number(b) || b == "(" || b == "reciprocal";
//when both conditions true, insert a * b/w them 
        if (left_ok && right_ok) {
            out.push_back("*");
        }
    }
    return out;
}

//INFIX TO POSTFIX ALGORITH :- THE SHUNTING YARD
vector<string> token_to_postfix(const vector<string>& tokens){
    if (tokens.empty()) {
        throw invalid_argument("no tokens to be converted to postfix");
    }
//the dictionary for mathematical precendace of operators..
    unordered_map<string, int> precedence ={
        {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"%", 2},
        {"^", 3}, {"~", 4}, {"reciprocal", 4} //highest bec they are UNARY
    };

    vector<string> stack; //operator stack
    vector<string> out; //postfix list of output
//no. go directly to output here
    for (const string& t : tokens) {
        if (is_number(t)) {
            out.push_back(t);
            continue;
        }
//openieng bracket always goes to stack and never to output
        if (t == "(") {
            stack.push_back(t);
            continue;
        }
//pop everything from the stack to output until we find (
        if (t == ")"){
            while (!stack.empty() && stack.back() != "(") {
                out.push_back(stack.back());
                stack.pop_back();
            }
            //If stack is empty before finding ( → extra ) in input → error

            if (stack.empty()) {
                throw invalid_argument("Mismatched parentheses: extra ')'");
            }
            stack.pop_back(); // remove the "(" itself
// check wether after closing bracket there's a ~ or reciprocal sitting on stack
            if (!stack.empty() && (stack.back() == "~" || stack.back() == "reciprocal")) {
                out.push_back(stack.back()); //yes then  pop it to output too

                stack.pop_back();
            }
            continue;
        }

        if (precedence.count(t)){  //precedence.count(t) checks if t is in our precedence map (is it an operator?)
            bool right_assoc = (t == "^" || t == "~" || t == "reciprocal"); //this means right associative- 2^3^2 = 2^(3^2) not (2^3)^2

            //While stack has operators of higher/equal priority..  pop them to output first
            while (!stack.empty() && stack.back() != "(" && precedence.count(stack.back())) {
                int p_top= precedence[stack.back()]; // priority of top of stack
                int p_cur= precedence[t];  // priority of current token
                bool should_pop = (!right_assoc && p_top >= p_cur) || (right_assoc && p_top > p_cur);
                if (!should_pop) break;
                out.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(t);
            continue;
        }

        throw invalid_argument("Invalid token: " + t);
    }
// After all tokens processed then dump remaining stack to output

    while (!stack.empty()){
        if (stack.back() == "("){  // If ( found, it means stack was never closed, hence error
            throw invalid_argument("Mismatched parentheses: extra '('");
        }
        out.push_back(stack.back());
        stack.pop_back();
    }

    return out;
}
//MATH
double calc(double a, double b, const string& op){
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("cannot divide by zero");
        return a / b;
    }
    if (op == "%"){
        if (b == 0) throw runtime_error("cannot modulo by zero");
        return fmod(a, b);
    }
    if (op== "^") return pow(a, b);
    throw invalid_argument("Unknown operator: "+op);
}

//Takes postfix list and calculates final answer using a stack:

double evaluation(const vector<string>& postfix)   {
    if (postfix.empty()){
        throw invalid_argument("empty postfix expresssion");
    }

    vector<double> stack;

    for (const string& p : postfix) { //numbers are push directly onto stack
        if (is_number(p)) {
            stack.push_back(stod(p));
            continue;
        }
//negate.. just flip sign of top of stack
        if (p == "~") {
            if (stack.empty()) throw invalid_argument("insufficient operands for unary minus");
            stack.back()= -stack.back(); //peek at top without popping
            continue;
        }
//Pop top number, compute 1/x, push result back
        if (p == "reciprocal"){
            if (stack.empty()) throw invalid_argument("Insufficient operands for reciprocal");
            double x = stack.back();
            if (x ==0) throw runtime_error("cannot take reciprocal of zero");
            stack.back() = 1.0 / x;
            continue;
        }
//Binary operator- needs TWO numbers from stack
        if (p== "+" || p == "-" || p == "*" || p== "/" || p =="%" || p == "^"){
            if (stack.size() < 2) throw invalid_argument("Insufficient operands");
            double b = stack.back(); stack.pop_back();
            double a = stack.back(); stack.pop_back();
            stack.push_back(calc(a, b, p));
            continue;
        }

        throw invalid_argument("Invalid token in postfix: " + p);
    }

    if (stack.size()!= 1) {
        throw invalid_argument("invalid expression");
    }
    return stack.back();
}

double evaluate(const string& query){
    vector<string> tokens = tokenize(query);
    tokens = add_implicit_multiplication(tokens);
    vector<string> postfix = token_to_postfix(tokens);
    return evaluation(postfix);
}

int main(){
    string query;
    getline(cin, query);
    try {
        cout<< evaluate(query) << "\n";
    } catch (const exception& e){
        cout<< e.what() << "\n";
    }
    return 0;
}