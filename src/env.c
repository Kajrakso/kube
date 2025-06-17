/* shamelessly copied from https://github.com/sebastianotronto/nissy-classic/blob/master/src/env.h */
#include "env.h"

bool  initialized_env = false;
char* tabledir;

void mymkdir(char* d, int m) {
#ifdef _WIN32
    mkdir(d);
#else
    mkdir(d, m);
#endif
}

void init_env(void) {
    char* kubedata  = getenv("KUBEDATA");
    char* localdata = getenv("XDG_DATA_HOME");
    char* home      = getenv("HOME");

    bool read, write;

    if (initialized_env)
        return;

    if (kubedata != NULL)
    {
        tabledir = malloc((strlen(kubedata) + 20) * sizeof(char));
        strcpy(tabledir, kubedata);
    }
    else if (localdata != NULL)
    {
        tabledir = malloc((strlen(localdata) + 20) * sizeof(char));
        strcpy(tabledir, localdata);
        strcat(tabledir, "/kube");
    }
    else if (home != NULL)
    {
        tabledir = malloc((strlen(home) + 20) * sizeof(char));
        strcpy(tabledir, home);
        strcat(tabledir, "/.kube");
    }
    else
    {
        tabledir = malloc(20 * sizeof(char));
        strcpy(tabledir, ".");
    }

    mymkdir(tabledir, 0777);
    strcat(tabledir, "/tables");
    mymkdir(tabledir, 0777);

    read  = !access(tabledir, R_OK);
    write = !access(tabledir, W_OK);

    if (!read)
    {
        fprintf(stderr, "Table files cannot be read.\n");
    }
    else if (!write)
    {
        fprintf(stderr, "Data directory not writable: ");
        fprintf(stderr, "tables can be loaded, but not saved.\n");
    }

    initialized_env = true;
}
