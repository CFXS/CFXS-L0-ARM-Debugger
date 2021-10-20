file(GLOB_RECURSE sources CONFIGURE_DEPENDS
  "src/*.c"
  "src/*.cpp"
)

file(GLOB_RECURSE headers CONFIGURE_DEPENDS
  "src/*.h"
  "src/*.hpp"
)

file(GLOB_RECURSE UI_files CONFIGURE_DEPENDS
  "src/UI/*.ui"
)

file(GLOB_RECURSE QRC_files CONFIGURE_DEPENDS
  "res/*.qrc"
)