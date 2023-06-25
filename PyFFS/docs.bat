@echo off
stubgen --parse-only --ignore-errors -m FFS && move .\out\FFS.pyi .\FFS.pyi && rm -rf out