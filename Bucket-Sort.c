/*
 ============================================================================
 Name        : Bucket.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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

	if(rank == 0)
	//  Passando esses dados para sua determinadas variaveis
	tamvet = atoi(argv[1]);
	nbuckets = atoi(argv[2]);
	nprocs = atoi(argv[3]);
	flag = atoi(argv[4]);

	// Alocando o vetor a ser ordenado

	int *vetor = (int *) malloc (sizeof(int) * tamvet);
	int *vet_buckets = (int *) malloc (sizeof(int) * nbuckets);

	//Gerando os valores aleatorios em um vetor de  Tamanho = tamvet
	srand(time(NULL));
	for(int i = 0; i < tamvet; i++){
		vetor[i] = rand() % (tamvet-1);
	}

	// Imprimi o Arranjo Inicial
	if (flag == 1) {
		printf("Posicao: ");
		for(int i = 0; i < tamvet; i++){
		printf("%d ", i);
		}
		printf("\n Valor: ");
		for(int i = 0; i < tamvet; i++){
				printf("%d ", vetor[i]);
		}
	}
	//Com MPI a gente manda um x cada processo e bota um if no inicio do arranjo
	// Processamento
	for (int i = 0; i < nbuckets; i++) {
		//  Coloca em cada balde os respectivos numeros certos, mas qnd da resto exato
		int count;
		if (tamvet % nbuckets == 0) {
			count = 0;
			for (int n = 0; n < tamvet; n++) {
				if(vetor[n] > (i*(tamvet/nbuckets)) && vetor[n] > ((i+1)*(tamvet/nbuckets))-1 ) {
					count++;
				}
			}
			vet_buckets[i] = (int *) malloc (sizeof(int) * count);
			int p_bucket = 0;
			for (int n = 0; n < tamvet; n++) {
				if(vetor[n] > i*(tamvet/nbuckets) && vetor[n] > ((i+1)*(tamvet/nbuckets))-1 ) {
					vet_buckets[i][p_bucket] = vetor[n];
				}
			}
		} else {
			count = 0;
			if (i < (tamvet % nbuckets)){

			} else {

			}
		}

		// Ordenando os baldes
		//Bubblue Sort
		 for (int fim = count-1; fim > 0; --fim) {
		        for (int t = 0; t < fim; ++t) {
		            if (vet_buckets[i][t] > vet_buckets[i][t+1]) {
		                int aux = vet_buckets[i][t];
		                vet_buckets[i][t] = vet_buckets[i][t+1];
		                vet_buckets[i][t+1] = aux;
		            }
		        }

		    }

		 // Concatenando os ordenados

	}
	//  MPI_Init(&argc, &argv);
	//  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//  MPI_Finalize();

	return 0;
}/*
 ============================================================================
 Name        : Bucket.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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

	if(rank == 0)
	//  Passando esses dados para sua determinadas variaveis
	tamvet = atoi(argv[1]);
	nbuckets = atoi(argv[2]);
	nprocs = atoi(argv[3]);
	flag = atoi(argv[4]);

	// Alocando o vetor a ser ordenado

	int *vetor = (int *) malloc (sizeof(int) * tamvet);
	int *vet_buckets = (int *) malloc (sizeof(int) * nbuckets);

	//Gerando os valores aleatorios em um vetor de  Tamanho = tamvet
	srand(time(NULL));
	for(int i = 0; i < tamvet; i++){
		vetor[i] = rand() % (tamvet-1);
	}

	// Imprimi o Arranjo Inicial
	if (flag == 1) {
		printf("Posicao: ");
		for(int i = 0; i < tamvet; i++){
		printf("%d ", i);
		}
		printf("\n Valor: ");
		for(int i = 0; i < tamvet; i++){
				printf("%d ", vetor[i]);
		}
	}
	//Com MPI a gente manda um x cada processo e bota um if no inicio do arranjo
	// Processamento
	for (int i = 0; i < nbuckets; i++) {
		//  Coloca em cada balde os respectivos numeros certos, mas qnd da resto exato
		int count;
		if (tamvet % nbuckets == 0) {
			count = 0;
			for (int n = 0; n < tamvet; n++) {
				if(vetor[n] > (i*(tamvet/nbuckets)) && vetor[n] > ((i+1)*(tamvet/nbuckets))-1 ) {
					count++;
				}
			}
			vet_buckets[i] = (int *) malloc (sizeof(int) * count);
			int p_bucket = 0;
			for (int n = 0; n < tamvet; n++) {
				if(vetor[n] > i*(tamvet/nbuckets) && vetor[n] > ((i+1)*(tamvet/nbuckets))-1 ) {
					vet_buckets[i][p_bucket] = vetor[n];
				}
			}
		} else {
			count = 0;
			if (i < (tamvet % nbuckets)){

			} else {

			}
		}

		// Ordenando os baldes
		//Bubblue Sort
		 for (int fim = count-1; fim > 0; --fim) {
		        for (int t = 0; t < fim; ++t) {
		            if (vet_buckets[i][t] > vet_buckets[i][t+1]) {
		                int aux = vet_buckets[i][t];
		                vet_buckets[i][t] = vet_buckets[i][t+1];
		                vet_buckets[i][t+1] = aux;
		            }
		        }

		    }

		 // Concatenando os ordenados

	}
	//  MPI_Init(&argc, &argv);
	//  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//  MPI_Finalize();

	return 0;
}
