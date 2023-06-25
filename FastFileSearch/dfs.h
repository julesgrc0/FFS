#pragma once

#include "stdafx.h"
#include "util.h"

void SearchDFS(std::string path, const std::string& search,
               std::vector<std::string>& results) {
  if (path.back() == '\\') path.pop_back();
  std::stack<std::string> stack;
  stack.push(path);

  while (!stack.empty()) {
    std::string current_dir = stack.top();
    stack.pop();

    ReadDirectory(current_dir, [&](const WIN32_FIND_DATAA& findData) {
      std::string filename = findData.cFileName;
      std::string item_path = current_dir + "\\" + findData.cFileName;
      bool valid_dir = IsValidDirectory(findData);

      if (filename.find(search) != std::string::npos &&
          (valid_dir ||
           !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
        results.push_back(filename);
      }

      if (valid_dir) {
        stack.push(item_path);
      }

      return true;
    });
  }
}

void SearchParallelDFS(std::string path, const std::string& search,
                       std::vector<std::string>& results) {
  unsigned int max_threads = std::thread::hardware_concurrency();

  std::stack<std::string> stack;
  stack.push(path);
  std::vector<std::future<void>> futures;

  while (!stack.empty()) {
    if (futures.size() >= max_threads) {
      auto& future = futures.back();
      future.wait();
      futures.pop_back();
    }

    std::string current_dir = stack.top();
    stack.pop();

    futures.push_back(std::async(
        std::launch::async, [&results, &stack, search, current_dir]() {
          SearchDFS(current_dir, search, results);

          ReadDirectory(current_dir, [&](const WIN32_FIND_DATAA& findData) {
            std::string item_path = current_dir + "\\" + findData.cFileName;
            if (IsValidDirectory(findData)) {
              stack.push(item_path);
            }
            return true;
          });
        }));
  }

  for (auto& future : futures) {
    future.wait();
  }
}