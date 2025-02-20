#!/bin/bash

# Running clang over files
find ./tests -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -style=Google -i {} \;

find ./lib -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -style=Google -i {} \;