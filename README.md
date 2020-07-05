It's a simple, C99-compatible ray tracer for my rendering side projects.

OpenMP is used for multithreading. If OpenMP isn't present, it will compile and run single-threaded.

TODO:
- [x] Add Lambertian diffuse material
- [ ] Add dielectric/glass material
- [ ] Replace OpenMP with custom parallel for
- [ ] Switch from recursion to iteration
- [ ] Support incremental image output
