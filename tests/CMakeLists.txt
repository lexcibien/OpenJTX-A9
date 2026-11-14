# Test targets belong in this directory. Keep Qt Test optional for release builds.
find_package(Qt6 ${OPENJTX_QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Test)
