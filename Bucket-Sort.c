
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[]) {
	unsigned int tamvet;
	unsigned int nbuckets;
	unsigned int nprocs;
	unsigned int flag;
	int rank;

	//  Recebendo dados via Linha de Comando
	if(argc != 5){
		printf("Uso:\n");
		printf("\t%s <Tamanho Vetor> <Numeros de Buckets> <Processos MPI> <Flag(1 ou 0)> \n", argv[0]);
		printf("Lembrete: Deixe um espaco entre cada numero \n");
		return 1;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(rank == 0)
	//  Passando esses dados para sua determinadas variaveis
	tamvet = atoi(argv[1]);
	nbuckets = atoi(argv[2]);
	nprocs = atoi(argv[3]);
	flag = atoi(argv[4]);

	// Alocando o vetor a ser ordenado

	int *vetor = (int *) malloc (sizeof(int) * tamvet);

	//Gerando os valores aleatorios em um vetor de  Tamanho = tamvet
	srand(time(NULL));
	for(int i = 0; i < tamvet; i++){
		vetor[i] = rand() % (tamvet-1);
	}

	// Imprimi o Arranjo Inicial
	if (flag == 1) {
		printf("Posicao: ")
		for(int i = 0; i < tamvet; i++){
		printf("%d ", i)
		}
		printf("\n Valor: ")
		for(int i = 0; i < tamvet; i++){
				printf("%d ", vetor[i])
		}
	}

	// Processamento

	MPI_Finalize();

	return 0;
}
