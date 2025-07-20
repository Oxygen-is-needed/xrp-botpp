# Compiling Xrp-bot++ on Linux

> [!NOTE]
>
> Not fully documented.
> Requirements have not been fully explored.
> Compiling Process may be durascially changed in the near future.

## Requirements

- [fmt library](https://github.com/fmtlib/fmt)
- [libcurl]()
- [tgbot](https://github.com/reo7sp/tgbot-cpp)
- [boost](https://www.boost.org/)

Along with all compilation tools, and other necessary packages of each library.

- g++
- make


## Compile

Before you compile you need to make sure you have the
[json](https://github.com/nlohmann/json) library. If there is no file named
`lib/json.hpp` then run `make get`, to retreive the file.

You could also just do this manually as well, by downloading the `json.hpp` from
the repositories releases.

Then to compile, run `make`.


## Clean

To clean just run `make clean`, to remove the binary.
