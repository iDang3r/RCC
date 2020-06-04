# RCC
Rusin Compiler Collection

Данный проект включает в себя следующие модули:
- [ASM](#asm)
- [CPU](#cpu)
- [DisASM](#disasm)
- [My_Lang](#my_lang)

Рассмотрим их поподробнее:
## ASM

Представляет из себя транслятор из моего ассемблера в бинарный код [эмулятора](#cpu).

#### Синтаксис

Все команды работают с аргументами из стека

Поддержка точности: 2 знака после запятой

Поддержка регистров: ax, bx, cx, dx, di, si, r8-15

 Команда  |      Действие
----------|---------------------
`end     `| завершение программы
`push    `| положить значение в стек
`add     `| сложить 
`sub     `| вычесть
`mul     `| перемножить
`div     `| разделить
`in      `| ввод числа с консоли
`out     `| вывод числа в консоль
`sqr     `| возведение в квадрат
`sqrt    `| извлечение квадратного корня
`pushr   `| запушить значение из регистра
`popr    `| вытащить значение в регистр
`pushm   `| запушить значение из памяти
`popm    `| вытащить значение в память
`pushv   `| запушить значение из видеопамяти
`popv    `| вытащить значение в видеопамять
`pic     `| вывести в консоль изображение(видеопамять)
`circle  `| записать в видеопамять окружность(по радиусу)
`inc     `| инкремент
`call    `| вызов функции
`ret     `| возвращение из функции
`'name': `| метка
`jmp     `| безусловнй jump
`ja, je..`| условные джампы 

> Пример простой программы, решающей [квадратное уравнене](https://github.com/iDang3r/RCC/blob/master/Examples/SquareSolver.txt), а так же нахождение факториала рекурсивно

```asm
jmp main

fact:
 popr ax
 push 1
 pushr ax
 jae stop_fact
 
 pushr ax
 pushr ax 
 push -1
 add
 call fact
 mul
ret

 stop_fact:
 push 1
ret

main:
 push 6
 call fact
 out

```

В результате работы программы создается файл `cpu_code.bin`, который можно *скормить* эмулятору 

[Назад в меню](#rcc)

## CPU

Представляет из себя эмулятор процессора, принимающий бинарный файл `cpu_code.bin`, созданный [ASM'ом](#asm).

> Пример работы программы, решающей квадратное уравнение

<img src="https://github.com/iDang3r/RCC/blob/master/Images/SqrtSolver%20console.png" width="600">

[Назад в меню](#rcc)

## DisASM


[Назад в меню](#rcc)

## My_Lang

[Назад в меню](#rcc)
