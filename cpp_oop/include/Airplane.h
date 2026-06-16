// to avoid multiple include (include guard)
#ifndef AIRPLANE_H
#define AIRPLANE_H

#include "Vehicle.h" 

// Airplane is-a Vehicle, so Airplane inherits from Vehicle just like Car
class Airplane : public Vehicle {
private:
    int altitude;

public:
    // Constructor of Airplane
    Airplane(int fuel, int max_speed, int altitude, Color color = BLUE);
 
    // 除了inherited from Vehicle的public getter/setter function，Airplane還有自己的函式
    void climb(int meters);
    int getAltitude() const;

    std::string move() const override; // declare with override to enable polymorphism
};

#endif
