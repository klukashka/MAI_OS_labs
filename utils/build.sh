gcc -shared -fPIC include/square_1.c -o libraries/libsquare_1.so
gcc -shared -fPIC include/square_2.c -o libraries/libsquare_2.so
gcc -shared -fPIC include/derivative_1.c -o libraries/libderivative_1.so -lm
gcc -shared -fPIC include/derivative_2.c -o libraries/libderivative_2.so -lm