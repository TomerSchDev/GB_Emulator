# CMake generated Testfile for 
# Source directory: /home/tomer/Devlopment/NES_Emultor
# Build directory: /home/tomer/Devlopment/NES_Emultor/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(check_gbe "/home/tomer/Devlopment/NES_Emultor/build/tests/check_gbe")
set_tests_properties(check_gbe PROPERTIES  _BACKTRACE_TRIPLES "/home/tomer/Devlopment/NES_Emultor/CMakeLists.txt;96;add_test;/home/tomer/Devlopment/NES_Emultor/CMakeLists.txt;0;")
subdirs("lib")
subdirs("gbemu")
subdirs("tests")
