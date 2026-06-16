// to avoid multiple include (include guard)
#ifndef CAR_H
#define CAR_H

#include "Vehicle.h" // to use class declaration of Vehicle


// Car is-a Vehicle, so Car inherits from Vehicle
class Car : public Vehicle {
private:
    int seat_count;    // encapsulated

public:
    // Constructor with color parameter
    Car(int fuel, int max_speed, int seat_count, Color color = RED);

    // 除了inherited from Vehicle的public getter/setter function，Car還有自己的函式
    void drive();
    int getSeatCount() const;

    std::string move() const override; // declare with override to enable polymorphism
};

#endif
