#include "src.hpp"
#include <cassert>
#include <utility>

int main() {
    matrix a(2, 2);
    a(1, 0) = 0; a(1, 1) = 2;
    a(2, 0) = 3; a(2, 1) = 4;
    assert(a.determination() == fraction(-6));
    matrix copy(a);
    copy(1, 0) = 1;
    assert(a(1, 0) == fraction(0));
    a = a;
    matrix moved(std::move(copy));
    copy = moved;
    assert(copy(1, 0) == fraction(1));
    matrix product = a * a.transposition();
    assert(product(1, 0) == fraction(4));
    bool caught = false;
    try { a(0, 0); } catch (const matrix_error &) { caught = true; }
    assert(caught);

    int from[] = {1, 1, 2}, to[] = {2, 3, 3};
    fraction r[] = {fraction(1, 2), fraction(1, 4), fraction(2)};
    fraction current[] = {2, 1, -3};
    fraction voltage[] = {1, 2, fraction(1, 2)};
    resistive_network triangle(3, 3, from, to, r);
    assert(triangle.get_equivalent_resistance(1, 2) == fraction(9, 22));
    assert(triangle.get_voltage(2, current) == fraction(10, 11));
    assert(triangle.get_power(voltage) == fraction(33, 8));

    // Complete unit-resistance graphs: R(a,b) = 2/n.
    for (int n = 2; n <= 10; ++n) {
        int f[45], t[45], m = 0;
        fraction resistance[45], currents[10], voltages[10];
        for (int i = 1; i <= n; ++i)
            for (int j = i + 1; j <= n; ++j) {
                f[m] = i; t[m] = j; resistance[m++] = 1;
            }
        resistive_network network(n, m, f, t, resistance);
        currents[0] = 1; currents[n - 1] = -1;
        for (int i = 1; i <= n; ++i) {
            for (int j = i; j <= n; ++j)
                assert(network.get_equivalent_resistance(i, j)
                       == (i == j ? fraction(0) : fraction(2, n)));
            if (i < n) {
                voltages[i - 1] = network.get_voltage(i, currents);
                assert(voltages[i - 1] == fraction(i == 1 ? 2 : 1, n));
            }
        }
        assert(network.get_power(voltages) == fraction(2, n));
    }
    resistive_network single(1, 0, nullptr, nullptr, nullptr);
    fraction v[] = {7};
    assert(single.get_power(v) == fraction(0));
    assert(single.get_equivalent_resistance(1, 1) == fraction(0));
}
