#include <deque>
#include <map>
#include <unordered_map>

#include "boost/ut.hpp"
#include "glaze/csv/read.hpp"
#include "glaze/csv/write.hpp"

using namespace boost::ut;
using namespace glz;

struct my_struct
{
   std::vector<int> num1{};
   std::deque<float> num2{};
   std::vector<bool> maybe{};
   std::vector<std::array<int, 3>> v3s{};
};

template <>
struct glz::meta<my_struct>
{
   using T = my_struct;
   static constexpr auto value = glz::object("num1", &T::num1, "num2", &T::num2, "maybe", &T::maybe, "v3s", &T::v3s);
};

void csv_tests()
{
   "read/write column wise"_test = [] {
      std::string input_col =
         R"(num1,num2,maybe,v3s[0],v3s[1],v3s[2]
11,22,1,1,1,1
33,44,1,2,2,2
55,66,0,3,3,3
77,88,0,4,4,4)";

      my_struct obj{};

      read_csv<colwise>(obj, input_col);

      expect(obj.num1[0] == 11);
      expect(obj.num2[2] == 66);
      expect(obj.maybe[3] == false);
      expect(obj.v3s[0] == std::array{1, 1, 1});
      expect(obj.v3s[1] == std::array{2, 2, 2});
      expect(obj.v3s[2] == std::array{3, 3, 3});
      expect(obj.v3s[3] == std::array{4, 4, 4});

      std::string out{};

      write<opts{.format = csv, .layout = colwise}>(obj, out);
      expect(out ==
             R"(num1,num2,maybe,v3s[0],v3s[1],v3s[2]
11,22,1,1,1,1
33,44,1,2,2,2
55,66,0,3,3,3
77,88,0,4,4,4
)");

      expect(!write_file_csv<colwise>(obj, "csv_test_colwise.csv"));
   };

   "read/write row wise"_test = [] {
      std::string input_row =
         R"(num1,11,33,55,77
num2,22,44,66,88
maybe,1,1,0,0
v3s[0],1,2,3,4
v3s[1],1,2,3,4
v3s[2],1,2,3,4)";

      my_struct obj{};
      read_csv(obj, input_row);

      expect(obj.num1[0] == 11);
      expect(obj.num2[2] == 66);
      expect(obj.maybe[3] == false);
      expect(obj.v3s[0][2] == 1);

      std::string out{};

      write<opts{.format = csv}>(obj, out);
      expect(out ==
             R"(num1,11,33,55,77
num2,22,44,66,88
maybe,1,1,0,0
v3s[0],1,2,3,4
v3s[1],1,2,3,4
v3s[2],1,2,3,4)");

      expect(!write_file_csv(obj, "csv_test_rowwise.csv"));
   };

   "std::map row wise"_test = [] {
      std::map<std::string, std::vector<uint64_t>> m;
      auto& x = m["x"];
      auto& y = m["y"];

      for (size_t i = 0; i < 10; ++i) {
         x.emplace_back(i);
         y.emplace_back(i + 1);
      }

      std::string out{};
      write<opts{.format = csv}>(m, out);
      expect(out == R"(x,0,1,2,3,4,5,6,7,8,9
y,1,2,3,4,5,6,7,8,9,10
)");

      out.clear();
      write<opts{.format = csv}>(m, out);

      m.clear();
      expect(!read<opts{.format = csv}>(m, out));

      expect(m["x"] == std::vector<uint64_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
      expect(m["y"] == std::vector<uint64_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
   };

   "std::map column wise"_test = [] {
      std::map<std::string, std::vector<uint64_t>> m;
      auto& x = m["x"];
      auto& y = m["y"];

      for (size_t i = 0; i < 10; ++i) {
         x.emplace_back(i);
         y.emplace_back(i + 1);
      }

      std::string out{};
      write<opts{.format = csv, .layout = colwise}>(m, out);
      expect(out == R"(x,y
0,1
1,2
2,3
3,4
4,5
5,6
6,7
7,8
8,9
9,10
)");

      out.clear();
      write<opts{.format = csv, .layout = colwise}>(m, out);

      m.clear();
      expect(!read<opts{.format = csv, .layout = colwise}>(m, out));

      expect(m["x"] == std::vector<uint64_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
      expect(m["y"] == std::vector<uint64_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
   };

   "std::unordered_map row wise"_test = [] {
      std::unordered_map<std::string, std::vector<uint64_t>> m;
      auto& x = m["x"];
      auto& y = m["y"];

      for (size_t i = 0; i < 10; ++i) {
         x.emplace_back(i);
         y.emplace_back(i + 1);
      }

      std::string out{};
      write<opts{.format = csv}>(m, out);
      expect(out == R"(y,1,2,3,4,5,6,7,8,9,10
x,0,1,2,3,4,5,6,7,8,9
)" || out == R"(x,0,1,2,3,4,5,6,7,8,9
y,1,2,3,4,5,6,7,8,9,10
)");

      out.clear();
      write<opts{.format = csv}>(m, out);

      m.clear();
      expect(!read<opts{.format = csv}>(m, out));

      expect(m["x"] == std::vector<uint64_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
      expect(m["y"] == std::vector<uint64_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
   };
}

int main() { csv_tests(); }
