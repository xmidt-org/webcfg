# webcfg

webcfg is the client implementation of webconfig.

[![Build Status](https://github.com/xmidt-org/webcfg/workflows/CI/badge.svg)](https://github.com/xmidt-org/webcfg/actions)
[![codecov.io](http://codecov.io/github/xmidt-org/webcfg/coverage.svg?branch=master)](http://codecov.io/github/xmidt-org/webcfg?branch=master)
[![Apache V2 License](http://img.shields.io/badge/license-Apache%20V2-blue.svg)](https://github.com/xmidt-org/webcfg/blob/master/LICENSE.txt)
[![GitHub release](https://img.shields.io/github/release/xmidt-org/webcfg.svg)](CHANGELOG.md)


# Building and Testing Instructions

```
mkdir build
cd build
cmake ..
make
make test
```
# To run webconfig as a standalone binary

```
mkdir build
cd build
cmake .. -DWEBCONFIG_BIN_SUPPORT:BOOL=true
make
make test
```
