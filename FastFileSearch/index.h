#pragma once

#include "stdafx.h"
#include "util.h"


typedef std::unordered_map<std::string, std::vector<std::string>> IndexMap;

void CreateIndex(std::string& path, IndexMap& index)
{
    if (path.back() == '\\') path.pop_back();

    ReadDirectory(path, [&](const WIN32_FIND_DATAA& findData) {

        std::string name(findData.cFileName);
        std::string item_path = path + "\\" + name;
        bool valid_dir = IsValidDirectory(findData);


        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || valid_dir)
        {
            if (index.find(name) == index.end())
            {
                index[name] = std::vector<std::string>();
            }

            index[name].push_back(item_path);

            if (valid_dir)
            {
                CreateIndex(item_path, index);
            }
        }

        return true;

        });
}

void WriteIndex(std::vector<uint8_t>& out_cache, IndexMap& index)
{
    std::stringstream out_data;
    for (const auto& [keyword, paths] : index)
    {
        out_data << keyword << START_SEPARATOR;
        for (size_t i = 0; i < paths.size(); i++)
        {
            out_data << paths[i];
            if (i != paths.size() - 1)
            {
                out_data << SEPARATOR;
            }
        }
        out_data << END_SEPARATOR;
    }

    out_cache = std::vector<uint8_t>(std::istreambuf_iterator<char>(out_data), {});
}

void LoadIndex(std::vector<uint8_t>& in_cache, IndexMap& index)
{
    std::string currentKey;
    std::string currentValue;
    std::vector<std::string> currentVector;

    bool parsingKey = true;
    bool parsingVector = false;

    for (char c : in_cache) {
        switch (c) {
        case START_SEPARATOR:
            parsingKey = false;
            parsingVector = true;
            break;

        case SEPARATOR:
            currentVector.emplace_back(std::move(currentValue));
            currentValue.clear();
            break;

        case END_SEPARATOR:
            currentVector.emplace_back(std::move(currentValue));
            index.emplace(std::move(currentKey), std::move(currentVector)); 
            currentKey.clear();
            currentVector.clear();
            parsingKey = true;
            parsingVector = false;
            break;

        default:
            if (parsingKey) {
                currentKey += c;
            }
            else if (parsingVector) {
                currentValue += c;
            }
        }
    }

}

void SearchIndex(const std::string& search, std::vector<std::string>& results, IndexMap& index)
{
    for (auto& [keyword, paths] : index)
    {
        if (keyword.find(search) != std::string::npos)
        {
            results.insert(results.end(), paths.begin(), paths.end());
        }
    }
}