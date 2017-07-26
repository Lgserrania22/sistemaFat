/*-------------------------------------------------
 * Simulador de FAT - File Allocation Table
 * Luiz Eduardo da Silva
 *-------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define TAM_GRANULO 3
#define TAM_MEMORIA 30
#define TRUE 1
#define FALSE 0
typedef struct noSet * ptnoSet;
typedef struct noSet {
    int inicio, fim;
    ptnoSet prox;
} noSet;
typedef struct noArq *ptnoArq;
typedef struct noArq {
    char nome[13];
    int caracteres;
    ptnoSet setores;
    ptnoArq prox;
} noArq;
typedef char memoria[TAM_MEMORIA][TAM_GRANULO];
void mostraSetores(ptnoSet S, char *n) {
    printf("%s = [", n);
    while (S) {
        printf("(%d,%d)", S->inicio, S->fim);
        S = S->prox;
        if (S) printf(",");
    }
    printf("]\n");
}
void mostraArquivos(ptnoArq A) {
    printf("Arquivos:\n");
    while (A) {
        printf("  %12s, %2d caracter(es).  ", A->nome, A->caracteres);
        mostraSetores(A->setores, "Setores");
        A = A->prox;
    }
    printf("\n");
}
void mostraMemoria(memoria Memo) {
    int i, j;
    for (i = 0; i < TAM_MEMORIA; i++) {
        printf("%3d:[", i);
        for (j = 0; j < TAM_GRANULO - 1; j++)
            printf("%c,", Memo[i][j]);
        printf("%c]", Memo[i][TAM_GRANULO - 1]);
        if ((i + 1) % 10 == 0)
            printf("\n");
    }
}
void inicia(ptnoSet *Area, ptnoArq *Arq, memoria Memo) {
    int i, j;
    *Area = (ptnoSet) malloc(sizeof (noSet));
    (*Area)->inicio = 0;
    (*Area)->fim = TAM_MEMORIA - 1;
    (*Area)->prox = NULL;
    *Arq = NULL;
    for (i = 0; i < TAM_MEMORIA; i++)
        for (j = 0; j < TAM_GRANULO; j++)
            Memo[i][j] = ' ';
}

/*---------------------------------------------
 * Implementar as rotinas para simulacao da FAT
 *---------------------------------------------*/
/*Função para buscar setor livre*/
int verificaAreaLivre(ptnoSet *Area){
    if(!(*Area)) return 0;
    int espacoLivre = 0;
    ptnoSet aux = (*Area);
    while(aux){
        espacoLivre += aux->fim - aux->inicio + 1;
        aux = aux->prox;
    }
    return espacoLivre;
}
/*--------------------------------*/
/*Verifica tamanho do texto do arquivo*/

int tamanhoArquivo(char *texto){
    int tamanho = strlen(texto);
    return tamanho;
}

/*Insere arquivo na lista de arquivos e retorna o ponteiro que aponta para ele*/
ptnoArq insereArquivo(ptnoArq *arquivo, char *texto, char *nome){
    if(!(*arquivo)){
        (*arquivo) = (ptnoArq *)malloc(sizeof(noArq));
        (*arquivo)->caracteres = tamanhoArquivo(texto);
        strcpy((*arquivo)->nome, nome);
        (*arquivo)->prox = NULL;
        (*arquivo)->setores = NULL;
        return (*arquivo);
    }else{
        ptnoArq aux, P = NULL, Q = (*arquivo);
        while(Q && strcmp(nome, Q->nome) > 0){
            P = Q;
            Q = Q->prox;
        }
        if(!P){
            P = (ptnoArq *)malloc(sizeof(noArq));
            strcpy(P->nome, nome);
            P->caracteres = tamanhoArquivo(texto);
            P->prox = NULL;
            P->setores = NULL;
            P->prox = Q;
            (*arquivo) = P;
            return P;
        }else if(!Q){
            Q = (ptnoArq *)malloc(sizeof(noArq));
            strcpy(Q->nome, nome);
            Q->caracteres = tamanhoArquivo(texto);
            Q->prox = NULL;
            Q->setores = NULL;
            P->prox = Q;
            return Q;
        }else{
            aux = (ptnoArq *)malloc(sizeof(noArq));
            P->prox = aux;
            aux->prox = Q;
            strcpy(aux->nome, nome);;
            aux->caracteres = tamanhoArquivo(texto);
            aux->setores = NULL;
            return aux;
        }
    }
}

/*------------------------------------*/
/*Insere setor na lista de setores ocupados pelo arquivo*/
void insereSetor(ptnoSet *setores, int inicio, int fim){
    if(!(*setores)){
        (*setores) = (ptnoSet *)malloc(sizeof(noSet));
        (*setores)->inicio = inicio;
        (*setores)->fim = fim;
        (*setores)->prox = NULL;
    }else{
        ptnoSet aux, P = NULL, Q = (*setores);
        while(Q && inicio > Q->inicio){
            P = Q;
            Q = Q->prox;
        }
        if(!Q){
            Q = (ptnoSet *)malloc(sizeof(noSet));
            Q->inicio = inicio;
            Q->fim = fim;
            Q->prox = NULL;
            P->prox = Q;
        }else{            
            if(!P){
                if(fim + 1 == Q->inicio){
                    Q->inicio = inicio;
                    (*setores) = Q;
                }else{
                    aux = (ptnoSet *)malloc(sizeof(noSet));
                    aux->prox = Q;
                    aux->inicio = inicio;
                    aux->fim = fim;
                    (*setores) = aux;
                }
            }else{
                if(fim + 1 == Q->inicio){
                    Q->inicio = inicio;
                }
                if(P->fim + 1 == inicio){
                    P->fim = fim;
                    if(P->fim == Q->inicio){
                        Q->inicio = P->inicio;
                        if((*setores) == P){
                            (*setores) = Q;
                            free(P);
                        }
                    }
                    return TRUE;
                }else{
                    aux = (ptnoSet *)malloc(sizeof(noSet));
                    P->prox = aux;
                    aux->prox = Q;
                    aux->inicio = inicio;
                    aux->fim = fim;
                }
            }
        }           
    }
}

/*------------------------------------*/
/*Função para preencher os setores na memória*/
void preencheSetor(int inicio, int fim, char *texto,memoria *Memo, int tamArquivo, int *caracteresInseridos){
    int setorAtual = inicio;
    while(setorAtual <= fim){
       int contador = 0;
       while(contador < 3 && *caracteresInseridos < tamArquivo){
           (*Memo)[setorAtual][contador] = texto[(*caracteresInseridos)];
           contador++;
           (*caracteresInseridos)++;
       }
       setorAtual++;
    }
}

void liberaMemoria(memoria *Memo, int inicio, int fim){
    int i;
    for(i = inicio; i <= fim; i++){
        (*Memo)[i][0] = ' ';
        (*Memo)[i][1] = ' ';
        (*Memo)[i][2] = ' ';
    }
}
/*Função para gravar arquivo*/
int gravaArquivo(ptnoArq *arquivo, ptnoSet *Area, char *texto, char *nome, memoria *Memo){
    int tamArquivo = tamanhoArquivo(texto);       
    int caracteresInseridos = 0;
    if(tamArquivo >= verificaAreaLivre(Area) * 3) return FALSE;
    ptnoArq P = insereArquivo(&(*arquivo), texto, nome);
    int numeroSetores = 0;
    if(tamArquivo % 3) numeroSetores = tamArquivo/3 + 1;
    else numeroSetores = tamArquivo/3;
    while(numeroSetores){              
        if(numeroSetores >= (*Area)->fim - (*Area)->inicio){      
            insereSetor(&P->setores, (*Area)->inicio, (*Area)->fim);
            preencheSetor((*Area)->inicio, (*Area)->fim, texto, Memo, tamArquivo, &caracteresInseridos);
            numeroSetores = numeroSetores - (*Area)->fim - (*Area)->inicio - 1;
            (*Area) = (*Area)->prox;
        }else{
            int setorFim = (*Area)->inicio + numeroSetores - 1;
            insereSetor(&P->setores, (*Area)->inicio, setorFim);
            preencheSetor((*Area)->inicio, setorFim, texto, Memo, tamArquivo, &caracteresInseridos);
            (*Area)->inicio = setorFim + 1;
            numeroSetores = 0;
        }
    }
    mostraMemoria((*Memo));
    return TRUE;
}

void desalocaSetores(ptnoArq arquivo, ptnoSet *Area, memoria *Memo){
    ptnoSet P = arquivo->setores;
    if(!P){
        return TRUE;
    }
    while(P){
        insereSetor(Area,P->inicio,P->fim);
        liberaMemoria(Memo, P->inicio, P->fim);
        P = P->prox;
    }
    return TRUE;
}

int removeArquivo(ptnoArq *arquivo, char *nome, ptnoSet *Area, memoria *Memo){
    if(!(*arquivo)){
        return FALSE;
    }
    ptnoArq P = NULL, Q = (*arquivo), aux;
    while(Q && strcmp(nome, Q->nome) > 0){
        P = Q;
        Q = Q->prox;
    }
    if(!Q){
        return FALSE;
    }
    aux = Q;
    Q = Q->prox;
    if(P){
        P->prox = Q;
    }else{
        (*arquivo) = Q;
    }
    desalocaSetores(aux, Area, Memo);
    free(aux);
    return TRUE;
}

int excluiArquivo(){
    
}

/*--------------------------*/
/*
 *
 */
void ajuda() {
    printf("\nCOMANDOS\n");
    printf("--------\n");
    printf("G <arquivo.txt> <texto><ENTER>\n");
    printf(" -Grava o <arquivo.txt> e conteúdo <texto> no disco\n");
    printf("D <arquivo.txt>\n");
    printf(" -Deleta o <arquivo.txt> do disco\n");
    printf("A <arquivo.txt>\n");
    printf(" -Apresenta o conteudo do <arquivo.txt>\n");
    printf("M\n");
    printf(" -Mostra as estruturas utilizadas\n");
    printf("H\n");
    printf(" -Apresenta essa lista de comandos\n");
    printf("F\n");
    printf(" -Fim da simulacao\n");
}
/*------------------------------------------
 * CORPO PRINCIPAL DO PROGRAMA
 *------------------------------------------*/
int main(void) {
    ptnoSet Area, set;
    ptnoArq Arq, ant;
    memoria Memo;
    int teste;
    char com[3];
    char nome[13];
    char texto[TAM_MEMORIA * TAM_GRANULO];
    inicia(&Area, &Arq, Memo);
    do {
        printf("\n=> ");
        scanf("%3s", com);
        com[0] = toupper(com[0]);
        switch (com[0]) {
            case 'G':
                scanf("%s %s", nome, texto);
                printf("nome = %s\n", nome);
                printf("texto = %s\n", texto);
                teste = gravaArquivo(&Arq, &Area, texto, nome, &Memo);
                if(!teste){
                    puts("Deu ruim!");
                }else{
                    puts("Deu bom!");
                }
                /*
                 * Implementar as chamadas das funcoes pra GRAVAR arquivo
                 */
                break;
            case 'D':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                removeArquivo(&Arq, nome, &Area, &Memo);
                break;
            case 'A':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                /*
                 * Implementar as chamadas das funcoes pra APRESENTAR arquivo
                 */
                break;
            case 'M':
                mostraSetores(Area, "Area");
                mostraArquivos(Arq);
                printf ("Memoria:\n");
                mostraMemoria(Memo);
                break;
            case 'H':
                ajuda();
                break;
        }
    } while (com[0] != 'F');
    printf("\nFim da Execucao\n\n");
    return (EXIT_SUCCESS);
}