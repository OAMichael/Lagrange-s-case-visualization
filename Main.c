#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>  //For PATH_MAX

//#define DEBUG

// Signal handler
volatile int cought_signum = -1;

// Simple handler just for signum
void handler(int signum) {
    cought_signum = signum;

    if(kill(0, SIGKILL) < 0)
        perror("kill()");

    exit(signum);
}

int main(int argc, char* argv[]) {

    double M = 0.5;
    double L = 2;
    double A = 15;
    double C = 30;
    double theta_0 = M_PI / 3.0;
    double psi_0 = 0;
    double phi_0 = 0;
    double theta_dot_0 = 0.1;
    double psi_dot_0 = 0.75;
    double phi_dot_0 = 0.75;

    char choise[1] = "";
    printf("Would you like to input rigid body parameters? Y/N\n");
    if (scanf("%s", choise) == 0) { perror("scanf()"); return -1; };

    if (!strcmp(choise, "y") || !strcmp(choise, "Y")) {
        printf("Input paramets:\n"); 

        printf("M: ");           if (scanf("%lf", &M) == 0)           { perror("scanf()"); return -1; };
        printf("L: ");           if (scanf("%lf", &L) == 0)           { perror("scanf()"); return -1; };
        printf("A: ");           if (scanf("%lf", &A) == 0)           { perror("scanf()"); return -1; };
        printf("C: ");           if (scanf("%lf", &C) == 0)           { perror("scanf()"); return -1; };
        printf("theta_0: ");     if (scanf("%lf", &theta_0) == 0)     { perror("scanf()"); return -1; };
        printf("psi_0: ");       if (scanf("%lf", &psi_0) == 0)       { perror("scanf()"); return -1; };
        printf("phi_0: ");       if (scanf("%lf", &phi_0) == 0)       { perror("scanf()"); return -1; };
        printf("theta_dot_0: "); if (scanf("%lf", &theta_dot_0) == 0) { perror("scanf()"); return -1; };
        printf("psi_dot_0: ");   if (scanf("%lf", &psi_dot_0) == 0)   { perror("scanf()"); return -1; };
        printf("phi_dot_0: ");   if (scanf("%lf", &phi_dot_0) == 0)   { perror("scanf()"); return -1; };
    }
    else if (!strcmp(choise, "n") || !strcmp(choise, "N")) {
        printf("Default parameters\n");
    }
    else {
        perror("Wrong answer");
        return -1;
    }

    // Array of user's request for quantities
    char ans[sizeof("psi, phi, theta, p, q, r")];

    // Other stuff array
    char *parameters[7];
    int params_amount = 0;
    
    // Obtaining requested parameters
    printf("What parametres would you like to see graphed?\n");
    if (read(fileno(stdin), ans, sizeof("psi, phi, theta, p, q, r")) == -1) {
        perror("read()");
        return -1;
    }

    //! Debugging...
    #ifdef DEBUG
    printf("users' inquiry: %s\n", ans);
    #endif

    // Dividing user's answer into needed parameters
    char* pch = strtok(ans, " ,.-\n");
    if (pch == NULL) {
        perror("strtok()");
        return -1;
    }

    parameters[params_amount] = pch;
    params_amount++;
    while ((pch = strtok(NULL, " ,.-\n")) != NULL) {
        parameters[params_amount++] = pch;
    }

    //! Debugging...
    #ifdef DEBUG
    printf("params amount %d\n", params_amount);
    for (int i = 0; i < params_amount; ++i) {
        printf("\"%s\"\n", parameters[i]);
    }
    printf("last element is %s\n", parameters[params_amount - 1]);
    #endif

    // Handling signals
    // --------------------------------------------------------------------------------------------
    if (signal(SIGINT, handler) == SIG_ERR){
        perror("signal(SIGINT)");
        return -1;
    }
    if (signal(SIGTERM, handler)) {
        perror("signal(SIGTERM)");
        return -1;
    }
    // --------------------------------------------------------------------------------------------

    // Forking for graphics representation
    switch(fork()) {
        case -1: 
            perror("fork()");
            exit(EXIT_FAILURE);
            break;

        case 0: /* child */
        {
            // For body parameters
            if(!strcmp(choise, "Y") || !strcmp(choise, "y"))
            {
                char* tmp_buf[10];

                int retVal = 0;

                retVal = asprintf(&tmp_buf[0], "%lf", M);
                retVal = asprintf(&tmp_buf[1], "%lf", L);
                retVal = asprintf(&tmp_buf[2], "%lf", C);
                retVal = asprintf(&tmp_buf[3], "%lf", A);
                retVal = asprintf(&tmp_buf[4], "%lf", theta_0);
                retVal = asprintf(&tmp_buf[5], "%lf", psi_0);
                retVal = asprintf(&tmp_buf[6], "%lf", phi_0);
                retVal = asprintf(&tmp_buf[7], "%lf", theta_dot_0);
                retVal = asprintf(&tmp_buf[8], "%lf", psi_dot_0);
                retVal = asprintf(&tmp_buf[9], "%lf", phi_dot_0);

                if(retVal < 0)
                {
                    perror("asprintf()");
                    return -1;
                }

                execl(
                    "OpenGL.exe",
                    "/OpenGL.exe",
                    tmp_buf[0],
                    tmp_buf[1],
                    tmp_buf[2],
                    tmp_buf[3],
                    tmp_buf[4],
                    tmp_buf[5],
                    tmp_buf[6],
                    tmp_buf[7],
                    tmp_buf[8],
                    tmp_buf[9],
                    NULL);
             }
             else  
                execl(
                    "OpenGL.exe",
                    "/OpenGL.exe",
                    NULL); 
        }
        break;

        default: 
            break;
    }

    sleep(1);

    // Forking children for need parameters
    for (int i = 0; i < params_amount; ++i) {
        int retFork = -1;
        switch (retFork = fork()) {
            case -1:
                perror("fork()");
                exit(EXIT_FAILURE);
                break;
            
            case 0: /* child */
                // Graphing...
                execl(
                    "Plot.py",
                    "/Plot.py",
                    parameters[i],
                    NULL
                );
                break;
            
            default:
                break;
        }

        if (retFork == 0)
            break;
    }

    // Waiting until all children have exited
    while(wait(NULL) != -1) {}

}