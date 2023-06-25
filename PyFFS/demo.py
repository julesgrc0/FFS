import os
import sys
import datetime
from FFS import *


def select_options(opts):
    for index, opt in enumerate(opts):
        print(f"[{index}]: {opt}")

    try:
        index = int(input(f"(select)>"))
        index %= len(opts)

        return index
    except:
        return None
    
def quit_message():
    input("Press any key to exit...")
    sys.exit(0)


def input_path(msg: str) -> str:
    while True:
        try:
            path = input(f"({msg})>")
        except KeyboardInterrupt as e:
            quit_message()

        if os.path.exists(path):
            return path
        else:
            print("[-] This path does not exists")
        
def input_yesno() -> str:
    return input("(y/n)>").lower().startswith("y")


def live_searching(func, func_add = None): 
    path = None
    if func_add is None:   
        path = input_path("target")
    multithread = True
    show_results = 0

    while True:
        search = input(">")
        if search is None:
            break
        match(search):
            case "":
                break
            case "$exit":
                break
            case "$target":
                path = input_path("target")
                print('[+] Target path has been changed.')
            case "$thread":       
                multithread = not multithread
                print(f"[+] Multithread has been {'enable' if multithread else 'disable'}.")
            case "$results":
                show_results = int(input("(results)>"))
                print(f'[+] {show_results} results will be display in the next search.')
            case _:
                start = datetime.datetime.now()
                results = []
                if func_add is None:
                    print(multithread)
                    results = func(path, search, multithread)
                else:
                    results = func(search, func_add)
                delta = datetime.datetime.now() - start

                print(f"{len(results)} results ({delta if delta.seconds != 0 else str(int(delta.microseconds/1000)) + ' ms'})")
                for i in range(0, min(len(results), show_results)):
                    try:
                        print(f"[{i}]: {results[i].decode('utf-8')}")
                    except:
                        print(f"[{i}]: {results[i]}")


def time_function(msg, func, *args):
    start = datetime.datetime.now()
    data = func(*args)
    delta = datetime.datetime.now() - start
    print(f"[+] {msg} ({delta})")

    return data

def generate_cache(name, create_func, write_func):
    path = input_path("target")
                
    data = time_function(f"{name} generated", create_func, path)

    cache = input("(cache)>")
    time_function("Cache generated", cache_write, cache, write_func(data))

    return data

def main(args):
    opts = ["dfs","linear","index","tree"]
    selected_index = select_options(opts)

    if selected_index is None:
        quit_message()
    
    match(opts[selected_index]):
        case "dfs":
            print('[+] DFS Search selected')
            live_searching(dfs_search)
        case "linear":
            print('[+] Linear Search selected')
            live_searching(linear_search)
        case "index":
            print("[+] Index Search selected")
            print("[?] Generate new cache file")
            if input_yesno():
                index = generate_cache("Index", indexer_create, indexer_write)
                live_searching(indexer_search, index)
            else:
                print("[?] Load a cache file")
                index = None
                if input_yesno():
                    path = input_path("cache")

                    cache = time_function("Cache loaded", cache_read, path)
                    index = time_function("Index loaded", indexer_read, cache)
                else:
                    path = input_path("target")
                    index = time_function("Index generated", indexer_create, path)

                if index is not None:
                    live_searching(indexer_search, index)
        case "tree":
            print("[+] Tree Search selected")
            print("[?] Generate new cache file")
            if input_yesno():
                generate_cache("Tree", tree_create, tree_write)
            else:
                path = input_path("cache")

                cache = time_function("Cache loaded", cache_read, path)
                live_searching(tree_search, cache)
    quit_message()
    return 0
            

if __name__ == "__main__":
    sys.exit(main(sys.argv))
