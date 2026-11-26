#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <string>
#include <utility>
#include <cstdlib>
#include "fraction.hpp"

class matrix {
private:

    // m行n列的矩阵，用动态二维数组存储，每个元素是分数类实例
    int m, n;
    fraction **data;

    //****************************
    // TODO: 你可以在此添加任何需要的类成员和函数。
    //****************************

public:

    //****************************
    // TODO: 你可以在此添加任何需要的类成员和函数。
    //****************************

    // 默认构造函数
    matrix() {
        m = n = 0;
        data = nullptr;
    }

    // TODO: 构造函数，构建 m_*n_ 的矩阵，矩阵元素设为0。
    matrix(int m_, int n_);

    // TODO: 拷贝构造函数，构建与 obj 完全相同的矩阵。
    matrix(const matrix &obj);

    // TODO: 移动拷贝构造函数。
    matrix(matrix &&obj) noexcept;

    // TODO: 析构函数。
    ~matrix();

    // TODO: 重载赋值号。
    matrix &operator=(const matrix &obj);

    // TODO: 重载括号，返回矩阵的第i行(1-based)、第j列(0-based)的元素的引用。如果 i、j 不合法，抛出 matrix_error 错误。
    fraction &operator()(int i, int j);

    // TODO: 重载乘号，返回矩阵乘法 lhs * rhs 的结果。如果 lhs 的列数与 rhs 的行数不相等，抛出 matrix_error 错误。
    friend matrix operator*(const matrix &lhs, const matrix &rhs);

    // TODO: 返回矩阵的转置。若矩阵为空，抛出 matrix_error 错误。
    matrix transposition();

    // TODO: 返回矩阵的行列式。建议用高斯消元实现。若矩阵不是方阵或为空，抛出 matrix_error 错误。
    fraction determination();
};

class resistive_network {
private:

    // 节点数量 和 接线数量
    int interface_size, connection_size;

    // 矩阵A 和 矩阵C
    matrix adjacency, conduction;
    matrix laplacian, grounded_inverse;
    bool inverse_ready = false;
    void prepare_inverse();

    //****************************
    // TODO: 你可以在此添加任何需要的类成员和函数。
    //****************************

public:

    //****************************
    // TODO: 你可以在此添加任何需要的类成员和函数。
    //****************************

    // TODO: 设置电阻网络，构建矩阵A和C。节点数量为interface_size_，接线数量为connection_size_。
    //       对于 1<=i<=connection_size_，从节点from[i-1]到节点to[i-1]有接线，对应电阻为resistance[i-1]。
    //       保证接线使得电阻网络联通，from[i-1] < to[i-1]，resitance[i-1] > 0，均合法。
    resistive_network(int interface_size_, int connection_size_, int from[], int to[], fraction resistance[]);

    ~resistive_network() = default;

    // TODO: 返回节点 interface_id1 和 interface_id2 (1-based)之间的等效电阻。
    //       保证 interface_id1 <= interface_id2 均合法。
    fraction get_equivalent_resistance(int interface_id1, int interface_id2);

    // TODO: 在给定节点电流I的前提下，返回节点id(1-based)的电压。认为节点interface_size(1-based)的电压为0。
    //       对于 1<=i<=interface_size，节点i(1-based)对应电流为 current[i-1]。
    //       保证 current 使得电阻网络有解，id < interface_size 合法。
    fraction get_voltage(int id, fraction current[]);


    // TODO: 在给定节点电压U的前提下，返回电阻网络的功率。
    //       对于 1<=i<=interface_size，节点i(1-based)对应电压为 voltage[i-1]。
    //       保证 voltage 合法。
    fraction get_power(fraction voltage[]);
};


inline matrix::matrix(int m_, int n_) : m(m_), n(n_), data(nullptr) {
    if (m < 0 || n < 0) throw matrix_error();
    data = new fraction *[m]();
    try {
        for (int i = 0; i < m; ++i) data[i] = new fraction[n];
    } catch (...) {
        for (int i = 0; i < m; ++i) delete[] data[i];
        delete[] data;
        throw;
    }
}

inline matrix::matrix(const matrix &obj) : matrix(obj.m, obj.n) {
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j) data[i][j] = obj.data[i][j];
}

inline matrix::matrix(matrix &&obj) noexcept
    : m(obj.m), n(obj.n), data(obj.data) {
    obj.m = obj.n = 0;
    obj.data = nullptr;
}

inline matrix::~matrix() {
    for (int i = 0; i < m; ++i) delete[] data[i];
    delete[] data;
}

inline matrix &matrix::operator=(const matrix &obj) {
    if (this != &obj) {
        matrix copy(obj);
        std::swap(m, copy.m);
        std::swap(n, copy.n);
        std::swap(data, copy.data);
    }
    return *this;
}

inline fraction &matrix::operator()(int i, int j) {
    if (i < 1 || i > m || j < 0 || j >= n) throw matrix_error();
    return data[i - 1][j];
}

inline matrix operator*(const matrix &lhs, const matrix &rhs) {
    if (lhs.n != rhs.m) throw matrix_error();
    matrix result(lhs.m, rhs.n);
    for (int i = 0; i < lhs.m; ++i)
        for (int k = 0; k < lhs.n; ++k) {
            if (lhs.data[i][k] == fraction(0)) continue;
            for (int j = 0; j < rhs.n; ++j)
                result.data[i][j] = result.data[i][j]
                    + lhs.data[i][k] * rhs.data[k][j];
        }
    return result;
}

inline matrix matrix::transposition() {
    if (m == 0 || n == 0) throw matrix_error();
    matrix result(n, m);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j) result.data[j][i] = data[i][j];
    return result;
}

inline fraction matrix::determination() {
    if (m == 0 || m != n) throw matrix_error();
    matrix work(*this);
    fraction result(1);
    for (int k = 0; k < n; ++k) {
        int pivot = k;
        while (pivot < n && work.data[pivot][k] == fraction(0)) ++pivot;
        if (pivot == n) return fraction(0);
        if (pivot != k) {
            std::swap(work.data[pivot], work.data[k]);
            result = fraction(0) - result;
        }
        const fraction diagonal = work.data[k][k];
        result = result * diagonal;
        for (int i = k + 1; i < n; ++i) {
            const fraction factor = work.data[i][k] / diagonal;
            for (int j = k + 1; j < n; ++j)
                work.data[i][j] = work.data[i][j] - factor * work.data[k][j];
            work.data[i][k] = fraction(0);
        }
    }
    return result;
}

inline resistive_network::resistive_network(int interface_size_, int connection_size_,
        int from[], int to[], fraction resistance[])
    : interface_size(interface_size_), connection_size(connection_size_),
      adjacency(interface_size_, connection_size_),
      conduction(connection_size_, connection_size_),
      laplacian(interface_size_, interface_size_) {
    for (int k = 0; k < connection_size; ++k) {
        adjacency(from[k], k) = fraction(1);
        adjacency(to[k], k) = fraction(-1);
        conduction(k + 1, k) = fraction(1) / resistance[k];
        const fraction g = conduction(k + 1, k);
        const int a = from[k], b = to[k];
        laplacian(a, a - 1) = laplacian(a, a - 1) + g;
        laplacian(b, b - 1) = laplacian(b, b - 1) + g;
        laplacian(a, b - 1) = laplacian(a, b - 1) - g;
        laplacian(b, a - 1) = laplacian(b, a - 1) - g;
    }
}

inline void resistive_network::prepare_inverse() {
    if (inverse_ready) return;
    const int size = interface_size - 1;
    matrix work(size, size), inverse(size, size);
    for (int i = 0; i < size; ++i) {
        inverse(i + 1, i) = fraction(1);
        for (int j = 0; j < size; ++j) work(i + 1, j) = laplacian(i + 1, j);
    }
    // A connected network's grounded Laplacian is positive definite.
    for (int k = 0; k < size; ++k) {
        const fraction pivot = work(k + 1, k);
        if (pivot == fraction(0)) throw resistive_network_error();
        for (int j = 0; j < size; ++j) {
            work(k + 1, j) = work(k + 1, j) / pivot;
            inverse(k + 1, j) = inverse(k + 1, j) / pivot;
        }
        for (int i = 0; i < size; ++i) {
            if (i == k) continue;
            const fraction factor = work(i + 1, k);
            if (factor == fraction(0)) continue;
            for (int j = 0; j < size; ++j) {
                work(i + 1, j) = work(i + 1, j) - factor * work(k + 1, j);
                inverse(i + 1, j) = inverse(i + 1, j) - factor * inverse(k + 1, j);
            }
        }
    }
    grounded_inverse = inverse;
    inverse_ready = true;
}

inline fraction resistive_network::get_equivalent_resistance(int a, int b) {
    if (a == b) return fraction(0);
    prepare_inverse();
    if (b == interface_size) return grounded_inverse(a, a - 1);
    return grounded_inverse(a, a - 1) + grounded_inverse(b, b - 1)
        - grounded_inverse(a, b - 1) - grounded_inverse(b, a - 1);
}

inline fraction resistive_network::get_voltage(int id, fraction current[]) {
    prepare_inverse();
    fraction result(0);
    for (int j = 0; j < interface_size - 1; ++j)
        result = result + grounded_inverse(id, j) * current[j];
    return result;
}

inline fraction resistive_network::get_power(fraction voltage[]) {
    fraction result(0);
    for (int i = 0; i < interface_size; ++i)
        for (int j = i + 1; j < interface_size; ++j) {
            const fraction difference = voltage[i] - voltage[j];
            result = result - laplacian(i + 1, j) * difference * difference;
        }
    return result;
}

#endif //SRC_HPP
