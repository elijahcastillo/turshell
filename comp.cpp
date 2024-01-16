#include <iostream>
#include <vector>
#include <string>
using namespace std;

int sum( int a, int b ){
return (a + b);
}
bool isPositive( int number ){
return (number > 0);
}
int sumArray( vector<int> arr ){
int total = 0;
int i = 0;
while( (i < arr.size()) ){
total = (total + arr[i]);
i = (i + 1);
}
return total;
}
string reverseString( string str ){
string reversed = "";
int i = (str.size() - 1);
while( (i >= 0) ){
reversed = (reversed + str[i]);
i = (i - 1);
}
return reversed;
}
int main() {
sum( 4, 5 );int result = sum( 3, 5 );
cout << "Sum of 3 and 5 is:" << " " << result<< std::endl;int pos = 5;
int neg = (-12);
cout << "Is 5 positive?" << " " << isPositive( pos )<< std::endl;cout << "Is -12 positive?" << " " << isPositive( neg )<< std::endl;vector<int> numbers = {6, 8, 2};
int arraySum = sumArray( numbers );
cout << "Sum of array elements:" << " " << arraySum<< std::endl;string originalString = "turshell";
string reversedString = reverseString( originalString );
cout << "Reversed 'turshell':" << " " << reversedString<< std::endl;    return 0;
}