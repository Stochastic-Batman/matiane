#pragma once  // to include this header file only once
#include <stdexcept>
#include <string>


namespace object {
  enum Kind {
    BLOB,
    TREE,
    COMMIT,
  };

  struct Object {
    Kind kind;
    size_t size;
    std::string content;
  };


  inline std::string kind2string(Kind kind) {
    switch (kind) {
      case (Kind::BLOB): return "blob";
      case (Kind::TREE): return "tree";
      case (Kind::COMMIT): return "commit";
      default: return "";
    }
  }
} // namespace object
