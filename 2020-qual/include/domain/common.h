#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <vector>

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

using namespace std;

struct Library {
  int64_t num_books;
  int64_t signup_days;
  int64_t can_ship;
  vector<int> books;
};

struct LibraryState {
  int signup_order;
  vector<int> ship_books;

  template <class Archive>
  void save(Archive &ar) const
  {
    ar(signup_order, ship_books);
  }

  template <class Archive>
  void load(Archive &ar)
  {
    ar(signup_order, ship_books);
  }
};

struct Solution {
  vector<LibraryState> libraries;
  vector<uint8_t> used_books;
  int64_t score;
  int cur_day;

  template <class Archive>
  void save(Archive &ar) const
  {
    ar(libraries, used_books, score, cur_day);
  }

  template <class Archive>
  void load(Archive &ar)
  {
    ar(libraries, used_books, score, cur_day);
  }
};

int64_t num_books, num_library, num_days;
vector<int64_t> book_score;
vector<Library> libraries;
Solution solution;

void input(istream &is)
{
  is >> num_books >> num_library >> num_days;

  // cerr << num_books << " " << num_library << " " << num_days << endl;

  book_score.resize(num_books);
  libraries.resize(num_library);

  for (auto &bs : book_score) {
    is >> bs;
  }

  for (auto &l : libraries) {
    is >> l.num_books >> l.signup_days >> l.can_ship;
    l.books.resize(l.num_books);
    for (auto &b : l.books) {
      is >> b;
    }

    sort(begin(l.books), end(l.books), [](int lhs, int rhs) {
      return book_score[lhs] > book_score[rhs];
    });
  }
}

void init_solution(char *path)
{
  std::ifstream ifs_state(path, std::ios::binary);
  if (ifs_state.is_open()) {
    cerr << "loading" << endl;
    cereal::BinaryInputArchive archive(ifs_state);
    archive(solution);
    cerr << "score " << solution.score << endl;
  } else {
    solution.libraries.resize(num_library);
    solution.used_books.resize(num_books);
    solution.score = 0;
    solution.cur_day = 0;

    for (auto &l : solution.libraries) {
      l.signup_order = -1;
    }
  }
}

void save_solution(char *path)
{
  cerr << "saving" << endl;
  std::ofstream os(path, std::ios::binary);
  cereal::BinaryOutputArchive archive(os);
  archive(solution);
}
