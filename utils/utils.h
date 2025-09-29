#ifndef UTILS_H
#define UTILS_H
#include "object.h"  // relative to main.cpp
#include <charconv>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <vector>
#include <zlib.h>


namespace utils {
  inline std::string hash2path(std::string_view hash) {
    std::stringstream path;
    path << ".matiane/objects/" << hash.substr(0, 2) << "/" << hash.substr(2);
    return path.str();
  }

  inline object::Object hash2object(std::string_view hash) {
    std::string obj_fpath = utils::hash2path(hash);

    std::ifstream f{obj_fpath};

    if (!f.is_open()) {
      std::cerr << "Unable to open file " << obj_fpath;
      throw std::ios_base::failure("Unable to open object file");
    }
    std::stringstream ss;
    ss << f.rdbuf();
    std::string compressed_blob{ss.str()};

    std::string blob {};
    blob.resize(compressed_blob.size());

    while (true) {
      size_t len = blob.size();
      if (auto res = uncompress((uint8_t *)blob.data(), &len, (const uint8_t *)compressed_blob.data(), compressed_blob.size());
          res == Z_BUF_ERROR) {
        blob.resize(blob.size() * 2);
      } else if (res != Z_OK) {
        std::cerr << "Failed to uncompress Zlib. (code: " << res << ")\n";
        throw std::runtime_error("Failed to uncompress");
      } else {
        blob.resize(len);
        break;
      }
    }

    size_t space_pos = blob.find(' ');
    size_t header_end = blob.find('\0');

    if (header_end == blob.size()) {
      std::cerr << "Expected null terminator seperating object header and contents\n";
      throw std::runtime_error("malformed object file");
    }

    // get kind
    const std::string kind_str = blob.substr(0, space_pos);

    object::Kind kind;
    if (kind_str == "blob") {
      kind = object::Kind::BLOB;
    } else if (kind_str == "tree") {
      kind = object::Kind::TREE;
    } else {
      std::cerr << "object kind not recognised: " << kind_str << "\nexpected one of 'blob', 'tree'\n";
      throw std::runtime_error("malformed object file");
    }

    // get size
    size_t size;
    size_t header_size = header_end + 1;
    auto [p, errc] = std::from_chars(blob.data() + space_pos + 1, blob.data() + header_end, size);
    if (errc == std::errc::invalid_argument) {
      throw errc;
    }

    size_t actual_size = blob.size() - header_size;

    if (size != actual_size) {
      std::cerr << "expected content size " << size << " got size " << actual_size << "\n";
      throw std::runtime_error("malformed object file");
    }

    // get content
    std::string content{blob.substr(header_end + 1, size)};

    return object::Object{kind, size, content};
  }

  inline std::string hex2bytestring(std::string_view hex) {

    std::stringstream ss;
    for (size_t i = 0; i < hex.size(); i+=2) {
      int hex_val;
      auto [p, errc] = std::from_chars(hex.data() + i, hex.data() + i + 2, hex_val, 16);
      if (errc == std::errc::invalid_argument) {
        std::cerr << "invalid hex input " << hex << '\n';
        throw errc;
      }
      ss << (char)hex_val;
    }
    return ss.str();
  }
} // namespace utils
#endif
