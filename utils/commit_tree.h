#pragma once  // to include this header file only once
#include "hash_object.h"
#include "object.h"
#include "write_tree.h"
#include <ctime>
#include <cstring>
#include <sstream>
#include <string_view>


namespace commit_tree {
  inline std::string invoke(std::string_view tree_sha, std::string_view parent_sha, std::string_view message) {
    std::stringstream body;
    std::time_t now = std::time(0);
    std::tm *local_time = std::localtime(&now);
    char buffer[8];
    std::strftime(buffer, sizeof(buffer), "%z", local_time);
    body << "tree " << tree_sha << "\nparent " << parent_sha << "\nauthor StochasticBatman" << now << " " << buffer << "\n\n" << message << "\n";
    object::Object commit { object::Kind::COMMIT, body.str().size(), body.str() };
    return hash_object::invoke(commit, true);
  }
} // namespace commit_tree
