#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>  //Для PATH_MAX

#define DEBUG

// функцияобработчика сигналов
volatile int cought_signum = -1;
// простой обработчик
void handler(int signum) {
    cought_signum = signum;
    exit(signum);
}

int main(int argc, char* argv[]) {

    // // Буфер в который будет помещен путь
    // // к текущей директории 
    // char PathName[PATH_MAX]; 
    // // Определяем путь к текущей директории  
    // if (readlink("/proc/self/exe", PathName, PATH_MAX) == -1) {
    //     perror("readlink");
    //     return -1;
    // }

    // //! отладка
    // #ifdef DEBUG
    // printf("current path: %s\n", PathName);
    // #endif

    // char* PythonExecutable = strcat(PathName, "Plot.py");
    // if (PythonExecutable == NULL) {
    //     perror("strcat");
    //     return -1;
    // }

    // //! отладка
    // #ifdef DEBUG
    // printf("current python plot executable file: %s\n", PythonExecutable);
    // #endif

    float M = 0.5;
    float L = 2;
    float A = 15;
    float C = 30;
    float psi_0 = 0;
    float theta_0 = M_PI / 3.0;
    float phi_0 = 0;
    float psi_dot_0 = 0.75;
    float theta_dot_0 = 0.1;
    float phi_dot_0 = 0.75;

    char choise[1] = "";
    printf("Would you like to input self parametsr? Yes/no\n");
    if (scanf("%s", choise) == 0) {perror("scanf()"); return -1;};

    if (!strcmp(choise, "Y")) {
        printf("Input paramets:\n"); 

        printf("M: ");           if (scanf("%f", &M) == 0) {perror("scanf()"); return -1;};
        printf("L: ");           if (scanf("%f", &L) == 0) {perror("scanf()"); return -1;};;
        printf("A: ");           if (scanf("%f", &A) == 0) {perror("scanf()"); return -1;};
        printf("C: ");           if (scanf("%f", &C) == 0) {perror("scanf()"); return -1;};
        printf("psi_0: ");       if (scanf("%f", &psi_0) == 0) {perror("scanf()"); return -1;};
        printf("theta_0: ");     if (scanf("%f", &theta_0) == 0) {perror("scanf()"); return -1;};
        printf("phi_0: ");       if (scanf("%f", &phi_0) == 0) {perror("scanf()"); return -1;};
        printf("psi_dot_0: ");   if (scanf("%f", &psi_dot_0) == 0) {perror("scanf()"); return -1;};
        printf("theat_dot_0: "); if (scanf("%f", &theta_dot_0) == 0) {perror("scanf()"); return -1;};
        printf("phi_dot_0: ");   if (scanf("%f", &phi_dot_0) == 0) {perror("scanf()"); return -1;};
    }
    else if (!strcmp(choise, "n")) {
        printf("Default parametrs\n");
    }
    else {
        perror("wrong answer");
        return -1;
    }

    // массив для хранения запроса пользователя
    char ans[sizeof("psi, phi, theta, p, q, r")];
    // массив прочитанных лексем
    char *parametrs[7];
    int params_amount = 0;
    // получаем параметры, которые необходимо вывести
    printf("What parametrs would you like to see?\n");
    if (read(fileno(stdin), ans, sizeof("psi, phi, theta, p, q, r")) == -1) {
        perror("read()");
        return -1;
    }

    //! отладка
    #ifdef DEBUG
    printf("users' inquiry: %s\n", ans);
    #endif

    // делим прочитанный запрос на лексемы
    char* pch = strtok(ans, " ,.-\n");
    if (pch == NULL) {
        perror("strtok()");
        return -1;
    }
    parametrs[params_amount] = pch;
    params_amount++;
    while ((pch = strtok(NULL, " ,.-\n")) != NULL) {
      parametrs[params_amount++] = pch;
    }

    //! отладка
    #ifdef DEBUG
    printf("params amount %d\n", params_amount);
    for (int i = 0; i < params_amount; ++i) {
        printf("\"%s\"\n", parametrs[i]);
    }
    printf("last element is %s\n", parametrs[params_amount-1]);
    #endif

    // докидываем обработчик
    // --------------------------------------------------------------------------------------------
    if (signal(SIGINT, handler) == SIG_ERR){
        perror("signal(SIGINT)");
        return -1;
    } // ^c
    if (signal(SIGTERM, handler)) {
        perror("signal(SIGTERM)");
        return -1;
    } // завершение работы
    // --------------------------------------------------------------------------------------------

    // форкаем дочерние процессы (по количеству запрошеных параметров)
    for (int i = 0; i < params_amount; ++i) {
        int retFork = -1;
        switch (retFork = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0: /* child */
            execl(
                "/home/pavel/workfile/Other_things/Lagrange-s-case-visualization/Plot.py",
                "/Plot.py",
                parametrs[i],
                NULL
            );
            break;
        default:
            break;
        }

        if (retFork == 0)
            break;
    }

    // ждем, пока завершатся все дети
    while(wait(NULL) != -1) {}

}