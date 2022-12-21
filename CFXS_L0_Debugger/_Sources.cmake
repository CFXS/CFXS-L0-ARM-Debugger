file(GLOB_RECURSE sources CONFIGURE_DEPENDS
  "src/*.c"
  "src/*.cpp"
  "vendor/temp/*.cpp"
)

# file(GLOB_RECURSE headers CONFIGURE_DEPENDS
#   "src/*.h"
#   "src/*.hpp"
#   "vendor/temp/*.h"
# )

file(GLOB_RECURSE QRC_files CONFIGURE_DEPENDS
  "res/*.qrc"
)