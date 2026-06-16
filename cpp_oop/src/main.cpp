#include <iostream>
#include <memory>
#include "Car.h"
#include "Airplane.h"

using namespace std;

int main() {
    cout << "===== C++ OOP Tutorial =====" << endl;
    cout << "Topics: Header/Source files, enum, extern, OOP" << endl << endl;

    // to see the initialization of extern global variable
    cout << "Global vehicle count: " << global_vehicle_count << endl;

    // enum simple usage
    Color my_color = Color::RED;
    if (my_color == Color::RED) {
        cout << "my Color is RED!" << endl;
    }

    // OOP demo
    Car* v1 = new Car(100, 180, 5, Color::RED);
    Airplane* v2 = new Airplane(300, 900, 1000, Color::BLUE);
    // 也可以用smart pointer
    // unique_ptr<Car> v1 = make_unique<Car>(100, 180, 5, Color::RED);
    // unique_ptr<Airplane> v2 = make_unique<Airplane>(300, 900, 1000, Color::BLUE);

    cout << v1->move() << endl;
    cout << v2->move() << endl;
    
    cout << "Car color: " << v1->getColorName() << endl;
    cout << "Airplane color: " << v2->getColorName() << endl;

    cout << "Global count: " << global_vehicle_count << endl;

    // this method is inherited from Vehicle
    cout << "Car Fuel: " << v1->getFuel() << endl; 
    cout << "Airplane maxSpeed: " << v2->getMaxSpeed() << endl;

    // these methods are additional function
    cout << "Car SeatCount: " << v1->getSeatCount() << endl;
    cout << "Airplane Altitude: " << v2->getAltitude() << endl;
    

    // try on your own to call Car, Airplane's other function


    // if use smart pointer, no need to delete,
    // since system automatically release memory
    delete v1;
    delete v2;

    return 0;
}
