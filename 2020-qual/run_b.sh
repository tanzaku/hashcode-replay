#!/bin/bash

set -eu

INPUT_DIR=./input

# g++ main.cpp -g -std=c++11 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -DADJUST -Iinclude

# g++ validate.cpp -o validate.out -O3 -std=c++11 -Iinclude

g++ gen_answer.cpp -o gen_answer.out -g -std=c++20 -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow -Iinclude
# g++ gen_answer.cpp -o gen_answer.out -O3 -std=c++20 -Iinclude

# g++ main_b.cpp -o main_b.out -O3 -std=c++20 -Iinclude -pthread -g -fsanitize=address -fsanitize=undefined -Wall -Wextra -Wshadow
g++ main_b.cpp -o main_b.out -O3 -std=c++20 -Iinclude -pthread -Wall -Wextra -Wshadow

./main_b.out ${INPUT_DIR}/b_read_on.txt b.cereal
# ./main_b.out ${INPUT_DIR}/c_incunabula.txt b.cereal
# ./main_b.out ${INPUT_DIR}/d_tough_choices.txt b.cereal

./gen_answer.out ${INPUT_DIR}/b_read_on.txt b.cereal b.out
# ./validate.out ../input/b_read_on.txt b.out
