#pragma once // to include this header file only once
#include "utils/hash_object.h"  // relative to main.cpp
#include "utils/sha1.h"
#include "utils.h"
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>


namespace write_tree {
  inline std::string build_tree_for(std::filesystem::path path, bool w_flag = false) {
    std::filesystem::directory_iterator cur_dir{path};
    std::map<std::string, std::string> entries;

    for (auto const &dir_entry : cur_dir) {
      if (dir_entry.path().filename().string()[0] == '.') {
        continue;
      }

      if (dir_entry.is_directory()) {
        entries[dir_entry.path().filename().string()] = "40000 " + dir_entry.path().filename().string() + '\0' + utils::hex2bytestring(build_tree_for(dir_entry, true));
      } else {
        char mode[7];
        if (dir_entry.is_regular_file()) {
          std::strcpy(mode, "100644");
        } else if (dir_entry.is_symlink()) {
          std::strcpy(mode, "120000");
        } else {
          std::strcpy(mode, "000000");
        }

        const std::string hash = hash_object::invoke(dir_entry.path(), true);
        const std::string raw_bytes = utils::hex2bytestring(hash);

        entries[dir_entry.path().filename().string()] = std::string(mode) + " " + dir_entry.path().filename().string() + '\0' + raw_bytes;
      }
    }

    std::stringstream body;
    for (auto const &[name, item] : entries) {
      body << item;
    }

    const object::Object obj{object::Kind::TREE, body.str().size(), body.str()};

    return hash_object::invoke(obj, w_flag);
  }

  inline std::string invoke() {
    return build_tree_for(std::filesystem::current_path(), true);
  }
} // namespace write_tree
