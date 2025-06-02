cmake_minimum_required(VERSION 3.14) # Подняли версию для удобства с FetchContent
project(task CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Установка стандартных путей для вывода артефактов ---
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Для многоконфигурационных генераторов (например, Visual Studio, Xcode)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin)
endforeach()


if(MSVC)
    add_compile_options(/W4 /WX)
else()
    # -Werror может быть слишком строгим на этапе разработки, можно закомментировать
    add_compile_options(-Wall -Wextra -pedantic -Werror) 
endif()

# --- Основное приложение ---
# Создаем библиотеку из логики клуба
add_library(club_logic OBJECT 
    computer_club.cpp
) 
# Делаем заголовки доступными для тех, кто использует club_logic
target_include_directories(club_logic PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR} # Для computer_club.h
)

# Исходные файлы для основного исполняемого файла
set(MAIN_APP_SOURCES
    main.cpp
)

# Имя исполняемого файла основного приложения (например, task или ComputerClubPrototype)
set(MAIN_EXECUTABLE_NAME task) # По ТЗ task.exe, так что имя 'task'
add_executable(${MAIN_EXECUTABLE_NAME} ${MAIN_APP_SOURCES})
# Линкуем основное приложение с библиотекой логики
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE club_logic)

# --- Конфигурация для Google Test ---
# Включаем возможность тестирования на уровне проекта
enable_testing()

# Включаем FetchContent для загрузки зависимостей
cmake_policy(SET CMP0135 NEW)
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Убедимся, что gtest собирается "тихо" и не влияет на наши глобальные настройки
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Для Windows с MSVC, если есть проблемы с CRT
FetchContent_MakeAvailable(googletest)


# --- Создание исполняемого файла для тестов ---
set(TEST_EXECUTABLE_NAME RunClubTests)
add_executable(${TEST_EXECUTABLE_NAME}
    # Файлы с тестами из директории tests/
    tests/test_time.cpp
    tests/test_event.cpp
    tests/test_table_info.cpp
    tests/test_parsers.cpp
)

# Линкуем тесты с библиотекой логики клуба и Google Test
target_link_libraries(${TEST_EXECUTABLE_NAME} PRIVATE 
    club_logic      # Наша библиотека с логикой
    GTest::gtest    # Библиотека Google Test
    GTest::gmock    # Библиотека Google Mock (если нужна)
    gtest_main      # Предоставляет main() для тестов
)
# Важно: GTest:: GTest::gmock и gtest_main должны идти после club_logic

# Указываем CTest, как запускать наши тесты
add_test(
    NAME ClubUnitTests 
    COMMAND ${TEST_EXECUTABLE_NAME}
)