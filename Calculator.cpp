#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <unordered_map>
using namespace std;

bool is_number(const string& s) {
    if (s.empty()) return false;
    int dots = 0;
    int start = 0;
    if (s[0] == '-') {
        if (s.size() == 1) return false;
        start = 1;
    }
    for (int i = start; i < static_cast<int>(s.size()); i++) {
        if (s[i] == '.') {
            dots++;
            if (dots > 1) return false;
        } else if (!isdigit(static_cast<unsigned char>(s[i]))) {
            return false;
        }
    }
    return true;
}

vector<string> tokenize(string query) {
    if (query.empty()) {
        throw invalid_argument("Empty expression");
    }

    bool allSpaces = true;
    for (char c : query) {
        if (!isspace(static_cast<unsigned char>(c))) {
            allSpaces = false;
            break;
        }
    }
    if (allSpaces) {
        throw invalid_argument("Empty expression");
    }

    string clean = "";
    for (char c : query) {
        if (!isspace(static_cast<unsigned char>(c))) {
            clean += c;
        }
    }

    vector<string> tokens;
    string number = "";
    int i = 0;

    while (i < static_cast<int>(clean.length())) {
        char ch = clean[i];

        if (isdigit(static_cast<unsigned char>(ch)) || ch == '.') {
            number += ch;
            i++;
            continue;
        }

        if (clean.substr(i, 11) == "reciprocal(") {
            if (!number.empty()) {
                tokens.push_back(number);
                number = "";
            }
            tokens.push_back("reciprocal");
            tokens.push_back("(");
            i += 11;
            continue;
        }
        if (clean.substr(i, 6) == "recip(") {
            if (!number.empty()) {
                tokens.push_back(number);
                number = "";
            }
            tokens.push_back("reciprocal");
            tokens.push_back("(");
            i += 6;
            continue;
        }

        if (!number.empty()) {
            tokens.push_back(number);
            number = "";
        }

        if (ch == '-') {
            bool unary = false;
            if (i == 0) {
                unary = true;
            } else if (!tokens.empty()) {
                string last = tokens.back();
                if (last == "(" || last == "+" || last == "-" ||
                    last == "*" || last == "/" || last == "%" ||
                    last == "^" || last == "~") {
                    unary = true;
                }
            }

            if (unary) {
                i++;
                if (i < static_cast<int>(clean.length()) && clean[i] == '(') {
                    tokens.push_back("~");
                    continue;
                }

                if (clean.substr(i, 11) == "reciprocal(" || clean.substr(i, 6) == "recip(") {
                    tokens.push_back("~");
                    continue;
                }

                string negNum = "";
                while (i < static_cast<int>(clean.length()) &&
                       (isdigit(static_cast<unsigned char>(clean[i])) || clean[i] == '.')) {
                    negNum += clean[i];
                    i++;
                }

                if (negNum.empty()) {
                    throw invalid_argument("Invalid expression");
                }

                tokens.push_back("-" + negNum);
                continue;
            }

            tokens.push_back("-");
            i++;
            continue;
        }

        if (ch == '+' || ch == '*' || ch == '/' || ch == '%' ||
            ch == '^' || ch == '(' || ch == ')') {
            tokens.push_back(string(1, ch));
            i++;
            continue;
        }

        throw invalid_argument(string("Invalid character: ") + ch);
    }

    if (!number.empty()) {
        tokens.push_back(number);
    }

    return tokens;
}

vector<string> add_implicit_multiplication(const vector<string>& tokens) {
    vector<string> out;
    for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
        out.push_back(tokens[i]);
        if (i + 1 >= static_cast<int>(tokens.size())) continue;

        string a = tokens[i];
        string b = tokens[i + 1];

        bool left_ok = is_number(a) || a == ")";
        bool right_ok = is_number(b) || b == "(" || b == "reciprocal";

        if (left_ok && right_ok) {
            out.push_back("*");
        }
    }
    return out;
}

vector<string> token_to_postfix(const vector<string>& tokens) {
    if (tokens.empty()) {
        throw invalid_argument("No tokens to convert");
    }

    unordered_map<string, int> precedence = {
        {"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"%", 2},
        {"^", 3}, {"~", 4}, {"reciprocal", 4}
    };

    vector<string> stack;
    vector<string> out;

    for (const string& t : tokens) {
        if (is_number(t)) {
            out.push_back(t);
            continue;
        }

        if (t == "(") {
            stack.push_back(t);
            continue;
        }

        if (t == ")") {
            while (!stack.empty() && stack.back() != "(") {
                out.push_back(stack.back());
                stack.pop_back();
            }
            if (stack.empty()) {
                throw invalid_argument("Mismatched parentheses: extra ')'");
            }
            stack.pop_back();
            if (!stack.empty() && (stack.back() == "~" || stack.back() == "reciprocal")) {
                out.push_back(stack.back());
                stack.pop_back();
            }
            continue;
        }

        if (precedence.count(t)) {
            bool right_assoc = (t == "^" || t == "~" || t == "reciprocal");
            while (!stack.empty() && stack.back() != "(" && precedence.count(stack.back())) {
                int p_top = precedence[stack.back()];
                int p_cur = precedence[t];
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

    while (!stack.empty()) {
        if (stack.back() == "(") {
            throw invalid_argument("Mismatched parentheses: extra '('");
        }
        out.push_back(stack.back());
        stack.pop_back();
    }

    return out;
}

double calc(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("Cannot divide by zero");
        return a / b;
    }
    if (op == "%") {
        if (b == 0) throw runtime_error("Cannot modulo by zero");
        return fmod(a, b);
    }
    if (op == "^") return pow(a, b);
    throw invalid_argument("Unknown operator: " + op);
}

double evaluation(const vector<string>& postfix) {
    if (postfix.empty()) {
        throw invalid_argument("Empty postfix expression");
    }

    vector<double> stack;

    for (const string& p : postfix) {
        if (is_number(p)) {
            stack.push_back(stod(p));
            continue;
        }

        if (p == "~") {
            if (stack.empty()) throw invalid_argument("Insufficient operands for unary minus");
            stack.back() = -stack.back();
            continue;
        }

        if (p == "reciprocal") {
            if (stack.empty()) throw invalid_argument("Insufficient operands for reciprocal");
            double x = stack.back();
            if (x == 0) throw runtime_error("Cannot take reciprocal of zero");
            stack.back() = 1.0 / x;
            continue;
        }

        if (p == "+" || p == "-" || p == "*" || p == "/" || p == "%" || p == "^") {
            if (stack.size() < 2) throw invalid_argument("Insufficient operands");
            double b = stack.back(); stack.pop_back();
            double a = stack.back(); stack.pop_back();
            stack.push_back(calc(a, b, p));
            continue;
        }

        throw invalid_argument("Invalid token in postfix: " + p);
    }

    if (stack.size() != 1) {
        throw invalid_argument("Invalid expression");
    }
    return stack.back();
}

double evaluate(const string& query) {
    vector<string> tokens = tokenize(query);
    tokens = add_implicit_multiplication(tokens);
    vector<string> postfix = token_to_postfix(tokens);
    return evaluation(postfix);
}

int main() {
    string query;
    getline(cin, query);
    try {
        cout << evaluate(query) << "\n";
    } catch (const exception& e) {
        cout << e.what() << "\n";
    }
    return 0;
}