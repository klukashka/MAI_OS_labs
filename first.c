#include <stdio.h>
#include "./include/derivative_1.c"
#include "./include/square_1.c"


int main(){
    int number_of_function;
    printf("First program.\n");
    printf("Enter function number: ");
    scanf("%d", &number_of_function);

    if (number_of_function == 1) {
        float A, deltaX;
        printf("Enter its parameters: ");
        scanf("%f %f", &A, &deltaX);

        float result = Derivative(A, deltaX);
        printf("derivative: %f\n", result);
    } else if (number_of_function == 2) {
        float A, B;
        printf("Enter its parameters: ");
        scanf("%f %f", &A, &B);

        int result = Square(A, B);
        printf("square: %d\n", result);
    } else {
        perror("Error: wrong function number. Should be 1 or 2\n");
        return 1;
    }
}