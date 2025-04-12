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

const char* getNombreRegistro(unsigned char reg_id, unsigned char sector) {
    static char buffer[8];
    const char* nombres[] = {"EAX", "EBX", "ECX", "EDX", "EEX", "EFX", "CS", "DS", "IP", "CC", "AC"};

    const char* nombre_base = (reg_id < 11) ? nombres[reg_id] : "R?";
    switch(sector) {
        case 0: snprintf(buffer, sizeof(buffer), "%%%s", nombre_base); break;
        case 1: snprintf(buffer, sizeof(buffer), "%%%sL", nombre_base); break;
        case 2: snprintf(buffer, sizeof(buffer), "%%%sH", nombre_base); break;
        case 3: snprintf(buffer, sizeof(buffer), "%%%sX", nombre_base); break;
    }
    return buffer;
}

void mostrarOperando(unsigned char tipo, reg* memoria, int pos, char* out) {
    switch(tipo) {
        case 0: strcpy(out, ""); break;

        case 1: {
            unsigned char reg_info = memoria[pos].byte;
            unsigned char sector = (reg_info & 0b11000000) >> 6;
            unsigned char reg_id = reg_info & 0b00001111;
            const char* reg_nombre = getNombreRegistro(reg_id, sector);
            snprintf(out, 16, "%s", reg_nombre);
            break;
        }

        case 2: {
            int val = (memoria[pos].byte << 8) | memoria[pos + 1].byte;
            snprintf(out, 16, "%d", val);
            break;
        }

        case 3: {
            unsigned char seg = memoria[pos].byte;
            int offset = (memoria[pos + 1].byte << 8) | memoria[pos + 2].byte;
            const char* segmento = (seg == 0) ? "CS" : (seg == 1) ? "DS" : "S?";
            snprintf(out, 20, "[%s+%d]", segmento, offset);
            break;
        }
    }
}

const char* getMnemonico(unsigned char opcode) {
    switch(opcode) {
        case 0x10: return "MOV";
        case 0x11: return "ADD";
        case 0x12: return "SUB";
        case 0x13: return "SWAP";
        case 0x14: return "MUL";
        case 0x15: return "DIV";
        case 0x16: return "CMP";
        case 0x17: return "SHL";
        case 0x18: return "SHR";
        case 0x19: return "AND";
        case 0x1A: return "OR";
        case 0x1B: return "XOR";
        case 0x1C: return "LDL";
        case 0x1D: return "LDH";
        case 0x1E: return "RND";
        case 0x00: return "SYS";
        case 0x01: return "JMP";
        case 0x02: return "JZ";
        case 0x03: return "JP";
        case 0x04: return "JN";
        case 0x05: return "JNZ";
        case 0x06: return "JNP";
        case 0x07: return "JNN";
        case 0x08: return "NOT";
        case 0x0F: return "STOP";
        default: return "???";
    }
}

void Disassembler(reg memoria[], int tamano_codigo) {
    int ip = 0;

    while (ip < tamano_codigo) {
        unsigned char byte0 = memoria[ip].byte;
        unsigned char tipoA = (byte0 & 0b11000000) >> 6;
        unsigned char tipoB = (byte0 & 0b00110000) >> 4;
        unsigned char opcode = byte0 & 0b00001111;

        const char* mnem = getMnemonico(opcode);

        int tamanio_opA = (tipoA == 0) ? 0 : (tipoA == 1 ? 1 : (tipoA == 2 ? 2 : 3));
        int tamanio_opB = (tipoB == 0) ? 0 : (tipoB == 1 ? 1 : (tipoB == 2 ? 2 : 3));
        int longitud = 1 + tamanio_opA + tamanio_opB;

        printf("[%04X] ", ip);
        for (int i = 0; i < longitud; i++) {
            printf("%02X ", memoria[ip + i].byte);
        }

        printf("   |  %-6s ", mnem);

        char opA_str[32] = "", opB_str[32] = "";
        int offset = 1;

        if (tamanio_opA > 0) {
            mostrarOperando(tipoA, memoria, ip + offset, opA_str);
            offset += tamanio_opA;
        }

        if (tamanio_opB > 0) {
            mostrarOperando(tipoB, memoria, ip + offset, opB_str);
        }

        if (tamanio_opA > 0) {
            printf("%s", opA_str);
            if (tamanio_opB > 0) {
                printf(", %s", opB_str);
            }
        }

        printf("\n");
        ip += longitud;
    }
}
