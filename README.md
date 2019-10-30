# 2019-10-30

- [x] PA1
- [x] PA2
- [x] PA3
- [x] PA4

To compile and run, run
```bash
cd nemu
make clean
cd tools/qemu-diff/
make
cd -
make
cd ..

cd nexus-am/
make clean
make
cd ..

cd nanos-lite/
make clean
make update
make
make run
c

```

---

# ICS2018 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System) in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to http://nju-ics.gitbooks.io/ics2018-programming-assignment/content/

To initialize, run
```bash
bash init.sh
```

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Nexus-am](https://github.com/NJU-ProjectN/nexus-am)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)
