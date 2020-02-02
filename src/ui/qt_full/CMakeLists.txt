set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5
    REQUIRED
    COMPONENTS
        Core
        Gui
        Multimedia
        Widgets
)

include_directories(../../)

add_executable(zlgb
    InfoWindow.cpp
    KeyBindingDialog.cpp
    LogWindow.cpp
    main.cpp
    MainWindow.cpp
    SettingsConstants.cpp
    SettingsDialog.cpp
)

target_link_libraries(zlgb
    Qt5::Core
    Qt5::Gui
    Qt5::Multimedia
    Qt5::Widgets
    zlgb_core
)

target_compile_definitions(zlgb
    PRIVATE QT_DEPRECATED_WARNINGS
)

add_subdirectory(debugger)