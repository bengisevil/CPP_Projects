#!/bin/sh
# assumes root is defined as CMAKE_SOURCE_DIR
echo "Running Coverage assuming source root = " ${ROOT}
lcov -c -i -b ${ROOT} -d . --no-external -o Coverage.baseline
./unit_tests
./virtual_machine_gui_test
lcov -c -b ${ROOT} -d . --no-external  -o Coverage.out
lcov -a Coverage.baseline -a Coverage.out -o Coverage.combined
lcov -r Coverage.combined catch.hpp moc_* *.moc -o Coverage.combined.cleaned
genhtml Coverage.combined.cleaned -o Coverage_Report
