#include "Cliente.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

int main(){
    char buffer[25];
    pid_t pid = fork();
    Cliente* cl = new Cliente();
    if(pid){
        int fifo = open(NOMBREFIFOSERVIDOR,O_RDONLY);
        ssize_t slect;
        slect = read(fifo,(void*)buffer,8);
        close(fifo);
        unlink(NOMBREFIFOSERVIDOR);
        TCOM tipo;
        TPID pid2 = 0;
        memcpy(&tipo,buffer,sizeof(TCOM));
        memcpy(&pid2,buffer+sizeof(TCOM),sizeof(TPID));
        cout<<slect<<endl;
        cout<<tipo<<endl;
        cout<<pid2<<endl;
        delete cl;
        exit(0);
    }
    else{
        cl->conectarAlServidor();
        cl->desconectar();
        delete cl;
        exit(0);
    }
    return 0;
}
