#include <domain/common.h>

#include <tanzaku/concurrent/par_c.h>
#include <tanzaku/rng/random.h>
#include <tanzaku/sa/temp_manager.h>
#include <tanzaku/sa/time_manager.h>

using namespace tanzaku::sa::temp_manager;
using namespace tanzaku::sa::time_manager;
using namespace tanzaku::rng;
using namespace tanzaku::concurrent;

using namespace std;

Random rng;

int main(int argc, char *argv[])
{
  par_c::ThreadPoolRAII raii;

  rng.init();

  ifstream in(argv[1]);
  if (!in.is_open()) {
    cerr << __FUNCTION__ << " " << __LINE__ << endl;
    throw;
  }

  input(in);

  init_solution(argv[2]);

  auto libs = par_c::split(num_library);

  auto reducer = [&](pair<double, int> lhs, pair<double, int> rhs) {
    return std::max(lhs, rhs);
  };

  auto assign_books = [&](int i) {
    const auto &l = libraries[i];

    int sum = 0;
    vector<int> books;

    int64_t num_can_ship_books = (num_days - solution.cur_day - l.signup_days) * l.can_ship;
    if (num_can_ship_books <= 0) {
      return make_pair(sum, books);
    }

    for (int bi : libraries[i].books) {
      if (!solution.used_books[bi]) {
        sum += book_score[bi];
        books.push_back(bi);
        if (--num_can_ship_books == 0) {
          break;
        }
      }
    }

    return make_pair(sum, books);
  };

  // mutex m;

  auto mapper = [&](const par_c::Range &r) {
    auto res = make_pair(-1.0, -1);
    for (int i = r.from; i < r.to; i++) {
      if (solution.libraries[i].signup_order >= 0) {
        continue;
      }

      const auto &l = libraries[i];
      auto assined = assign_books(i);
      // double use_day = (assined.second.size() + l.can_ship - 1) / l.can_ship;
      auto cur = make_pair(assined.first / double(l.signup_days), i);
      res = reducer(res, cur);
    }
    // {
    //   unique_lock<mutex> lock(m);
    //   cerr << "range " << r.from << " " << r.to << " " << res.first << " " << res.second << endl;
    // }

    return res;
  };

  for (int i = 0; i < num_library && solution.cur_day < num_days; i++) {
    auto res = par_c::map_reduce(mapper, reducer, libs);
    if (res.first <= 0) {
      break;
    }

    cerr << i << " " << res.first << " " << res.second << " " << solution.cur_day << endl;

    auto &l = solution.libraries[res.second];
    l.ship_books = assign_books(res.second).second;
    l.signup_order = solution.cur_day;
    solution.cur_day += libraries[res.second].signup_days;
    for (int bi : l.ship_books) {
      solution.used_books[bi] = true;
      solution.score += book_score[bi];
    }
  }

  cerr << "solution.score: " << solution.score << endl;

  save_solution(argv[2]);
}
