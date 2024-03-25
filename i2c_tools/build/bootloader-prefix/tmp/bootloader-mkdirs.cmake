# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/nindaba/esp/esp-idf/components/bootloader/subproject"
  "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader"
  "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader-prefix"
  "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader-prefix/tmp"
  "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader-prefix/src"
  "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/nindaba/Documents/vistula/semester-7/pen-motion/i2c_tools/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
