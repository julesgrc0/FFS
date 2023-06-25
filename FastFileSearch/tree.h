#pragma once

#include "stdafx.h"
#include "util.h"

void CreateTree(std::string path, std::string& tree) {
  if (path.back() == '\\') path.pop_back();

  ReadDirectory(path, [&](const WIN32_FIND_DATAA& findData) {
    if (IsValidDirectory(findData)) {
      std::string subDirectory = path + "\\" + findData.cFileName;
      tree.append(subDirectory);
      tree.push_back(SEPARATOR);

      CreateTree(subDirectory, tree);
    } else if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      tree.append(findData.cFileName);
      tree.push_back(SEPARATOR);
    }

    return true;
  });
}

void WriteTree(std::vector<uint8_t>& out_cache, std::string& tree) {
  out_cache = std::vector<uint8_t>(tree.begin(), tree.end());
}

void SearchTree(const std::string& search, std::vector<uint8_t>& in_cache,
                std::vector<std::string>& results) {
  size_t pos = 0;
  size_t searchSize = search.size();

  while (pos < in_cache.size()) {
    auto it = std::search(in_cache.begin() + pos, in_cache.end(),
                          search.begin(), search.end());
    if (it == in_cache.end()) break;

    size_t searchPos = std::distance(in_cache.begin(), it);

    size_t dirPos = std::string::npos;
    size_t dirStartPos = std::string::npos;
    size_t dirEndPos = std::string::npos;
    size_t fileEndPos = std::string::npos;
    bool is_valid = true;

    for (auto it = in_cache.begin() + searchPos; it != in_cache.begin(); --it) {
      if (dirPos == std::string::npos && *it == '\\')
        dirPos = std::distance(in_cache.begin(), it);

      if (dirPos != std::string::npos && *it == SEPARATOR) break;

      dirStartPos = std::distance(in_cache.begin(), it);
    }

    for (auto it = in_cache.begin() + dirStartPos;
         it < in_cache.end() && *it != SEPARATOR; ++it)
      dirEndPos = std::distance(in_cache.begin(), it);

    for (auto it = in_cache.begin() + searchPos; it < in_cache.end(); ++it) {
      fileEndPos = std::distance(in_cache.begin(), it);

      if (*it == '\\') {
        is_valid = false;
        break;
      }

      if (*it == SEPARATOR) break;
    }

    if (is_valid && fileEndPos != std::string::npos &&
        dirPos != std::string::npos) {
      auto directoryStart = in_cache.begin() + dirStartPos;
      auto directoryEnd = in_cache.begin() + dirEndPos;
      auto filenameStart = in_cache.begin() + searchPos;
      auto filenameEnd = in_cache.begin() + fileEndPos;

      std::string directory(directoryStart, directoryEnd);
      std::string filename(filenameStart, filenameEnd);

      directory.push_back('\\');
      directory.append(filename);
      results.emplace_back(std::move(directory));
    }

    pos = searchPos + searchSize;
  }
}