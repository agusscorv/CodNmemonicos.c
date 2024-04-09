#include <stdio.h>
#include <string.h>
//#include "Nmemonicos.h"
#define N 16384

typedef struct {
    unsigned char byte;
} reg;

typedef struct{
    char identificador[5];
    char version[1];
    long int tamano;
} header;

void ConvertidorBinario(int n, int []);
void CargarArchivo(FILE *nombrearchivo, reg memoria[N], int *DS, header *Header);
void LeerMemoria(reg memoria[N], int DS);
int getFileSize (FILE* file);

int main() {
    header Header;
    int DS;  //Data assegment
    reg * memoria[N];   //ram
    FILE * Archivo;
    Archivo=fopen("filename.vmx","rb");
    if (Archivo == NULL)
        printf("Archivo no existe");
    else {
        CargarArchivo(Archivo, memoria, &DS,&Header);
        fclose(Archivo);
        LeerMemoria(memoria,DS);
    }
    return 0;
}

void CargarArchivo(FILE *nombrearchivo, reg memoria[N], int *DS,header *Header){
    int i=0,j;
    int tamano=getFileSize(nombrearchivo);
    unsigned char data[tamano];
    fread(data, sizeof (char), tamano  , nombrearchivo);
    for (int i = 0; i < 5; i++) {
        Header->identificador[i] = data[i];
        printf("%c \n", data[i]);     //identificador
    }
    for (int i=5; i < 6; ++i) {
        Header->version[0] = data[i];
        printf("%d \n",data[i]);     //version
    }
    Header->tamano = data[6] * 256 + data[7];
    printf("%d \n",Header->tamano);


    printf("Memoria: \n");
    j=0;
    for (int i=8; i < tamano; ++i) {        // Cargando la memoria con instrucciones
        printf("%c ",data[i]);
        memoria[j].byte=data[i];
        printf("%04X: %02X \n", i - 8, memoria[j].byte);
        j++;
    }
    *DS=Header->tamano;
    printf("Carga completa %d \n",*DS);
}

void LeerMemoria(reg memoria[N], int DS){
    int j;
    for (int i = 0; i < DS; ++i){
        printf("%d %02X \n", i, memoria[i]);
    }
}

int getFileSize (FILE* file){
    fseek(file,0,SEEK_END);
    int size= ftell(file);
    fseek(file,0,SEEK_SET);
    return size;
}
//            ConvertidorBinario(memoria[i], &byte)
//            if (byte[0]==1)
void ConvertidorBinario(int n, int byte[]){
    int aux=0;
    int a[8],i;
    for(int i=0;n>0;i++)
    {
        a[i]=n%2;
        n=n/2;
    }
    for (int j = i-1; j>=0 ; --j) {
        byte[aux]=a[j];
    }
}

int CantOP(int byte[]){
    int entero;
    int i,j=8;
    for (i=0;i<3;i++){
        entero=byte[i]*j;
        j/=2;
    }
    if (entero==15)
        return 0;
    else
    if (!(entero%2))
        return 2;
    else
        return 1;

}

int CantB(int bitizq,int bitder){
    int i;
    if (bitizq)
        if (bitder)
            return 0;
        else
            return 1;
    else
    if (bitder)
        return 2;
    else
        return 3;
}

int Nfunc(int byte[]){
    int i,numero=0,j=1;
    for (i=0;i<3;i++){
        numero+=j*byte[i];
        j*=2;
    }
    return numero;
}

void OperandoDeMemoria(int byte1[], int byte2, int byte3, int *PosReg, int ){
    *PosReg= Nfunc(byte1);
    OperandoInmediato(byte2,byte3);
}

int OperandoInmediato(int byte1, int byte2){
    return byte1*256+byte2;
}

void OperandoRegistro(int byte[], char Reg[], int *PosReg) {
    if (byte[5]) {
        if (byte[4]) {
            strcpy(Reg,"AX");
        } else
            strcpy(Reg,"AH");
    }
    else {
        if (byte[4]) {
            strcpy(Reg,"AL");
        } else {
            strcpy(Reg,"EAX");
        }
    }
    *PosReg= Nfunc(byte);
}