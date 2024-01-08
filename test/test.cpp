#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <locale>

void setTextColor(WORD color) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout, color);
}

constexpr WORD RED = FOREGROUND_RED | FOREGROUND_INTENSITY;
constexpr WORD GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

// Trim from end of string (right)
std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}


std::string exec(const char* cmd) {
    std::string data;
    FILE* stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    std::string command = cmd + std::string(" 2>&1");

    stream = _popen(command.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        _pclose(stream);
    }
    return data;
}


class Test {

public:
    Test(const std::string& name, const std::string& scriptPath, const std::string& expectedOutput)
        : testName(name), scriptPath(scriptPath), expectedOutput(expectedOutput) {}

    bool run() const {
        std::string command = "turshell " + scriptPath; 
        std::string output = rtrim(exec(command.c_str()));
        std::string trimmedExpectedOutput = rtrim(expectedOutput);

        if (output != trimmedExpectedOutput) {
            setTextColor(RED);
            std::cerr << "\nTest '" << testName << "' failed.\n";
            std::cerr << "Expected:\n" << trimmedExpectedOutput << "\n\nGot:\n" << output << "\n";
            std::cerr << "Expected length: " << trimmedExpectedOutput.length() << ", Got length: " << output.length() << "\n";
            setTextColor(WHITE);
            return false;
        }
        setTextColor(GREEN);
        std::cout << "Test '" << testName << "' passed.\n";
        setTextColor(WHITE);
        return true;
    }

    const std::string& getName() const {
        return testName;
    }

private:
    std::string testName;
    std::string scriptPath;
    std::string expectedOutput;
};


class TestRunner {
public:
    void addTest(const Test& test) {
        tests.push_back(test);
    }

    void runAll() {
        int passed = 0;
        std::cout << "\nRunning tests...\n";
        for (const auto& test : tests) {
            std::cout << "\nRunning test: " << test.getName() << "\n";
            if (test.run()) {
                ++passed;
            }
        }
        setTextColor(GREEN);
        std::cout << "\n" << passed << " of " << tests.size() << " tests passed.\n";
        setTextColor(WHITE);
    }

private:
    std::vector<Test> tests;
};



int main() {
    TestRunner runner;

    std::string expected_1 = 
    "Integer type: 5 \n" 
    "Float type: 0.831500 \n"
    "String type: turshell is awesome \n"
    "Bool type: true \n"
    "Array type: Array[5, 2, 7, 1] \n";


std::string expected_2 = 
    "1 is Odd \n"
    "2 is Prime \n"
    "3 is Prime \n"
    "4 is Even \n"
    "5 is Prime \n"
    "6 is Even \n"
    "7 is Prime \n"
    "8 is Even \n"
    "9 is Odd \n"
    "10 is Even \n";


std::string expected_3 = 
    "Name: Elijah Castillo \n"
    "Letter at index 3: j \n"
    "Modified name: Elijnh Cistillo \n"
    "Length of modified name: 15 \n"
    "Concatenation of foo and bar: foobar \n"
    "Title is Boss \n";

std::string expected_4 = 
"22 \n"
"Bob \n"
"Array[521.340027, 1087.810059] \n"
"1087.810059 \n"
"Struct{isClose: true, name: Tim} \n"
"Tim \n"
"Modfying and accessing struct \n"
"15 \n"
"Not Bob Concated \n"
"Array[12.640000, 1087.119995, 789.229980] \n"
"1087.119995 \n"
"Struct{isClose: false, name: Tim} \n"
"Tim \n"
"46 \n"
"true \n"
"1888.989990 \n"
"3 \n"
"Jimmy \n"
"Array[7233.339844, 823.809998] \n"
"823.809998 \n"
"Struct{isClose: false, name: George} \n"
"George \n";



std::string expected_5 = 
"Struct{x: 3, y: 5} \n"
"8 \n"
"16 \n"
"true \n"
"false \n"
"llehsrut \n"
"LOG: llehsrut \n"
"\n"
"======== Modify Tests ========== \n"
"\n"
"Array[1, 5, 3] \n"
"Array[1, 5, 3] \n"
"Array[Struct{x: 1, y: 2}, Struct{x: 6, y: 4}, Struct{x: 3, y: 5}, Struct{x: 3, y: 3}] \n"
"Struct{x: 3, y: 5} \n"
"Array[Struct{x: 7, y: 3}, Struct{x: 14, y: 76}] \n";

    // Add your tests here
    runner.addTest(Test("1 (Types)", "test/typesTest.txt", expected_1));
    runner.addTest(Test("2 (Controll Flow)", "test/controlFlowTest.txt", expected_2));
    runner.addTest(Test("3 (Strings)", "test/stringTest.txt", expected_3));
    runner.addTest(Test("4 (Structs)", "test/structsTests.txt", expected_4));
    runner.addTest(Test("5 (Functions)", "test/functionsTest.txt", expected_5));

    runner.runAll();

    return 0;
}
