#include <stdio.h>
int x = 3;

void funA();

int main(void){
    int x = 5;
    printf("%d\n", x);
    funA();
}
void funA(){
    printf("%d\n", x);
}
