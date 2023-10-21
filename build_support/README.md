REF:
- [bustub/build_support at master · cmu-db/bustub](https://github.com/cmu-db/bustub/tree/master/build_support)
- [llvm-project/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py at main · llvm/llvm-project](https://github.com/llvm/llvm-project/blob/main/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py)
- [llvm-project/clang-tools-extra/clang-tidy/tool/clang-tidy-diff.py at main · llvm/llvm-project](https://github.com/llvm/llvm-project/blob/main/clang-tools-extra/clang-tidy/tool/clang-tidy-diff.py)

YOU SHOULD:
```bash
cd build_support
sudo ./packages.sh
```

YOU CAN:
```bash
# after cmake (./build.sh debug|release)
cd build
make format
make check-format
make check-lint       # you can specify checked files
make check-clang-tidy # you can specify checked files regex
make fix-clang-tidy   # you can specify checked files regex
make check-clang-tidy-diff
make fix-clang-tidy-diff
```

SEE MORE INFO [build_support.cmake](./build_support.cmake)