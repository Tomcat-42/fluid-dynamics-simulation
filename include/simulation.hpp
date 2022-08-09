#pragma once

#undef DEMO

#ifndef DEMO
#define SOURCE_SIZE 150
#endif

#include <algorithm>
#include <cassert>
#include <complex>
#include <cstdint>
#include <cstdlib>

#define N (size)

#define I(i, j) ((j) + (size + 2) * (i))

// more bloat for the bloat king
enum sparam {
    DT,
    DIFF,
    VISC
};

// NOTE: we use size as template parameter to allow the use of static_assert
template<typename T, std::size_t size, std::size_t iter>
class Simulation {

#define FIELD_SIZE ((size + 2) * (size + 2))

    T* u;
    T* uP;

    T* v;
    T* vP;

    T* d;
    T* dP;

    double dt;
    double diff;
    double visc; // NOTE: viscosity is constant throughout the simulation, working on that

public:
    constexpr Simulation(const double dt, const double diff, const double visc): dt(dt), diff(diff), visc(visc){
        // NOTE: according to spec, new will return memory that is aligned to 16, double is aligned to 8
        // the problem is that index 1, is not aligned to 16 and since most of the computation happens relative
        // to index 1 this might cause problems with SSE/AVX
        // TODO: test this
        this->u = new T[FIELD_SIZE];
        this->v = new T[FIELD_SIZE];
        this->d = new T[FIELD_SIZE];

        this->uP = new T[FIELD_SIZE];
        this->vP = new T[FIELD_SIZE];
        this->dP = new T[FIELD_SIZE];
    }

    void test(){
        std::cout << &u[0] << std::endl;
    }

    const T* getU() const {
        return this->u;
    }

    const T* getV() const {
        return this->v;
    }

    const T* getD() const {
        return this->d;
    }

    template<enum sparam P>
    constexpr const double& getParam() const {
        if constexpr (P == sparam::DT) return this->dt;
        else if constexpr (P == sparam::DIFF) return this->diff;
        else if constexpr (P == sparam::VISC) return this->visc;
    }

    // NOTE: this is used in the paper to copy form xP to x, adding the source from the screen
    inline __attribute__((unused))
    void addSource(T* x, T* s, const double& dt){
#pragma omp for simd
        for (std::size_t i = 0; i < FIELD_SIZE; i++) x[i] += dt * s[i];
    }

    // NOTE: call this before step()
    inline __attribute__((always_inline))
    void addSourceFromScreen(std::size_t i, std::size_t j, const double& d, const double& u, const double& v){
        this->dP[I(i + 1, j + 1)] = d;
        this->uP[I(i + 1, j + 1)] = u;
        this->vP[I(i + 1, j + 1)] = v;

        this->d[I(i + 1, j + 1)] += this->dt * d;
        this->u[I(i + 1, j + 1)] += this->dt * u;
        this->v[I(i + 1, j + 1)] += this->dt * v;
    }

    template<std::uint8_t B> inline
    const std::tuple<T&, T&, T&> operator()(const std::size_t& i, const std::size_t& j) const {
        if constexpr (B == 1) return {this->dP[I(i + 1, j + 1)], this->uP[I(i + 1, j + 1)], this->vP[I(i + 1, j + 1)]};
        else return {this->d[I(i + 1, j + 1)], this->u[I(i + 1, j + 1)], this->v[I(i + 1, j + 1)]};
    }

    template<std::uint8_t B> inline
    void diffuse(T* x, T* x0, const T& diff, const T& dt){
        double a { dt * diff * N * N };

        for (std::size_t k = 0; k < iter; k++){
#pragma omp parallel
            {
#pragma omp for simd collapse(2)
                for (std::size_t i = 1; i <= N; i++){
                    for (std::size_t j = 1; j <= N; j++){
                        x[I(i, j)] =
                            (x0[I(i, j)] + a * (x[I(i - 1, j)] + x[I(i + 1, j)] +
                                                x[I(i, j - 1)] + x[I(i, j + 1)])) / (1 + 4 * a);
                    }
                }
            }
            this->setBoundary<B>(x);
        }
    }

    template<std::uint8_t B> inline
    void advect(T* d, T* d0, T* u, T* v, const T& dt){

        double dt0 { dt * N };

#pragma omp parallel
        {
#pragma omp for simd collapse(2)
            for (std::size_t i = 1; i <= N; i++){
                for (std::size_t j = 1; j <= N; j++){
                    double x {std::clamp((double)i - dt0 * u[I(i, j)], (double)0.5, (double)N + 0.5)};
                    double y {std::clamp((double)j - dt0 * v[I(i, j)], (double)0.5, (double)N + 0.5)};

                    std::size_t i0 {(std::size_t)x};
                    std::size_t i1 {i0 + 1};
                    std::size_t j0 {(std::size_t)y};
                    std::size_t j1 {j0 + 1};

                    double s1 {x - i0};
                    double s0 {1 - s1};
                    double t1 {y - j0};
                    double t0 {1 - t1};

                    d[I(i, j)] =
                        s0 * (t0 * d0[I(i0, j0)] + t1 * d0[I(i0, j1)]) +
                        s1 * (t0 * d0[I(i1, j0)] + t1 * d0[I(i1, j1)]);
                }
            }
        }
        this->setBoundary<B>(d);
    }

    void densityStep(T* x, T* x0, T* u, T* v, const T& diff, const T& dt){
         // this->addSource(x, x0, dt);
#ifndef DEMO
#pragma omp for simd collapse(2)
         for (std::size_t i = 10; i < 50; i++)
            for (std::size_t j = (N / 2) - SOURCE_SIZE; j <= (N / 2) + SOURCE_SIZE; j++){
                x0[I(i, j)] = .5;
                x[I(i, j)] += dt * .5;
            }
#endif // DEMO

        std::swap(x, x0);
        this->diffuse<0>(x, x0, diff, dt);

        std::swap(x, x0);
        this->advect<0>(x, x0, u, v, dt);
    }

    inline
    void project(T* u, T* v, T* p, T* div){
        double h = 1.0 / N;

#pragma omp parallel
        {
#pragma omp for simd collapse(2)
            for (std::size_t i = 1; i <= N; i++){
                for (std::size_t j = 1; j <= N; j++){
                    div[I(i, j)] = -0.5 * h * (u[I(i + 1, j)] - u[I(i - 1, j)] + v[I(i, j + 1)] - v[I(i, j - 1)]);
                    p[I(i, j)] = 0.0f;
                }
            }
        }

        this->setBoundary<0>(div);
        this->setBoundary<0>(p);

        for (std::size_t k = 0; k < iter; k++){
#pragma omp parallel
            {
#pragma omp for simd collapse(2)
                for (std::size_t i = 1; i <= N; i++){
                    for (std::size_t j = 1; j <= N; j++){
                        p[I(i, j)] = (div[I(i, j)] + p[I(i - 1, j)] + p[I(i + 1, j)] +
                                                     p[I(i, j - 1)] + p[I(i, j + 1)]) / 4;
                    }
                }
            }
            this->setBoundary<0>(p);
        }

#pragma omp parallel
        {
#pragma omp for simd collapse(2)
            for (std::size_t i = 1; i <= N; i++){
                for (std::size_t j = 1; j <= N; j++){
                    u[I(i, j)] -= 0.5 * (p[I(i + 1, j)] - p[I(i - 1, j)]) / h;
                    v[I(i, j)] -= 0.5 * (p[I(i, j + 1)] - p[I(i, j - 1)]) / h;
                }
            }
        }
        this->setBoundary<1>(u);
        this->setBoundary<2>(v);
    }

    template<std::uint8_t B> inline
    void setBoundary(T* x){
#pragma omp for simd
        for (std::size_t i = 1; i <= N; i++){
            if constexpr (B == 1){
                x[I(0    , i)] = -x[I(1, i)];
                x[I(N + 1, i)] = -x[I(N, i)];
            } else {
                x[I(0    , i)] = x[I(1, i)];
                x[I(N + 1, i)] = x[I(N, i)];
            }

            if constexpr (B == 2){
                x[I(i, 0    )] = -x[I(i, 1)];
                x[I(i, N + 1)] = -x[I(i, N)];
            } else {
                x[I(i, 0    )] = x[I(i, 1)];
                x[I(i, N + 1)] = x[I(i, N)];
            }

            x[I(0    , 0    )] = 0.5f * (x[I(1, 0    )] + x[I(0    , 1)]);
            x[I(0    , N+1  )] = 0.5f * (x[I(1, N + 1)] + x[I(0    , N)]);
            x[I(N + 1, 0    )] = 0.5f * (x[I(N, 0    )] + x[I(N + 1, 1)]);
            x[I(N + 1, N + 1)] = 0.5f * (x[I(N, N + 1)] + x[I(N + 1, N)]);
        }
    }

    void velocityStep(T* u, T* v, T* u0, T* v0, const T& visc, const T& dt){
        // this->addSource(u, u0, dt);
        // this->addSource(v, v0, dt);

#ifndef DEMO
        static_assert((N / 2) - SOURCE_SIZE > 0, "The simulation size is too small");

#pragma omp for simd collapse(2)
        for (std::size_t i = 10; i < 20; i++)
            for (std::size_t j = (N / 2) - SOURCE_SIZE; j <= (N / 2) + SOURCE_SIZE; j++){
                u0[I(i, j)] = 1.0;
                v0[I(i, j)] = 0.0;
                u[I(i, j)] += dt * 1.0;
                v[I(i, j)] += dt * 0.0;
            }
#endif // DEMO

        std::swap(u, u0);
        this->diffuse<1>(u, u0, visc, dt);

        std::swap(v, v0);
        this->diffuse<2>(v, v0, visc, dt);

        this->project(u, v, u0, v0);
        std::swap(u, u0);
        std::swap(v, v0);

        this->advect<1>(u, u0, u0, v0, dt);
        this->advect<2>(v, v0, u0, v0, dt);

        this->project(u, v, u0, v0);
    }

    void step(){
        this->velocityStep(this->u, this->v, this->uP, this->vP, this->visc, this->dt);
        this->densityStep(this->d, this->dP, this->u, this->v, this->diff, this->dt);

// NOTE: this might be faster, don't know, but the idea is to keep things in cache
#pragma omp parallel
        {
#pragma omp for simd
            for (std::size_t i = 0; i < FIELD_SIZE; i++) this->dP[i] = 0.0;
#pragma omp for simd
            for (std::size_t i = 0; i < FIELD_SIZE; i++) this->uP[i] = 0.0;
#pragma omp for simd
            for (std::size_t i = 0; i < FIELD_SIZE; i++) this->vP[i] = 0.0;
        }
    }
};

