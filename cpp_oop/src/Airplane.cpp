#include "Airplane.h"

Airplane::Airplane(int fuel, int max_speed, int altitude, Color color)
    : Vehicle(fuel, max_speed, color), altitude(altitude) {}

void Airplane::climb(int meters) {
    if (fuel > 0) {
        status = MOVING;  // 使用 enum 設定狀態
        altitude += meters;
        fuel--;
    }
    else {
        status = IDLE;
    }
}

int Airplane::getAltitude() const {
    return altitude;
}

std::string Airplane::move() const {
    return "Airplane flies in the sky";
}
