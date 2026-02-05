if(NOT DEFINED RUN_BINARY)
  message(FATAL_ERROR "RUN_BINARY not set")
endif()

if(NOT DEFINED ENV{RUN_GRAPHICS})
  message(STATUS "RUN_GRAPHICS not set; skipping run target.")
  return()
endif()

execute_process(
  COMMAND "${RUN_BINARY}"
  RESULT_VARIABLE run_result
)

if(NOT run_result EQUAL 0)
  message(FATAL_ERROR "Run failed with exit code ${run_result}")
endif()
