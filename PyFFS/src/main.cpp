#include <Python.h>
#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "cache.h"
#include "dfs.h"
#include "index.h"
#include "linear.h"
#include "stdafx.h"
#include "tree.h"

namespace py = pybind11;

typedef struct PyIndexerData {
  IndexMap index;
} PyIndexerData;

typedef struct PyTreeData {
  std::string data;
} PyTreeData;

typedef struct PyCacheData {
  std::vector<uint8_t> cache;
} PyCacheData;

PYBIND11_MODULE(FFS, mod) {
  mod.doc() = R"pbdoc(
FFS 2023 (c) - Jules GARCIA 
(Fast File Search)
       )pbdoc";
  mod.attr("__version__") = "0.0.1";

  py::class_<PyCacheData>(mod, "PyCacheData");
  py::class_<PyIndexerData>(mod, "PyIndexerData");
  py::class_<PyTreeData>(mod, "PyTreeData");

  mod.def(
      "dfs_search",
      [&](const std::string& path, const std::string& search, bool multi) {
        std::vector<std::string> results;

        if (!multi) {
          std::cout << "single";

          SearchDFS(path, search, results);
        } else {
          SearchParallelDFS(path, search, results);
        }

        return std::vector<py::bytes>(results.begin(), results.end());
      },
      py::arg("path"), py::arg("search"), py::arg("multithread"));

  mod.def(
      "linear_search",
      [&](const std::string& path, const std::string& search, bool multi) {
        std::vector<std::string> results;

        if (!multi) {
          std::cout << "single";
          SearchLinear(path, search, results);
        } else {
          SearchParallelLinear(path, search, results);
        }

        return std::vector<py::bytes>(results.begin(), results.end());
      },
      py::arg("path"), py::arg("search"), py::arg("multithread"));

  mod.def(
      "cache_write",
      [&](const std::string& path, PyCacheData& data) {
        
        return WriteCache(path, data.cache);
      },
      py::arg("path"), py::arg("cache"));
  mod.def(
      "cache_read",
      [&](const std::string& path) {
        PyCacheData data;
        ReadCache(path, data.cache);
        return data;
      },
      py::arg("path"));

  mod.def(
      "indexer_create",
      [&](std::string& path) {
        PyIndexerData map;
        CreateIndex(path, map.index);
        return map;
      },
      py::arg("path"));

  mod.def(
      "indexer_write",
      [&](PyIndexerData& map) {
        PyCacheData data;
        WriteIndex(data.cache, map.index);
        return data;
      },
      py::arg("index"));

  mod.def(
      "indexer_read",
      [&](PyCacheData& data) {
        PyIndexerData map;
        LoadIndex(data.cache, map.index);
        return map;
      },
      py::arg("cache"));

  mod.def(
      "indexer_search",
      [&](std::string& search, PyIndexerData& map) {
        std::vector<std::string> results;
        SearchIndex(search, results, map.index);
        return std::vector<py::bytes>(results.begin(), results.end());
      },
      py::arg("search"), py::arg("index"));

  mod.def(
      "tree_create",
      [&](std::string& path) {
        PyTreeData tree;
        CreateTree(path, tree.data);
        return tree;
      },
      py::arg("path"));

  mod.def(
      "tree_write",
      [&](PyTreeData& tree) {
        PyCacheData data;
        WriteTree(data.cache, tree.data);
        return data;
      },
      py::arg("tree"));

  mod.def(
      "tree_search",
      [&](std::string& search, PyCacheData& data) {
        std::vector<std::string> results;
        SearchTree(search, data.cache, results);
        return std::vector<py::bytes>(results.begin(), results.end());
      },
      py::arg("search"), py::arg("cache"));
}