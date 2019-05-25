// Copyright 2019 Volkov Daniil

#include<iostream>
#include<iomanip>
#include<tbb/tbb.h>

double MainFunction1(double x, double y) {
    return exp(-x * x - y * y);
}
double TopFunction(double x) {
    return exp(-x * x);
}
double LowerFunction(double x) {
    return -exp(-x * x);
}
double oneDimensionalIntegral(double x, double func(double, double), double a, double b, int n);
class Integration {
    double ax, bx;
    double * result;
    int n;
public: Integration( double _ax, double _bx, double* res, int _n) {
    ax = _ax;
    bx = _bx;
    result = res;
    n = _n;
}
        void operator()(const tbb::blocked_range<int>& r) const {
            int begin = r.begin(), end = r.end();
            
            double step = (bx - ax) / n;
            for (int i = begin; i != end; i++)  *result += oneDimensionalIntegral(ax + i * step,
                MainFunction1, TopFunction(ax + i * step), LowerFunction(ax + i * step), n);
        }
};

double Trapezoid(double func(double, double), double ax, double bx,
    double TopLimit(double), double LowerLimit(double), int n) {
    double step = (bx - ax) / n;
    double result = 0;

    for (int i = 1; i < n - 1; i++) {
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

void TBBIntegration(double ax, double bx, double* result, int size,  int grainSize) { 
    tbb::parallel_for(tbb::blocked_range<int>(0, size, grainSize), Integration(ax, bx, result, size));
}

int main(int argc, char *argv[]) {
    int numThreads, n;
    if (argc == 1) {
        numThreads = 12;
        n = 10000;
    }
    else
    {
        numThreads = atoi(argv[1]);
        n = atoi(argv[2]);
    }
    double  SerialTime, ParallelTime;
    tbb::task_scheduler_init init;
    init.initialize(4);
    tbb::tick_count start = tbb::tick_count::now();
    std::cout << "Serial result: " << Trapezoid(MainFunction1, 0, 10, TopFunction,
        LowerFunction, n);
    std::cout << "\nSerial Time: " <<
    (SerialTime = ((tbb::tick_count::now()) - start).seconds());
    double *result;
    start = tbb::tick_count::now();
    TBBIntegration(0, 10, result, 10000, 100);
    std::cout << "\nParallel result: " << *result;
    std::cout << "\nParallel Time: " <<
    (ParallelTime = ((tbb::tick_count::now()) - start).seconds());
    std::cout << "\nBoost: " << (SerialTime / ParallelTime) << "\nEfficiency: ";
    std::cout << (SerialTime / ParallelTime) / numThreads;
}
