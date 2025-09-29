#include "utils/cat_file.h"
#include "utils/hash_object.h"
#include "utils/list_tree.h"
#include "utils/write_tree.h"
#include "utils/commit_tree.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <zlib.h>


int main(int argc, char *argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2) {
      std::cerr << "No command provided.\n";
      return EXIT_FAILURE;
    }

    std::string command = argv[1];

    if (command == "init") {
      try {
        std::filesystem::create_directory(".matiane");
        std::filesystem::create_directory(".matiane/objects");
        std::filesystem::create_directory(".matiane/refs");

        std::ofstream headFile(".matiane/HEAD");
        if (headFile.is_open()) {
          headFile << "ref: refs/heads/main\n";
          headFile.close();
        } else {
          std::cerr << "Failed to create .matiane/HEAD file.\n";
          return EXIT_FAILURE;
        }

        std::cout << "Initialized matiane directory\n";
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
      }
    } else if (command == "cat-file") {
      cat_file::invoke(argv[3]);
    } else if (command == "hash-object") {
      bool w_flag = false;

      if (argc == 4) {
        if (std::strcmp(argv[2], "-w") == 0) {
          w_flag = true;
        } else {
          std::cerr << "Unrecognised flag to hash-object: " << argv[2] << '\n';
          return EXIT_FAILURE;
        }
      }

      std::cout << hash_object::invoke(argv[argc - 1], w_flag);
      return EXIT_SUCCESS;

    } else if (command == "ls-tree") {
      bool name_only = false;
      if (argc == 4) {
        if (std::strcmp(argv[2], "--name-only") == 0) {
          name_only = true;
        }
      }

      std::string_view hash { argv[argc - 1] };
      list_tree::invoke(hash, name_only);
    } else if (command == "write-tree") {
      std::cout << write_tree::invoke();
      return EXIT_SUCCESS;
    } else if (command == "commit-tree") {
      // I only need to read the string, no need for modification and ownership
      std::string_view tree_sha {argv[2]};
      std::string_view parent_sha {argv[4]};
      std::string_view message {argv[6]};
      std::cout << commit_tree::invoke(tree_sha, parent_sha, message);
    } else {
      std::cerr << "Unknown command " << command << '\n';
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
