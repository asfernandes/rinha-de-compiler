<div align="center">

![banner]

[<img src="https://img.shields.io/badge/Discord-7289DA?style=for-the-badge&logo=discord&logoColor=white">](https://discord.gg/e8EzgPscCw)

</div>

## Rinha Interpreter written in C++ using ANTLR4, vcpkg and cmake

### About

This interpreter parses the Rinha source code (not the JSON AST!) and executes it.
You can read more about it [here](https://github.com/aripiprazole/rinha-de-compiler).

### How to build the docker image

```bash
docker build -t rinha-de-compiler .
```

### How to use the docker image

```bash
# Run source.rinha from the current directory
docker run --rm -v .:/var/rinha rinha-de-compiler
```

[banner]: ./img/banner.png
