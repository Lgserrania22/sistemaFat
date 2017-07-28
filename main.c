/*(*+-------------------------------------------------------------+
 | UNIFAL – Universidade Federal de Alfenas. |
 | BACHARELADO EM CIENCIA DA COMPUTACAO. |
 | Trabalho..: SIMULACAO DE SISTEMA DE ARQUIVOS FAT |
 | Disciplina: Estrutura de Dados I |
 | Professor.: Luiz Eduardo da Silva |
 | Aluno(s)..: Luis Gustavo da Souza Carvalho |
 |             Pedro Barraza |
 *             José Fernandes |
 | Data......: 27/07/2017 |
 +-------------------------------------------------------------+*)*/
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
int verificaAreaLivre(ptnoSet *Area, int tamArquivo){
    if(!(*Area)) return FALSE; //Caso não exista Area, retorna falso
    int espacoLivre = 0;
    ptnoSet aux = (*Area); //Variável que irá percorrer a lista de area livre
    while(espacoLivre < tamArquivo && aux){
        espacoLivre += (aux->fim - aux->inicio + 1) * 3;//A cada nó, espacoLivre recebe o espaço livre na memória. O +1 é para contar o inicio. O *3 é pra contar os 3 espaços por setor
        aux = aux->prox;//aux recebe o próximo nó
    }
    if(espacoLivre >= tamArquivo){ //Se o espaço livre for maior ou igual ao tamanho do arquivo
        return TRUE; 
    }else{
        return FALSE;
    }
}
/*--------------------------------*/

/*Verifica tamanho do texto do arquivo*/
int tamanhoArquivo(char *texto){
    int tamanho = strlen(texto);
    return tamanho;
}

/*Insere arquivo na lista de arquivos e retorna o ponteiro que aponta para ele*/
ptnoArq alocaArquivo(ptnoArq *arquivo, char *texto, char *nome){
    if(!(*arquivo)){ //Se a lista de arquivos estiver vazia
        (*arquivo) = (ptnoArq *)malloc(sizeof(noArq)); //Aloca espaço para a cabeça da lista
        (*arquivo)->caracteres = tamanhoArquivo(texto); //Recebe o número de caracteres
        strcpy((*arquivo)->nome, nome); //Recebe o nome do arquivo
        (*arquivo)->prox = NULL; //Prox é nulo pois é o primeiro arquivo
        (*arquivo)->setores = NULL; //Setores serão inseridos posteriormente
        return (*arquivo); //Retorna o nó que foi inserido
    }else{//Caso já exista um arquivo
        ptnoArq aux, P = NULL, Q = (*arquivo);
        while(Q && strcmp(nome, Q->nome) > 0){ //Percorre a lista enquanto o nome dado for maior que Q->nome
            P = Q;
            Q = Q->prox;
        }
        if(!P){ //Se P for nulo, significa que o arquivo será inserido no começo da lista
            P = (ptnoArq *)malloc(sizeof(noArq)); //Aloca memória pro arquivo
            strcpy(P->nome, nome); //Insere o nome
            P->caracteres = tamanhoArquivo(texto); //Insere o número de caracteres
            P->setores = NULL; //Setores serão inseridos posteriormente
            P->prox = Q; //P->prox aponta pro Q
            (*arquivo) = P; //P passa a ser a cabeça da lista
            return P; //Retorna o nó inserido
        }else if(!Q){ //Se não existir Q, significa que será inserido no final da lista
            Q = (ptnoArq *)malloc(sizeof(noArq)); //Aloca a memória
            strcpy(Q->nome, nome); //Insere o nome
            Q->caracteres = tamanhoArquivo(texto); //Insere o número de caracteres
            Q->prox = NULL; //Não existe próximo, pois está no final
            Q->setores = NULL; //Setores serão inseridos posteriormente
            P->prox = Q; //P->prox aponta para Q
            return Q; //Retorna o nó inserido
        }else{ //Senão, será inserido no meio da lista
            aux = (ptnoArq *)malloc(sizeof(noArq)); //Aloca memória
            P->prox = aux; //Aux fica depois de P
            aux->prox = Q; //E antes de Q
            strcpy(aux->nome, nome);; //Insere nome
            aux->caracteres = tamanhoArquivo(texto); //Insere o número de caracteres
            aux->setores = NULL; //Setores serão inseridos posteriormente
            return aux; //Retorna o nó inserido
        }
    }
}

/*------------------------------------*/
/*Insere setor na lista de setores ocupados pelo arquivo*/
void alocaSetor(ptnoSet *setores, int inicio, int fim){
    if(!(*setores)){ //Se não existir nenhum setor
        (*setores) = (ptnoSet *)malloc(sizeof(noSet)); //Aloca memória pra cabeça da lista
        (*setores)->inicio = inicio; //Insere o inicio
        (*setores)->fim = fim; //Insere o fim
        (*setores)->prox = NULL; //Não tem próximo, pois é o primeiro elemento
    }else{
        ptnoSet aux, P = NULL, Q = (*setores);
        while(Q && inicio > Q->inicio){ //Enquanto existir Q e o inicio for maior do que Q->inicio, a lista é percorrida
            P = Q;
            Q = Q->prox;
        }
        if(!Q){ //Se no final não existir Q, significa que vai ser inserido no final da lista
            if(P->fim + 1 == inicio){ //Se o P->fim for igual ao inicio
                P->fim = fim; //P->fim recebe fim, pois significa que os setores são contínuos
            }else{ //Se não são contínuos
                Q = (ptnoSet *)malloc(sizeof(noSet)); //Aloca memória pro novo setor
                Q->inicio = inicio; //Insere inicio
                Q->fim = fim; //Insere fim
                Q->prox = NULL; //Não tem próximo, pois está no final da lista
                P->prox = Q; //P->prox aponta para o novo nó
            }
        }else{ //Se existir Q           
            if(!P){ //Se não existir P, significa que irá inserir no começo da lista
                if(fim + 1 == Q->inicio){ //Se o fim + 1 for igual ao Q->inicio, 
                    Q->inicio = inicio; //Q->inicio recebe inicio, pois são contínuos
                }else{ //Se fim + 1 não for igual ao Q->inicio
                    aux = (ptnoSet *)malloc(sizeof(noSet)); //Aloca um novo setor
                    aux->prox = Q; //O novo setor aponta pra Q
                    aux->inicio = inicio; //Recebe inicio
                    aux->fim = fim; //Recebe fim
                    (*setores) = aux; //Novo setor passa a ser a cabeça da lista
                }
            }else{ //Caso exista P
                if(fim + 1 == Q->inicio){ //Se o fim + 1 for igual ao Q->inicio, 
                    Q->inicio = inicio; //Q->inicio recebe inicio, pois são contínuos
                }
                if(P->fim + 1 == inicio){ //Se P->fim + 1 for igual ao inicio, eles são contínuos                    
                    if(P->fim + 1 == Q->inicio){ //Se P->fim + 1 for igual ao Q->inicio, significa que os dois ficaram contínuos com o novo setor livre
                        P->fim = Q->fim; //Nesse caso, P->fim recebe Q->fim
                        P->prox = Q->prox; //P->prox passa a apontar pro nó depois de Q
                        free(Q); //Libera a memória de Q
                    }else{
                        P->fim = fim; // Senão o fim de P passa a ser o fim dado
                    }
                    return TRUE;
                }else{ // Caso P->fim + 1 não seja igual ao inicio, eles não são contínuos e o setor ficará entre P e Q 
                    aux = (ptnoSet *)malloc(sizeof(noSet)); //Aloca memória
                    P->prox = aux; //P->prox aponta para o novo setor
                    aux->prox = Q;// aux->prox aponta pra Q
                    aux->inicio = inicio; //Seta inicio
                    aux->fim = fim; //Seta fim
                }
            }
        }           
    }
}

/*------------------------------------*/
/*Função para preencher os setores na memória*/
void preencheSetor(int inicio, int fim, char *texto,memoria *Memo, int tamArquivo, int *caracteresInseridos){
    int setorAtual = inicio; //Setor atual recebe o inicio
    while(setorAtual <= fim){ //Enquanto ele for menor ou igual ao fim
       int contador = 0;
       while(contador < 3 && *caracteresInseridos < tamArquivo){ //Insere 3 caracteres por setor
           (*Memo)[setorAtual][contador] = texto[(*caracteresInseridos)];
           contador++;
           (*caracteresInseridos)++;
       }
       setorAtual++;
       //A variável caracteresInseridos é passada por referência porque ela irá controlar todos os caracteres inseridos la na função gravarArquivo
    }
}

//Função que substitui os caracteres na memória por ' '
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
    ptnoSet aux;
    int tamArquivo = tamanhoArquivo(texto); //Retorna o número de caracteres do texto      
    int caracteresInseridos = 0; //Variável que será usada para controlar quantos caracteres foram inseridos 
    if(!verificaAreaLivre(Area, tamArquivo)) return FALSE; //Se o tamanho do arquivo for maior que a área livre, a função para
    ptnoArq P = alocaArquivo(&(*arquivo), texto, nome);//Aloca na memória o arquivo, os setores serão inseridos depois
    int numeroSetores = 0;
    if(tamArquivo % 3) numeroSetores = tamArquivo/3 + 1;//Caso o tamanho do arquivo não seja divisível por 3, numeroSetores recebe o tamanho do arquivo dividido por 3 + 1
    else numeroSetores = tamArquivo/3; //Senão ele recebe o tamanho do arquivo divido por 3
    while(numeroSetores){              
        if(numeroSetores > (*Area)->fim - (*Area)->inicio){ //Se o número de setores for maior ou igual a quantidade de setores livres no primeiro nó da area livre     
            alocaSetor(&P->setores, (*Area)->inicio, (*Area)->fim);//Ele aloca o setor usado no arquivo criado anteriormente
            preencheSetor((*Area)->inicio, (*Area)->fim, texto, Memo, tamArquivo, &caracteresInseridos);//Preenche o setor na matriz da Memoria
            numeroSetores = numeroSetores - ((*Area)->fim - (*Area)->inicio) - 1;//Retira do numeroSetores a quantidade de setores que foi inserida no primeiro nó. O -1 foi necessário, pois apenas subtraindo o inicio do fim, não contaria que o inicio foi usado também
            aux = (*Area);//Aux guarda o endereço de Area
            (*Area) = (*Area)->prox;//Como todo o nó foi utilizado, Area aponta para o próximo nó
            free(aux);//Libera a memória do nó que foi utilizado
        }else{//Caso o número de setores seja menor que a area livre do primeiro nó
            int setorFim = (*Area)->inicio + numeroSetores - 1;//Variável que irá dizer qual o último setor que será usado nesse nó. O -1 é pra considerar o inicio que também será usado
            alocaSetor(&P->setores, (*Area)->inicio, setorFim);//Aloca o setor no Arquivo criado anteriormente
            preencheSetor((*Area)->inicio, setorFim, texto, Memo, tamArquivo, &caracteresInseridos);//Preenche a matriz memória com os caracteres do texto
            (*Area)->inicio = setorFim + 1;//Altera o valor do inicio do nó pro setor fim + 1. Sem o +1 ele cairia no último setor utilizado e, nesse caso, queremos o que vem depois dele
            numeroSetores = 0;
        }
    }
    return TRUE;
}

//Função que desaloca os setores após excluir um arquivo e libera a memória 
void desalocaSetores(ptnoArq arquivo, ptnoSet *Area, memoria *Memo){
    ptnoSet aux;
    ptnoSet P = arquivo->setores;//P = o endereço do primeiro nó de setores que o arquivo ocupa. Usei P ao invés do próprio arquivo->setores por motivo de legibilidade
    if(!P){//Se não tiver nenhum setor, para por aqui
        return;
    }
    while(P){//Se tiver ...
        alocaSetor(Area,P->inicio,P->fim);//Aloca a nova área livre
        liberaMemoria(Memo, P->inicio, P->fim);//Libera a memória adicionando ' ' onde tinha caracteres
        aux = P;//Aux recebe P
        P = P->prox;//P recebe o próximo nó de setores ocupados pelo arquivo
        free(aux);//Libera a memória que o setor ocupava 
    }
    return;
}

//Função que remove um arquivo
int removeArquivo(ptnoArq *arquivo, char *nome, ptnoSet *Area, memoria *Memo){
    if(!(*arquivo)){//Se não tiver nenhum arquivo, para por aqui.
        return FALSE;
    }
    ptnoArq P = NULL, Q = (*arquivo), aux;
    while(Q && strcmp(nome, Q->nome) > 0){//Enquanto o nome dado for maior que Q->nome, Q continua percorrendo a lista
        P = Q;
        Q = Q->prox;
    }
    if(!Q){//Se Q terminar nulo, é pq não existe o arquivo
        return FALSE;
    }
    aux = Q;
    Q = Q->prox;
    if(P){//Se existir um P, significa que o arquivo não está na cabeça da lista, sendo assim, P->prox passa a apontar pro arquivo que está logo após o excluído
        P->prox = Q;
    }else{//Senão, a cabeça da lista passa a ser o arquivo logo após o excluido
        (*arquivo) = Q;
    }
    desalocaSetores(aux, Area, Memo);//Chama a função que desaloca os setores e libera a memória
    free(aux);//Libera a memória que o arquivo ocupava
    return TRUE;
}

//Função que apresenta o arquivo solicitado
int apresentaArquivo(ptnoArq arquivo, char *nome, memoria *Memo){
    ptnoSet aux;
    while(arquivo && strcmp(nome, arquivo->nome) > 0){//Enquanto existir arquivo e o nome passado for maior que arquivo->nome, a lista vai percorrendo
        arquivo = arquivo->prox;
    }
    if(!arquivo || strcmp(nome,arquivo->nome) != 0){//Se arquivo terminar nulo, é pq o arquivo não existe
        return FALSE;
    }
    else{
        aux = arquivo->setores; //Aux recebe a cabeça da lista de setores que o arquivo ocupa
        int i, j; //Variáveis que controlam o laço de repetição
        printf("Setores | Conteúdo\n");
        printf("------------------\n");
        while(aux){ //Enquanto tiver setores 
            printf("(%2d,%2d) | ", aux->inicio, aux->fim); //Printa o setor inicio e o fim
            for(i = aux->inicio; i <= aux->fim; i++){ //Depois printa os caracteres dentro dos setores              
                for(j = 0; j < TAM_GRANULO; j++){
                    printf("%c", (*Memo)[i][j]);
                }
            }
            printf("\n");
            aux = aux->prox;//Vai para o próximo setor, se tiver, e repete a operação
        }
    }
    return TRUE;
}

//Função que verifica se existe um arquivo com determinado nome
int existeArquivo(ptnoArq Q, char *nome){
    while(Q && (strcmp(nome, Q->nome) >= 0)){//Enquanto Q existir e o  nome dado for maior ou igual ao Q->nome
        if(strcmp(nome, Q->nome) == 0){//Se o nome for igual, retorna false pra informar que não pode inserir o arquivo
            return FALSE;
        }
        Q = Q->prox;//Senão, Q = Q->prox para percorrer a lista
    }
    return TRUE;//Caso não tenha encontrado um arquivo com o mesmo nome, retorna TRUE para poder inserir um arquivo
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
                if(existeArquivo(Arq, nome)){ //Verifica se o arquivo já existe, caso não, ele começa tentar gravar            
                    printf("nome = %s\n", nome);
                    printf("texto = %s\n", texto);
                    teste = gravaArquivo(&Arq, &Area, texto, nome, &Memo);
                    if(!teste){
                        puts("\nMemória cheia!");
                    }else{
                        puts("\nArquivo gravado com sucesso!");
                    }
                }else{
                    puts ("\nJá existe um arquivo com o mesmo nome!");
                }
                /*
                 * Implementar as chamadas das funcoes pra GRAVAR arquivo
                 */
                break;
            case 'D':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                teste = removeArquivo(&Arq, nome, &Area, &Memo);
                if(teste){
                    puts("\nArquivo removido com sucesso!");
                }else{
                    puts("\nO arquivo não pôde ser removido!");
                }
                break;
            case 'A':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                /*
                 * Implementar as chamadas das funcoes pra APRESENTAR arquivo
                 */
                teste = apresentaArquivo(Arq, nome, &Memo);
                if(!teste){
                    puts("\nArquivo não existe!");
                }
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