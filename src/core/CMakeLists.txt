find_package(Threads REQUIRED)

add_library(zlgb_core
    Audio.cpp
    Buttons.cpp
    Cpu.cpp
    Display.cpp
    EmulatorMgr.cpp
    Input.cpp
    Interrupt.cpp
    Logger.cpp
    MemoryBankController.cpp
    Memory.cpp
    NoiseChannel.cpp
    Serial.cpp
    SquareWaveChannel.cpp
    Timer.cpp
    Utils.cpp
    WaveformChannel.cpp
)

target_link_libraries(zlgb_core
    Threads::Threads
)

add_subdirectory(tests)