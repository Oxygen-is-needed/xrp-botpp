<div align="center">

# xrp-bot++
##### A improved xrp notification bot.

[![C++](https://img.shields.io/badge/C%2B%2B-blue?style=flat-square&logo=cplusplus)](https://www.stroustrup.com/)
[![libcurl](https://img.shields.io/badge/libcurl-blue?style=flat-square&logo=curl)](https://curl.se/libcurl/)
[![xrp](https://img.shields.io/badge/xrp-black?style=flat-square&logo=xrp)](https://xrpl.org/)
[![tgbot-cpp](https://img.shields.io/badge/telegram-green?style=flat-square&logo=telegram)](https://github.com/reo7sp/tgbot)

<img alt="xrp-bot++ logo" src="img/logo.svg" width=50%></img>
</div>

> [!WARNING]
> 
> Project is not fully completed, nor documented. Proceed at your own risk.

XRP-BOTPP is a bot designed to aggregate and then disseminate inforamtion about
an XRP token across multiple API endpoints.


## Program Usage


>        -d | --disable   - Disable selected method.
>        -e | --enable    - Enable selected method.
>        -h | --help      - Print help text.
>        -k | --key       - Set the API key, by file, by stdin '-',
>                           or by argument. Enables method if not already.
>        -m | --method    - Change method, or use no arguments to list methods.
>        -v | --version   - Print version.


### API Keys

Api keys can be passed via the command line using the `-k` or `--key` flags
followed by the key. But this is relativly insecure.

To add security, when no API key is given, the program will check two
alternative locations before raising a no-key-given error.

The first location is an environment viarble, which follows a specific naming
convention: the prefix "SECRET-" followed by the API name in uppercase letters.

If the environment variable is not found, the system will then look for a file
with a name constructed using the prefix "secret-" followed by the API name
lowercase letters.


## Docs

- Compiling
    - [Compiling on Linux](./docs/compiling/linux.md)



## Contribution

Suggestions and General Contribution are Welcome. This is a learning project so
be prepared to be bombarded with questions, and potentially for it to be
re-written.

## References

- [Deepwiki: tgbot-cpp](https://deepwiki.com/reo7sp/tgbot-cpp)
- [Github: tgbot-cpp](https://github.com/reo7sp/tgbot-cpp)
- [Docs: tgbot-cpp](https://reo7sp.github.io/tgbot-cpp/)

- [Github: {fmt}](https://github.com/fmtlib/fmt)
- [Docs: {fmt}](https://fmt.dev/latest/index.html)

- [Docs: XRPL](https://xrpl.org/docs)
- [Docs: XRPSCAN](https://docs.xrpscan.com/api-documentation/introduction)
- [Docs: XRPSCAN](https://docs.xrpscan.com/api-documentation/introduction)
- [Docs: XRPLMETA](https://xrplmeta.org/docs)
