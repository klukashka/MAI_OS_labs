#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

float (*derivative)(float, float);
int (*square)(float, float);

void* handle_derivative;
void* handle_square;

void solve(int number_of_function) {
    if (number_of_function == -1) {
        printf("Enter function number then: ");
        scanf("%d", &number_of_function);
    }
    if (number_of_function == 1) {
        float A, B;
        printf("Enter its parameters: ");
        scanf("%f %f", &A, &B);

        float result = (*derivative)(A, B);
        printf("derivative: %f\n", result);
    } else if (number_of_function == 2){
        float A, B;
        printf("Enter its parameters: ");
        scanf("%f %f", &A, &B);

        float result = (*square)(A, B);
        printf("square: %f\n", result);
    } else {
        perror("Error: wrong function number. Should be either 1 or 2.\n");
    }
}

int load_libraries() {
    int realization;
    printf("Enter function number or change contracts\n");
    scanf("%d", &realization);

    if (realization == 0) {
        handle_derivative = dlopen("./libraries/libderivative_2.so", RTLD_LAZY);
        handle_square = dlopen("./libraries/libsquare_2.so", RTLD_LAZY);
    } else {
        handle_derivative = dlopen("./libraries/libderivative_1.so", RTLD_LAZY);
        handle_square = dlopen("./libraries/libsquare_1.so", RTLD_LAZY);
    }

    if (!handle_derivative) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    if (!handle_square) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();

    derivative = dlsym(handle_derivative, "Derivative");
    square = dlsym(handle_square, "Square");

    char* error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    if (realization != 0){
        return realization;
    } else {
        return -1;
    }
}

void close_libraries() {
    dlclose(handle_derivative);
    dlclose(handle_square);
}

int main() {
    printf("Second program.\n");
    solve(load_libraries());
    close_libraries();
}