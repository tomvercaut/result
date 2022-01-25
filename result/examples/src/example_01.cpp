#include <result/result.hpp>
#include <iostream>

class error {
public:
  enum class kind { OutOfBound };

  error(kind k, const std::string &msg) : kind(k), msg(msg) {}
  friend std::ostream &operator<<(std::ostream &os, const error &error);

  kind kind;
  std::string msg;
};


std::ostream &operator<<(std::ostream &os, const error &err) {
  std::string skind;
  switch(err.kind) {
  case error::kind::OutOfBound:
    skind = "OutOfBound";
    break;
  default:
    skind = "Unkown";
  }
  os << "kind: [" << skind << "], msg: " << err.msg;
  return os;
}

result::result<double, error> compute(int i) {
  if (i > 10) {
    return result::err<error>(
        error(error::kind::OutOfBound, "value must be less or equal to 10"));
  }
  return result::ok<double>(3.14);
}

int main() {
  // stores an ok result with an internal data type pointing to double
  auto res = compute(0);
  if (res.is_ok()) {
    std::cout << "result was ok, as expected: " << res.unwrap() << std::endl;
  } else {
    std::cerr << "result was expected to be ok, something went wrong" << std::endl;
  }
  // internal type changes from double to error, memory is reallocated
  res = compute(20);
  if (res.is_err()) {
    std::cout << "result was err, as expected" << std::endl;
    std::cout << "  >> " << res.unwrap_err() << std::endl;
  } else {
    std::cerr << "result was expected to be err, something went wrong" << std::endl;
  }
}