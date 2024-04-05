#include <stdio.h>
#include <string.h>
#define N 16384
typedef struct {
  char bit[8];
} reg;

typedef struct{
    char Identificador[5];
    char Version[1];
    int Tamano;
} header;


void CargarArchivo(FILE *nombrearchivo, reg *memoria[N], int *DS, header *Header);
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

    }
    return 0;
}

void CargarArchivo(FILE *nombrearchivo, reg * memoria[N], int *DS,header *Header){
    int i=0,j=0;
    int tamano=getFileSize(nombrearchivo);
    unsigned char data[tamano];
          fread(data, sizeof (char), tamano  , nombrearchivo);
    for (int i = 0; i < 5; ++i) {
        Header->Identificador[i] = data[i];
        printf("%c \n", data[i]);
    }
    for (int i=5; i < 6; ++i) {
        Header->Version[0] = data[i];
        printf("%d \n",data[i]);
    }
    for (int i=6; i < 8; ++i)
        printf("%d \n",data[i]);
    for (int i=8; i < tamano; ++i) {
        printf("%02X \n",data[i]);
    }
    printf("Carga completa %d \n",*DS);
}

void LeerMemoria(reg memoria[N], int DS){
    int j;
    for (int i = 0; i < DS; ++i)
    {
        for (j=0 ; j<8;j++)
            printf("%d %c \n", i, (memoria[i]).bit[j]);
        }
}

int getFileSize (FILE* file){
    fseek(file,0,SEEK_END);
    int size= ftell(file);
    fseek(file,0,SEEK_SET);
    return size;
}