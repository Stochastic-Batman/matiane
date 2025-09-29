#pragma once  // to include this header file only once
#include "../utils/object.h"  // relative to main.cpp
#include "../utils/sha1.h"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include<iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <zlib.h>


namespace hash_object {
  inline std::string invoke(const object::Object &obj, bool w_flag = false) {
    SHA1 hasher;
    const std::string src = object::kind2string(obj.kind) + " " + std::to_string(obj.size) + '\0' + obj.content;
    assert(obj.size == obj.content.size());
    hasher.update(src);

    const std::string hash = hasher.final();

    if (w_flag) {
      std::stringstream out_fpath;
      out_fpath << ".matiane/objects/" << hash.substr(0, 2);
      std::filesystem::create_directory(out_fpath.str());
      out_fpath << "/" << hash.substr(2);
      std::ofstream obj_file{out_fpath.str(), std::ios::binary};
      if (!obj_file.is_open()) {
        std::cerr << "unable to write object file " << out_fpath.str() << "\n";
      }

      size_t compressed_size = compressBound(src.size());

      char dest_buf[compressed_size];
      compress((Bytef *)dest_buf, (uLongf *)&compressed_size, (const Bytef *)src.c_str(), (uLong)src.size());
      /*std::cout << dest_buf << " compressed data \n";*/
      obj_file.write(dest_buf, compressed_size);
      obj_file.close();
    }

    return hash;
  }

  inline std::string invoke(const std::filesystem::path &in_fpath, bool w_flag = false) {
    std::ifstream in_file{in_fpath};
    in_file.seekg(0, in_file.end);
    size_t file_len = in_file.tellg();
    in_file.seekg(0, in_file.beg);

    std::stringstream ss;
    ss << in_file.rdbuf();
    const std::string file_contents{ss.str()};

    const object::Object obj{object::Kind::BLOB, file_len, file_contents};

    in_file.close();
    return invoke(obj, w_flag);
  }
} // namespace hash_object
