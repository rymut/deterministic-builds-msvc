add_executable(main_target "main_target.cpp")

add_executable(main_injector "main_injector.cpp")
target_link_libraries(main_injector detours::detours)

add_library(main_hook SHARED "main_hook.cpp" "main_hook.def")
target_link_libraries(main_hook detours::detours)