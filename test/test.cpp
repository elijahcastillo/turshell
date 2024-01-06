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

    // Add your tests here
    runner.addTest(Test("Test 1 (types)", "test/typesTest.txt", expected_1));
    // ... more tests

    runner.runAll();

    return 0;
}
