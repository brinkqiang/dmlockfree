# dmlockfree

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[![dmlockfree](https://img.shields.io/badge/brinkqiang-dmlockfree-blue.svg?style=flat-square)](https://github.com/brinkqiang/dmlockfree)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/brinkqiang/dmlockfree/blob/master/LICENSE)
[![blog](https://img.shields.io/badge/Author-Blog-7AD6FD.svg)](https://brinkqiang.github.io/)
[![Open Source Love](https://badges.frapsoft.com/os/v3/open-source.png)](https://github.com/brinkqiang)
[![GitHub stars](https://img.shields.io/github/stars/brinkqiang/dmlockfree.svg?label=Stars)](https://github.com/brinkqiang/dmlockfree) 
[![GitHub forks](https://img.shields.io/github/forks/brinkqiang/dmlockfree.svg?label=Fork)](https://github.com/brinkqiang/dmlockfree)

## Build status
| [Linux][lin-link] | [Mac][mac-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![mac-badge]       | ![win-badge]        |

[lin-badge]: https://github.com/brinkqiang/dmlockfree/workflows/linux/badge.svg "linux build status"
[lin-link]:  https://github.com/brinkqiang/dmlockfree/actions/workflows/linux.yml "linux build status"
[mac-badge]: https://github.com/brinkqiang/dmlockfree/workflows/mac/badge.svg "mac build status"
[mac-link]:  https://github.com/brinkqiang/dmlockfree/actions/workflows/mac.yml "mac build status"
[win-badge]: https://github.com/brinkqiang/dmlockfree/workflows/win/badge.svg "win build status"
[win-link]:  https://github.com/brinkqiang/dmlockfree/actions/workflows/win.yml "win build status"

## Intro
dmlockfree

| 测试名称                     | Linux耗时(ms) | Windows耗时(ms) | 差异倍数(Linux为基准) |
|------------------------------|---------------|------------------|----------------------|
| CDMAtomicQueue               | 526           | 4081             | 7.76x                |
| CDMQueue                     | 1367          | 2135             | 1.56x                |
| ConcurrentQueue              | 7375          | 9292             | 1.26x                |
| BlockingConcurrentQueue      | 12041         | 12176            | 1.01x                |
| ThreadSafeQueue              | 6960          | 3771             | 0.54x                |
| ThreadSafeQueue_condition    | 7597          | 3866             | 0.51x                |
| CAtomicQueue                 | 16589         | 20905            | 1.26x                |

## test
```
Running main() from gtest_main.cc
[==========] Running 7 tests from 7 test cases.
[----------] Global test environment set-up.
[----------] 1 test from CDMAtomicQueue
[ RUN      ] CDMAtomicQueue.CDMAtomicQueue
test CDMAtomicQueue 100000000
test CDMAtomicQueue Done total = 4999999950000000
[       OK ] CDMAtomicQueue.CDMAtomicQueue (3550 ms)
[----------] 1 test from CDMAtomicQueue (3551 ms total)

[----------] 1 test from CDMQueue
[ RUN      ] CDMQueue.CDMQueue
test CDMQueue 100000000
test CDMQueue Done total = 4999999950000000
[       OK ] CDMQueue.CDMQueue (1826 ms)
[----------] 1 test from CDMQueue (1827 ms total)

[----------] 1 test from ConcurrentQueue
[ RUN      ] ConcurrentQueue.ConcurrentQueue
test ConcurrentQueue 100000000
test ConcurrentQueue Done total = 4999999950000000
[       OK ] ConcurrentQueue.ConcurrentQueue (7260 ms)
[----------] 1 test from ConcurrentQueue (7260 ms total)

[----------] 1 test from BlockingConcurrentQueue
[ RUN      ] BlockingConcurrentQueue.BlockingConcurrentQueue
test BlockingConcurrentQueue 100000000
test BlockingConcurrentQueue Done total = 4999999950000000
[       OK ] BlockingConcurrentQueue.BlockingConcurrentQueue (9117 ms)
[----------] 1 test from BlockingConcurrentQueue (9118 ms total)

[----------] 1 test from ThreadSafeQueue
[ RUN      ] ThreadSafeQueue.ThreadSafeQueue
test ThreadSafeQueue 100000000
test ThreadSafeQueue Done total = 4999999950000000
[       OK ] ThreadSafeQueue.ThreadSafeQueue (4594 ms)
[----------] 1 test from ThreadSafeQueue (4595 ms total)

[----------] 1 test from ThreadSafeQueue_condition
[ RUN      ] ThreadSafeQueue_condition.ThreadSafeQueue_condition
test ThreadSafeQueue 100000000
test ThreadSafeQueue Done total = 4999999950000000
[       OK ] ThreadSafeQueue_condition.ThreadSafeQueue_condition (4590 ms)
[----------] 1 test from ThreadSafeQueue_condition (4591 ms total)        

[----------] 1 test from CAtomicQueue
[ RUN      ] CAtomicQueue.CAtomicQueue
test CAtomicQueue 100000000
test CAtomicQueue Done total = 4999999950000000
[       OK ] CAtomicQueue.CAtomicQueue (9591 ms)
[----------] 1 test from CAtomicQueue (9592 ms total)       

[----------] Global test environment tear-down
[==========] 7 tests from 7 test cases ran. (40537 ms total)
[  PASSED  ] 7 tests.
```

## Contacts

## Thanks
