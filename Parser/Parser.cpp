#include <iostream>
#include <cctype>
#include <cstdio>
#include <fstream>

using namespace std;

enum class SymbolType {
    IntegerConstant,
    Text,
    Semicolon,
    Period,
    Quotation,
    Other,
    EndOfInput
};

char currentChar;
SymbolType currentSymbol;
char spelling[9];
const int MAX_LENGTH = 8;
int constantValue;
const int MAX_INTEGER = INT_MAX;
ifstream inputFile("input.txt");

static void getNextChar() {
    if (!inputFile.is_open()) {
        fprintf(stderr, "Error: Unable to open input file\n");
        return;
    }
    currentChar = inputFile.get();
}

static void reportError(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
}

static void getNextSymbol() {
    int digit;
    int k = 0;

    while (!inputFile.eof() && !isdigit(currentChar) && currentChar != '\"' && currentChar != ';' && currentChar != '.' && !isalpha(currentChar)) {
        getNextChar();
    }

    if (inputFile.eof()) {
        currentSymbol = SymbolType::EndOfInput;
    }

    while (currentChar == ' ') {
        getNextChar();
    }

    switch (toupper(currentChar)) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z': {
        do {
            if (k < MAX_LENGTH) {
                spelling[k++] = currentChar;
            }
            getNextChar();
        } while (isalpha(currentChar) || isdigit(currentChar));

        spelling[k] = '\0';
        if (k > MAX_LENGTH) {
            reportError("String is too long!");
        }
        currentSymbol = SymbolType::Text;
        cout << "Text: " << spelling << endl;
        break;
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
        constantValue = 0;
        do {
            digit = currentChar - '0';
            if (constantValue < (MAX_INTEGER / 10) || (constantValue == (MAX_INTEGER / 10) && digit <= (MAX_INTEGER % 10))) {
                constantValue = (10 * constantValue) + digit;
            }
            else {
                reportError("Integer constant too large!");
                constantValue = 0;
            }
            getNextChar();
        } while (isdigit(currentChar));
        currentSymbol = SymbolType::IntegerConstant;
        cout << "Integer: " << constantValue << endl;
        break;
    }
    case '.': {
        currentSymbol = SymbolType::Period;
        getNextChar();
        cout << "Period" << endl;
        break;
    }
    case ';': {
        currentSymbol = SymbolType::Semicolon;
        getNextChar();
        cout << "Semicolon" << endl;
        break;
    }
    case '\"': {
        currentSymbol = SymbolType::Quotation;
        getNextChar();
        int quoteIndex = 0;
        while (quoteIndex < MAX_LENGTH && currentChar != '\"') {
            spelling[quoteIndex++] = currentChar;
            getNextChar();
        }
        if (currentChar == '\"') {
            spelling[quoteIndex] = '\0';
            cout << "String: " << spelling << endl;
            getNextChar();
        }
        else {
            reportError("String is too long or missing closing quotation mark!");
            spelling[quoteIndex] = '\0';
            cout << "String: " << spelling << endl;
        }
        break;
    }
    case '\0': {
        if (currentSymbol != SymbolType::Other) {
            currentSymbol = SymbolType::Other;
            cout << "End of Input" << endl;
        }
        break;
    }
    default: {
        currentSymbol = SymbolType::Other;
        cout << "Unknown Symbol with ASCII value: " << static_cast<int>(currentChar) << endl;
        getNextChar();
        break;
    }
    }
}

static bool accept(SymbolType symbol) {
    if (currentSymbol == symbol) {
        getNextSymbol();
        return true;
    }
    return false;
}

static bool expect(SymbolType symbol) {
    if (accept(symbol)) {
        return true;
    }
    reportError("Unexpected symbol!");
    return false;
}

static void processField() {
    if (accept(SymbolType::IntegerConstant) || accept(SymbolType::Quotation) || accept(SymbolType::Text)) {
        return;
    }
    else if (accept(SymbolType::Period)) {
        accept(SymbolType::Quotation);
        if (currentSymbol == SymbolType::Text) {
            processField();
        }
        else {
            reportError("Expected text after period!");
        }
    }
    else if (currentSymbol != SymbolType::Period && currentSymbol != SymbolType::Other && currentSymbol != SymbolType::EndOfInput) {
        reportError("Expected integer constant, string, text, period, or end of input!");
    }
}

static void processRecord() {
    processField();
    while (currentSymbol == SymbolType::Period) {
        accept(SymbolType::Period);
        processField();
    }
    if (!(currentSymbol == SymbolType::Semicolon || currentSymbol == SymbolType::Other || currentSymbol == SymbolType::EndOfInput)) {
        reportError("Expected semicolon or end of input after the last field!");
    }
    if (currentSymbol == SymbolType::Semicolon) {
        accept(SymbolType::Semicolon);
    }
}

static void processDataFile() {
    processRecord();
    while (currentSymbol != SymbolType::Other) {
        processRecord();
    }
    inputFile.close();
}

int main() {
    getNextChar();
    getNextSymbol();
    processDataFile();
    return 0;
}
