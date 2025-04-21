#!/bin/bash

# Running clang over files
find ./tests -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -i {} -style Chromium \;

find ./lib -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -i {} -style Chromium \;
