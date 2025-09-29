#pragma once  // to include this header file only once
#include "../utils/object.h"  // relative to main.cpp
#include "../utils/utils.h"
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <sstream>
#include <string_view>
#include <iostream>
#include <fstream>
#include <zlib.h>


namespace cat_file {
  inline void invoke(std::string_view hash) {
    object::Object obj = utils::hash2object(hash);
    printf("%s", obj.content.c_str());
  }
} // namespace cat_file
