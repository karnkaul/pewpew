include(FetchContent)

# tardigrade
FetchContent_Declare(
  tardigrade
  GIT_REPOSITORY https://github.com/karnkaul/tardigrade
  GIT_TAG 1b0909b # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/tardigrade"
)
set(TG_INSTALL ON)
FetchContent_MakeAvailable(tardigrade)

# capo
FetchContent_Declare(
  capo
  GIT_REPOSITORY https://github.com/capo-devs/capo
  GIT_TAG 259d406 # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/capo"
)
set(CAPO_BUILD_EXAMPLE ON)
FetchContent_MakeAvailable(capo)

# vulkify
FetchContent_Declare(
  vulkify
  GIT_REPOSITORY https://github.com/vulkify/vulkify
  GIT_TAG c367190 # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/vulkify"
)
set(VULKIFY_DEBUG_TRACE ON)
set(VULKIFY_BUILD_EXAMPLES ON)
FetchContent_MakeAvailable(vulkify)
