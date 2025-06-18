#include<stdio.h>
#include<math.h>
int main()
{
    double pi = 3.1415926;
    double n = 3;
    double a,b,c;
    double m = 2;
    double g1 = 1.0;
    klee_make_symbolic(&a, sizeof(a), "a");
    klee_make_symbolic(&b, sizeof(b), "b");
    klee_make_symbolic(&c, sizeof(c), "c");

    double temp1 = 1-exp(-10*(pow(b,n-m)-0.5-1.0/(2*n))*(pow(b,n-m)-0.5-1.0/(2*n)));
    g1 += temp1;
    double temp2 = 1-exp(-10*(pow(c,n-m)-0.5-2.0/(2*n))*(pow(c,n-m)-0.5-2.0/(2*n)));
    g1 += temp2;
    double f1 = a;
    double f2 = g1-0.85*a;
    double l = sqrt(2)*f2 - sqrt(2)*f1;
    double z = 1- f1 - f2 + 0.5 * pow(sin(2*pi*l),8);
    if(a>0 && b>0 && c>0 && z>=0){
        printf("yes\n");
        printf("%lf %lf %lf %lf\n",a,b,c,z);
    }
    else{
        printf("no\n");
    }
    printf("*********************************\n");
    return 0;
}
