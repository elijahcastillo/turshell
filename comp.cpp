#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct Counter {
int count;
void increment(  ){
this->count = (this->count + 1);
}
};

struct Employee {
string name;
int hoursWorked;
float hourlyRate;
float totalEarnings(  ){
return (this->hoursWorked * this->hourlyRate);
}
void updateHourlyRate( float newRate ){
this->hourlyRate = newRate;
}
};

int main() {
Counter myCounter = { .count = 0};
myCounter.increment(  );cout << "Counter value:" << " " << myCounter.count<< std::endl;
Employee employee = { .name = "Alice", .hoursWorked = 40, .hourlyRate = 20};
float earnings = employee.totalEarnings(  );
cout << employee.name << " " << "'s total earnings:" << " " << earnings<< std::endl;
employee.updateHourlyRate( 22.5 );float updatedEarnings = employee.totalEarnings(  );
cout << "Updated earnings:" << " " << updatedEarnings<< std::endl;
    return 0;
}