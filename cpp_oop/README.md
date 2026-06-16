# C++ OOP Tutorial

2025 Winter Turtorial Supplementary C++, OOP的範例程式

## Topics Covered

### 1. OOP (Object-Oriented Programming)
- **Encapsulation**: Private members in `Car` and `Airplane` classes
- **Inheritance**: `Car` and `Airplane` inherit from `Vehicle`
- **Polymorphism**: Virtual function `move()` with different implementations

### 2. Header File (.h) & Source File (.cpp)

#### Header File (`.h`)
- 放 declarations（宣告）、include、extern
- 別人看 header file 就可以知道有什麼功能
- 統一放在資料夾`include`

例如: `Vehicle.h`, `Car.h`, `Airplane.h`

#### Source File (`.cpp`)
- 實際地把 header file 中的東西 implement 出來
- 放在資料夾`src`
- `main.cpp`是主程式，因為只有`main()`所以我沒寫`main.h`

例如: `Vehicle.cpp`, `Car.cpp`, `Airplane.cpp`


### 3. enum (Enumeration)

enum 是一個 user-defined type，代表一組有限的命名整數。


**用途:**
- 劃分不同的類別或狀態
- 讓程式碼更容易閱讀和維護
- 範例 1: Color
- 範例 2: VehicleStatus
- 都在`Vehicle.h`


### 4. Global Variable & extern

**重要規則:**
- extern 只要宣告一次
- 接下來都用 include header file 的方式
- **初始化只要一次**（多個程式嘗試初始化會造成問題！）

**本專案範例:**
- `Vehicle.h`: 宣告 `extern int global_vehicle_count;`
- `Vehicle.cpp`: 初始化 `int global_vehicle_count = 0;`
- `main.cpp`: 透過 include `Car.h` (which includes `Vehicle.h`) 使用這個變數

## Project Structure

```
cpp_oop/
├── include/
│   ├── Vehicle.h    # base class + enum + extern 宣告(declaration)
│   ├── Car.h        # Car class declaration
│   └── Airplane.h   # Airplane declaration
├── src/
│   ├── Vehicle.cpp  # Vehicle 實作(implementation) + global 變數初始化
│   ├── Car.cpp      # Car implementation
│   ├── Airplane.cpp # Airplane implementation
│   └── main.cpp     # 主程式
├── Makefile         # 編譯腳本
└── README.md        # 本檔案
```

## How to Build and Run

- 由於有多個檔案需要編譯，我叫AI寫了`Makefile`
- `Makefile`可以包含編譯的指令，給`make`工具使用
- 學ROS時會有`CMakeLists.txt`，功能比Makefile更多，編譯過程中會自動生成`Makefile`
- 不需要打開`Makefile`
- 想要自己用`g++`之類的編譯請問AI

### Entering the directory in docker container
```bash
# 以免有人本地端無法編譯C++，這邊的指令是在container裡編譯
# 編譯管道與ROS無關，只是單純的 make工具 與 g++ compiler

### Build docker environement(Same as the homepage READE.md)
- `cd docker`
- docker compose up -d
- *Container ros2_tutorial Running* # expected o/p, dont type
- docker exec -it ros2_tutorial bash

### Enter cpp_oop directory
cd cpp_oop
```

### Using Makefile
```bash
# type the command in terminal

# Build the project
make

# Build and run
make run

# Clean build files
make clean

# Rebuild from scratch
make rebuild
```

