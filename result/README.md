# result
A C++ implementation of a 'value or error' type inspired by [std::result::Result<T, E>](https://doc.rust-lang.org/std/result/) in [Rust](https://www.rust-lang.org/) programming language.

## Overview
`result` is a C++ implementation of the [std::result::Result<T, E>](https://doc.rust-lang.org/std/result/) type to communicate either a value of an error.

Developers (and the end users of an application) normally want to verify if a function was executed successfully or if an error has occurred at some point. Two frequently used techniques to achieve this goal are exceptions and error / status codes. Using error codes, forces the developer to store data via non const references or pointers provided as function arguments. Status codes can easily be ignored by the developer and provide limited information on what exactly went wrong. Exceptions store more information on what went wrong but are also more complex to use.

Although both techniques have their pros and cons, the `result` type tries to be more practical. It provides easy methods to check if a result is ok or if an error has been registered. `result` also provide methods to chain and transform data stored inside the result.

## How to use the library

`result` is a header only library that doesn't have any external dependencies.

`result` as an interface library doesn't have any external dependencies, the tests depend on the [Catch2](https://github.com/catchorg/Catch2) testing framework. If you already have Catch2 installed on your system, you can tell the build generator system not to automatically install it. By default, it will download and install Catch2.

You can download and install `result` using [git](https://git-scm.com) and  [CMake](https://cmake.org).


### Getting the source code and build steps

The following steps describe how to install the library without using the CMake superbuild structure.

1. Checkout `result` from the git repository:
* ``git clone https://github.com/tomvercaut/result``

  1. Configure `result`:
     * ``cd result/result`` 
     * ``cmake -S . -B build -G <generator> [options]``

     Common build system generators:
     * ``Ninja`` or ``Ninja Multi-Config`` for generating [Ninja](https://ninja-build.org)
              build files \[preferred\].
     * ``Unix Makefiles`` for generating make-compatible parallel makefiles.
     * ``Visual Studio`` for generating Visual Studio projects and
              solutions.
     * ``Xcode`` for generating Xcode projects.
   
     Project specific CMake options:
     * ``-DRESULT_BUILD_TESTS:BOOL=[ON|OFF]``:
       
        Default value: `ON`
        
        If `ON`, the tests will be build.
        
     * ``-DRESULT_BUILD_EXAMPLES:BOOL=[ON|OFF]``:
        
        Default value: `ON`
        
        If `ON`, the example(s) will be build.
     * -DRESULT_GENERATE_DOC:
        
        Default value: `OFF`
        
        If `ON`, documenation will be generated using [Doxygen](https://www.doxygen.nl/index.html). To generate documentation using Doxygen, dot needs to be installed. Optional components are: mscgen and dia. The dot tool is part of [graphviz](https://www.graphviz.org/) and is used to generate diagrams and graphs. Graphviz is an open-source, cross-platform graph drawing toolkit.
     * ``-DCMAKE_PREFIX_PATH:PATH=<path>``:
        
        Semicolon-separated list of directories specifying the paths where `find_package` will search. This option can be omitted if the dependencies are installed on the system paths.
     * Example using Ninja Multi-Config:

    ``cmake -G "Ninja Multi-Config" -S . -B build``


3. ``cmake --build build --target <clean|result|test|all|install> [--config [Debug|Release]] [options]``


4. To run the tests using CMake:

   ``cmake --build build --target test --config [Debug|Release]``

### Example

```c++
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
```
  
## Licence

Licensed under the MIT license ([LICENSE](LICENSE) or [MIT license](https://opensource.org/licenses/MIT))
