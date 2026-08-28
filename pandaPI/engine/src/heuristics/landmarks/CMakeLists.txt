file(GLOB hlm_SRC "*.h" "*.cpp" "./lm_data_structures/*.cpp" "./lm_data_structures/*.h" "./lm_extraction/*.cpp" "./lm_extraction/*.h")

add_library(hlm ${hlm_SRC})
target_compile_options(hlm PUBLIC ${compiler_flags})
target_link_options(hlm PUBLIC ${linker_flags})
target_link_libraries(hlm PUBLIC hrc)
target_link_libraries(hlm PUBLIC heuristics)
target_link_libraries(hlm PUBLIC intDataStructures)
