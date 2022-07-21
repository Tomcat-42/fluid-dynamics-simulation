#pragma once

#include <algorithm>
#include <cassert>
#include <complex>
#include <cstdint>
#include <cstdlib>

#define N (this->size)

#define ITER 20

#define I(i, j) ((i) + (this->size + 2) * (j))

template<typename T>
class Field {
    std::size_t size;

    T* u;
    T* uP;

    T* v;
    T* vP;

    T* d;
    T* dP;

    T* x;
    T* xP;

public:
    constexpr Field(const std::size_t size): size(size) {
#define FIELD_SIZE ((this->size + 2)* (this->size + 2))
        this->u = new T[FIELD_SIZE];
        this->v = new T[FIELD_SIZE];
        this->d = new T[FIELD_SIZE];
        this->x = new T[FIELD_SIZE];

        this->uP = new T[FIELD_SIZE];
        this->vP = new T[FIELD_SIZE];
        this->dP = new T[FIELD_SIZE];
        this->xP = new T[FIELD_SIZE];
    }

    inline
    void addSource(T* x, T* s, const double& dt){
        for (std::size_t i = 0; i < FIELD_SIZE; i++) x[i] += dt * s[i];
    }

    std::tuple<T&, T&, T&> operator[](const std::size_t& i, const std::size_t& j){
        return {this->dP[I(i + 1, j + 1)], this->uP[I(i + 1, j + 1)], this->vP[I(i + 1, j + 1)]};
    }

    template<std::uint8_t B>
    void diffuse(T* x, T* x0, const T& diff, const T& dt){
        double a { dt * diff * N * N };

        for (std::size_t k = 0; k < ITER; k++){
            for (std::size_t i = 1; i <= N; i++){
                for (std::size_t j = 1; j <= N; j++){
                    x[I(i, j)] =
                        (x0[I(i, j)] + a * (x[I(i - 1, j)] + x[I(i + 1, j)] +
                                            x[I(i, j - 1)] + x[I(i, j + 1)])) / (1 + 4 * a);
                }
            }
            this->setBoundary<B>(x);
        }
    }

    template<std::uint8_t B>
    void advect(T* d, T* d0, T* u, T* v, const T& dt){

        double dt0 { dt * N };

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

        this->setBoundary<B>(d);
    }

    void densityStep(T* x, T* x0, T* u, T* v, const T& diff, const T& dt){
        this->addSource(x, x0, dt);

        std::swap(x, x0);
        this->diffuse<0>(x, x0, diff, dt);

        std::swap(x, x0);
        this->advect<0>(x, x0, u, v, dt);
    }

    void project(T* u, T* v, T* p, T* div){
        double h = 1.0 / N;


        for (std::size_t i = 1; i <= N; i++){
            for (std::size_t j = 1; j <= N; j++){

                div[I(i, j)] = -0.5 * h * (u[I(i + 1, j)] - u[I(i - 1, j)] + v[I(i, j + 1)] - v[I(i, j - 1)]);

                p[I(i, j)] = 0.0f;
            }
        }

        this->setBoundary<0>(div);
        this->setBoundary<0>(p);

        for (std::size_t k = 0; k < ITER; k++){
            for (std::size_t i = 1; i <= N; i++){
                for (std::size_t j = 1; j <= N; j++){
                    p[I(i, j)] = (div[I(i, j)] + p[I(i - 1, j)] + p[I(i + 1, j)] +
                                                 p[I(i, j - 1)] + p[I(i, j + 1)]) / 4;
                }
            }
            this->setBoundary<0>(p);
        }

        for (std::size_t i = 1; i <= N; i++){
            for (std::size_t j = 1; j <= N; j++){
                u[I(i, j)] -= 0.5 * (p[I(i + 1, j)] - p[I(i - 1, j)]) / h;
                v[I(i, j)] -= 0.5 * (p[I(i, j + 1)] - p[I(i, j - 1)]) / h;
            }
        }
        this->setBoundary<1>(u);
        this->setBoundary<2>(v);
    }

    template<std::uint8_t B>
    void setBoundary(T* x){
        for (std::size_t i = 1; i <= N; i++){
            if constexpr (B == 1){
                x[I(0, i)] = -x[I(1, i)];
            } else {
                x[I(0, i)] = x[I(1, i)];
            }

            if constexpr (B == 1){
                x[I(N + 1, i)] = -x[I(N, i)];
            } else {
                x[I(N + 1, i)] = x[I(N, i)];
            }

            if constexpr (B == 2){
                x[I(i, 0)] = -x[I(i, 1)];
            } else {
                x[I(i, 0)] = x[I(i, 1)];
            }

            if constexpr (B == 2){
                x[I(i, N + 1)] = -x[I(i, N)];
            } else {
                x[I(i, N+1)] = x[I(i, N)];
            }
        }
        x[I(0    , 0    )] = 0.5f * (x[I(1, 0    )] + x[I(0    , 1)]);
        x[I(0    , N+1  )] = 0.5f * (x[I(1, N + 1)] + x[I(0    , N)]);
        x[I(N + 1, 0    )] = 0.5f * (x[I(N, 0    )] + x[I(N + 1, 1)]);
        x[I(N + 1, N + 1)] = 0.5f * (x[I(N, N + 1)] + x[I(N + 1, N)]);
    }

    void velocityStep(T* u, T* v, T* u0, T* v0, const T& visc, const T& dt){
        this->addSource(u, u0, dt);
        this->addSource(v, v0, dt);

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

    void step(const T visc, const T diff, const T dt){
        this->velocityStep(this->u, this->v, this->uP, this->vP, visc, dt);
        this->densityStep(this->d, this->dP, this->u, this->v, diff, dt);
    }
};

