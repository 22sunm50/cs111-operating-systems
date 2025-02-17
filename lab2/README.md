# You Spin Me Round Robin

This project implements a Round Robin (RR) CPU scheduling algorithm in C using a doubly linked list (TAILQ) to manage processes, ensuring fair execution by enforcing a fixed time quantum. It tracks waiting time and response time, handling process arrivals, quantum expiration, and completion, then outputs the average times for evaluation.

## Building

```shell
make
```

## Running

Example of processes.txt:
```shell
4
1, 0, 7
2, 2, 4
3, 4, 1
4, 5, 4
```

Example of cmd for running ./rr
```shell
./rr processes.txt 3
```

Results :
```shell
Average waiting time: 7.00
Average response time: 2.75
```

## Cleaning up

```shell
make clean
```
