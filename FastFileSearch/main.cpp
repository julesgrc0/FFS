#include "stdafx.h"
#include "util.h"

#include "index.h"
#include "cache.h"


int main(int argc, const char** argv)
{
    
    std::string options;
    std::cout << "Generate index cache (y/n)>";
    std::getline(std::cin, options);

    if (options.size() <= 0 || options[0] == 'y')
    {
        std::string target_path;
        std::cout << "(target)>";
        std::getline(std::cin, target_path);

        IndexMap index;
        Timer::get()->start("create_index");
        CreateIndex(target_path, index);
        Timer::get()->end("create_index");

        std::vector<uint8_t> cache;
        Timer::get()->start("write_index");
        WriteIndex(cache, index);
        Timer::get()->end("write_index");

        if (index.empty())
        {
            std::cout << "failed to generate index\n";
            return 1;
        }

        std::string cache_path;
        std::cout << "(cache)>";
        std::getline(std::cin, cache_path);

        Timer::get()->start("write_cache");
        if (!WriteCache(cache_path, cache))
        {
            std::cout << "failed to write cache\n";
        }
        Timer::get()->end("write_cache");
    }
    else 
    {
        std::string cache_path;
        std::cout << "(cache)>";
        std::getline(std::cin, cache_path);

        std::vector<uint8_t> cache;
        Timer::get()->start("read_cache");
        if (!ReadCache(cache_path, cache))
        {
            std::cout << "failed to read cache\n";
            return 1;
        }
        Timer::get()->end("read_cache");

        IndexMap index;
        Timer::get()->start("load_index");
        LoadIndex(cache, index);
        Timer::get()->end("load_index");

        if (index.empty())
        {
            std::cout << "failed to parse index\n";
            return 1;
        }

        std::string search;

        while (true)
        {
            std::cout << "(search)>";
            std::getline(std::cin, search);
            if (search.empty()) break;

            std::vector<std::string> results;

            Timer::get()->start("search_index");
            SearchIndex(search, results, index);
            Timer::get()->end("search_index");

            if (results.empty())
            {
                std::cout << "no found \n";
            }else
            {
                size_t min_len = min(results.size(), 10);
                for (size_t i = 0; i < min_len; i++)
                {
                    std::cout << results[i] << '\n';
                }

                if (min_len != results.size())
                {
                    std::cout << "... and " << (results.size() - min_len) << " more results.\n";
                }
            }
        }
    }
    return 0;
}