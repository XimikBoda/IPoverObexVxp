set(PROJECT_NAME "UI")

add_library(${PROJECT_NAME} STATIC 
    UIBase.h
    UIMenuBase.h UIMenuBase.cpp
    UIEngine.h UIEngine.cpp
    UITextBox.h UITextBox.cpp
    UIComboBox.h UIComboBox.cpp
    UIButton.h UIButton.cpp
    UIConnectMenu.h UIConnectMenu.cpp
    T2Input.h T2Input.cpp
    Render.h Render.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ".")

target_link_libraries(${PROJECT_NAME} mreapi)