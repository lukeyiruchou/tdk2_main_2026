#include "Vehicle.h"
// Vehicle.cpp hava the function definition
// and variable initialization
// corresponding to Vehicle.h

// 初始化extern過的variable
int global_vehicle_count = 0;

Vehicle::Vehicle(int fuel, int max_speed, Color color)
    : fuel(fuel), max_speed(max_speed), color(color), status(IDLE) {
    global_vehicle_count++;  // 每創建一個交通工具，總數+1
}

Vehicle::~Vehicle() {
    global_vehicle_count--;  // 交通工具被刪除時，總數-1
}

void Vehicle::refuel(int amount) {
    status = REFUELING;
    fuel += amount;
    status = IDLE;
}

int Vehicle::getFuel() const {
    return fuel;
}

int Vehicle::getMaxSpeed() const {
    return max_speed;
}

Color Vehicle::getColor() const {
    return color;
}

VehicleStatus Vehicle::getStatus() const {
    return status;
}

void Vehicle::setStatus(VehicleStatus new_status) {
    status = new_status;
}

// ===== 使用 enum 來判斷 =====
std::string Vehicle::getColorName() const {
    if (color == Color::RED) {
        return "Red";
    }
    else if (color == Color::GREEN) {
        return "Green";
    }
    else if (color == Color::BLUE) {
        return "Blue";
    }
    else if (color == Color::YELLOW) {
        return "Yellow";
    }
    else if (color == Color::BLACK) {
        return "Black";
    }
    else if (color == Color::WHITE) {
        return "White";
    }
    return "Unknown";
}
