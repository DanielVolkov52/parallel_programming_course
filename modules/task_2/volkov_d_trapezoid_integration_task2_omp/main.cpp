// Copyright 2019 Volkov Daniel
#include<omp.h>
#include<iostream>
#include<iomanip>

double oneDimensionalIntegral(double x, double func(double, double), double a, double b, int n);
double Trapezoid(double func(double, double), double ax, double bx,
    double TopLimit(double), double LowerLimit(double), int n) {
    double step = (bx - ax) / n;
    double result = 0;
#pragma omp parallel for reduction(+: result)
    for (int i = 1; i <  n - 1; i++) {
        result += oneDimensionalIntegral(ax + i * step, func, TopLimit(ax + i * step), LowerLimit(ax + i * step), n);
    }
    result *= 2;
    result += oneDimensionalIntegral(ax, func, TopLimit(ax), LowerLimit(ax), n) +
        oneDimensionalIntegral(bx, func, TopLimit(bx), LowerLimit(bx), n);
    result *= (step / 2);
    return result;
}
double oneDimensionalIntegral(double x, double func(double, double), double b, double a, int n) {
    double step = (b - a) / n;
    double result = 0;
    for (int i = 1; i <= n - 1; i++) {
        result += func(x, a + i * step);
    }
    result *= 2;
    result += func(x, a) + func(x, b);
    result *= (step / 2);
    return result;
}
double MainFunction1(double x, double y) {
    return exp(-x * x - y * y);
}
double TopFunction(double x) {
    return exp(-x * x);
}
double LowerFunction(double x) {
    return -exp(-x * x);
}
int main(int argc, char *argv[]) {
    int numThreads, n ;
    if (argc == 1) {
        numThreads = 12;
        n = 10000;
    }
    else
    {
        numThreads = atoi(argv[1]);
        n = atoi(argv[2]);
    }
    double start, SerialTime, ParallelTime;
    omp_set_num_threads(1);
    start = omp_get_wtime();
    std::cout << "Serial result: " << Trapezoid(MainFunction1, 0, 10, TopFunction,
    LowerFunction, n);
    std::cout << "\nSerial Time: " << (SerialTime = (omp_get_wtime() - start));
    omp_set_num_threads(numThreads);
    start = omp_get_wtime();
    std::cout << "\nParallel result: " << Trapezoid(MainFunction1, 0, 10, TopFunction,
    LowerFunction, n);
    std::cout << "\nParallel Time: " << (ParallelTime = (omp_get_wtime() - start));
    std::cout << "\nBoost: " << (SerialTime / ParallelTime) << "\nEfficiency: ";
    std::cout << (SerialTime / ParallelTime) / numThreads;
}
