#pragma once  // to include this header file only once
#include "object.h"  // relative to main.cpp
#include "utils.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>


namespace list_tree {
inline void invoke(std::string_view hash, bool name_only = false) {
  object::Object tree = utils::hash2object(hash);
  size_t obj_count = std::count(tree.content.begin(), tree.content.end(), '\0');

  char *ptr = tree.content.data();
  size_t bytes_left = tree.size;

  std::stringstream builder;

  while (bytes_left > 0) {
    if (*ptr == ' ') {
      if (!name_only) {
        if (builder.str().size() < 6) {
          for (size_t i = 0; i < 6 - builder.str().size(); ++i) {
            std::cout << "0";
          }
        }
        std::cout << builder.str() << " " << object::kind2string(tree.kind) << " ";
      }
      builder.clear();
      builder.str("");
      ++ptr;
      --bytes_left;
    } else if (*ptr == '\0') {
      // just consumed name, about to consume SHA
      const std::string name = builder.str();
      builder.clear();
      builder.str("");
      ++ptr;
      for (size_t i = 0; i < 20; ++i) {
        builder << *ptr;
        ++ptr;
      }
      if (!name_only) {
        std::cout << builder.str() << std::string(24 - builder.str().size(), ' ') << name << "\n";
      } else {
        std::cout << name << "\n";
      }
      builder.clear();
      builder.str("");
      bytes_left -= 21;
    } else {
      builder << *ptr;
      ++ptr;
      --bytes_left;
    }
  }
}
} // namespace list_tree
