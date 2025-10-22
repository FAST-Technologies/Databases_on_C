cmake_minimum_required(VERSION 3.29)
project(Databases_on_C C)

set(CMAKE_C_STANDARD 11)

# Путь к PostgreSQL
set(PG_HOME "C:/Program Files/PostgreSQL/18")

# Проверка существования
if(NOT EXISTS "${PG_HOME}")
    message(FATAL_ERROR "PostgreSQL не найден по пути: ${PG_HOME}")
endif()

# ECPG файлы
set(ECPG_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_sql.pgc)
set(ECPG_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_sql.c)

# Препроцессинг ECPG
add_custom_command(
        OUTPUT ${ECPG_OUTPUT}
        COMMAND "${PG_HOME}/bin/ecpg.exe" -o ${ECPG_OUTPUT} ${ECPG_SOURCE}
        DEPENDS ${ECPG_SOURCE}
        COMMENT "Running ecpg preprocessor..."
        VERBATIM
)

# Исполняемый файл
add_executable(Databases_on_C ${ECPG_OUTPUT})

# Include директории
target_include_directories(Databases_on_C PRIVATE
        "${PG_HOME}/include"
        "${PG_HOME}/include/server"
)

# Библиотеки
target_link_directories(Databases_on_C PRIVATE "${PG_HOME}/lib")

# Линковка
target_link_libraries(Databases_on_C PRIVATE
        libecpg
        libpgtypes
        libpq
)
