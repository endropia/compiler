# compiler

Бахирева Елизавета Александровна

ДВФУ

Б9120-09.03.03пикд

2022

free pascal

# Запуск

Создать и зайти в директорию

``` 
mkdir build
cd build
```

Собрать проект

```
cmake ..
cmake --build .
```

Проект содержит 2 таргета:

- compiler
- compiler_tests

# Использование

```
compiler [args]
```

Аргументы:

- ``-l`` run lexer
- ``-p`` run parser
