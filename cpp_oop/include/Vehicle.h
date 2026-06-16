// to avoid multiple include (include guard)
#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

// enum: a user-defined type that represents a finite set of named integer
enum Color {
    RED,      // 預設 RED==0
    GREEN,    // GREEN==1
    BLUE,     // BLUE==2
    YELLOW,
    BLACK,
    WHITE
};

// 用來表示交通工具的狀態
enum VehicleStatus {
    IDLE = 0,
    MOVING = 1,
    REFUELING = 2,
    MAINTENANCE = 3
};

// 在.h宣告變數extern但不初始化
extern int global_vehicle_count;

// 交通工具（抽象概念）
class Vehicle {
protected:
    int fuel;          // 燃料
    int max_speed;     // 最大速度
    Color color;       // 顏色 (使用 enum)
    VehicleStatus status;  // 狀態 (使用 enum)

public:
    Vehicle(int fuel, int max_speed, Color color = WHITE);
    virtual ~Vehicle();               // destructor

    void refuel(int amount);

    // getter function (get the values of these variables but not directly accessing them)
    int getFuel() const; // const -> this function should not modify the variable in the class
    int getMaxSpeed() const;
    Color getColor() const;
    VehicleStatus getStatus() const;

    // setter function (set the values of specified varaible)
    void setStatus(VehicleStatus new_status);

    // demo how to use enum for if-statement
    std::string getColorName() const;

    // Polymorphism interface, declared virtual
    // this is pure virtual function(so the child of Vehicle must declare own move function)
    // pure virtual function does not need function definition here
    virtual std::string move() const = 0;
};

#endif
