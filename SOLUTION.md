# Solution and validation

The network stores its incidence and diagonal conductance matrices, together
with the conductance Laplacian L. Grounding the last node removes the singular
degree of freedom. The inverse B of the remaining Laplacian is computed lazily
by Gauss-Jordan elimination and cached.

- Voltage: multiply the requested row of B by the non-ground currents.
- Resistance: B[a,a] + B[b,b] - B[a,b] - B[b,a], with ground entries zero.
- Power: sum conductance times squared voltage difference over node pairs.

Construction costs O(n^2 + nm + m^2) space and initialization time for the
required dense matrices. Inversion takes O(n^3) time once. Subsequent resistance,
voltage, and power queries take O(1), O(n), and O(n^2), respectively.
The supplied fraction class is unchanged. Matrix storage owns its allocations,
supports deep copies and moves, and releases all rows on destruction.

## Local validation

```sh
g++ -std=c++17 -O1 -g -fsanitize=address,undefined \
    -fno-omit-frame-pointer -I. tests/test.cpp -o /tmp/problem090_test
/tmp/problem090_test
```

Passed the supplied triangle example, complete graphs of sizes 2 through 10,
single-node behavior, determinant pivoting, matrix multiplication, copying,
moving, self-assignment, and index validation. Sanitizers reported no errors.

## Judge results

| Submission | Method | Result | Score |
| --- | --- | --- | --- |
| 879552 | Git URL | Compile error: judge interpreted URL as C++ source | 0 |
| 879556 | Direct C++ source | Accepted, all 20 test points including leak checks | 100 |

Two of five submission attempts used. The client now supports `--source-file`
to handle this judge's Git submission behavior. All pushes were verified by
comparing the local commit with `git ls-remote origin HEAD`.
