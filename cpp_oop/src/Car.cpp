#include "Car.h"
// no need to include Vehicle.h, since Car.h has included Vehicle.h

Car::Car(int fuel, int max_speed, int seat_count, Color color)
    : Vehicle(fuel, max_speed, color), seat_count(seat_count) {}
    // Car's constructor must call the constructor of the parent(Vehicle)

void Car::drive() {
    if (fuel > 0) {
        status = MOVING;  // 使用 enum 設定狀態
        fuel--;           // protected member
    }
    else {
        status = IDLE;
    }
}

int Car::getSeatCount() const {
    return seat_count;
}

std::string Car::move() const {
    return "Car drives on the road";
}
