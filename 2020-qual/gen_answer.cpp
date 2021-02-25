#include <domain/common.h>

int main(int argc, char *argv[])
{
  ifstream in(argv[1]);
  if (!in.is_open()) {
    cerr << __FUNCTION__ << " " << __LINE__ << endl;
    throw;
  }

  input(in);

  init_solution(argv[2]);

  vector<int> valid_libraries;
  for (int i = 0; i < num_library; i++) {
    if (solution.libraries[i].signup_order >= 0) {
      valid_libraries.push_back(i);
    }
  }

  sort(begin(valid_libraries), end(valid_libraries), [](int lhs, int rhs) {
    return solution.libraries[lhs].signup_order < solution.libraries[rhs].signup_order;
  });

  std::ofstream os(argv[3]);
  os << valid_libraries.size() << endl;
  for (int i : valid_libraries) {
    os << i << endl;
    bool first = true;
    for (int bi : solution.libraries[i].ship_books) {
      if (!first) os << " ";
      os << bi;
      first = false;
    }
    os << endl;
  }
}
