add_library(taskmanager_datasource STATIC
    datasource/ProcessReader.cpp
    datasource/CpuStats.cpp
    datasource/MemoryStats.cpp
    datasource/DiskStats.cpp
    datasource/NetworkStats.cpp
    datasource/GpuStats.cpp
    datasource/UserSessions.cpp
    datasource/SystemdServices.cpp
    util/ProcFileUtils.cpp
    util/Formatting.cpp
)
target_include_directories(taskmanager_datasource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(taskmanager_datasource PUBLIC Qt6::Core Qt6::DBus)

qt_add_executable(taskmanager
    main.cpp
    SystemMonitor.cpp
    app/MainWindow.cpp
    app/NavigationSidebar.cpp
    views/ProcessesView.cpp
    views/DetailsView.cpp
    views/PerformanceView.cpp
    views/LineGraphWidget.cpp
    views/ServicesView.cpp
    views/StartupView.cpp
    views/UsersView.cpp
    views/AppHistoryView.cpp
    models/ProcessTableModel.cpp
    models/ServiceTableModel.cpp
    models/StartupTableModel.cpp
    models/UserTableModel.cpp
    ../resources/resources.qrc
)

target_include_directories(taskmanager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(taskmanager PRIVATE
    taskmanager_datasource
    Qt6::Widgets
    Qt6::Charts
    Qt6::DBus
)
