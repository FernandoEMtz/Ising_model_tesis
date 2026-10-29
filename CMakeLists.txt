cmake_minimum_required(VERSION 4.2)
project(Tesis_Modelo_Ising_V1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#Iniciar paquetería OpenMP
#find_package(OpenMP REQUIRED)

#Configurar cJSON
add_library(cjson_static STATIC extern/cJSON/cJSON.c)
target_include_directories(cjson_static PUBLIC extern/cJSON)

#Configurar PCG64
file(GLOB pcg_source "extern/pcg-c-0.94/src/*.c")
add_library(pcg_static STATIC ${pcg_source})
target_include_directories(pcg_static PUBLIC extern/pcg-c-0.94/include)

add_executable(Tesis_Modelo_Ising_V1 main.c
        src/gestor_red.c
        include/gestor_red.h
        src/gestor_observables.c
        include/gestor_observables.h)
target_include_directories(Tesis_Modelo_Ising_V1 PRIVATE include)

target_link_libraries(Tesis_Modelo_Ising_V1 PRIVATE cjson_static pcg_static)