#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

unsigned int tamvet;
unsigned int nbuckets;
unsigned int nprocs;
unsigned int flag;
int rank;

typedef struct Bucket {
	unsigned int min, max, quant;
	int* bucket_vector;
} Bucket;

void printVector(int vector[]);  //  Usado para printar tanto o vetor original desordenado quanto o vetor ordenado posteriormente.
void setRandomValuesToVector(int vector[]);  //  Seta valores aleatorios ao vetor desordenado.
void createInternalBuckets(int *vector, Bucket *buckets);  //  Usado pelos processos MPI para criar buckets (Cada processo cria 1 at� alcan�ar o numero de buckets necessarios).
void setIntervalValuesInBuckets(int *vector, Bucket *buckets);  //  Usado pelos processos MPI para vasculhar o vetor desordenado e colocar os numeros nos buckes segundo o intervalo de tal bucket.
void ordenateBuckets(Bucket *buckets);  //  Usado pelos processos MPI para ordenar cada bucket.
void concatenateBuckets(int *vector, Bucket *buckets);  // Usado pelos processo MPI para concatenar os buckets (Cada processo ir� modificar uma parte do vetor, sem problemas de condi��o de corrida).

int main(int argc, char *argv[]) {
	//  Recebendo dados via Linha de Comando.
	//  Verifica se todos os dados est�o inseridos, caso contrario, pede ao usuario para reexecutar o programa segundo as instru��es.
	if(argc != 4) {
		printf("Uso:\n");
		printf("\t%s <Tamanho Vetor> <Numeros de Buckets> <Flag(1 ou 0)> \n", argv[0]);
		printf("Lembrete: Deixe um espaco entre cada numero \n");
		return 0;
	} else {
		//  Passando dados para sua determinadas variaveis.
		tamvet = atoi(argv[1]);
		nbuckets = atoi(argv[2]);
		flag = atoi(argv[3]);
		//  Verificando condição que o numero de buckets não deve ser maior que o tamanho do vetor
		if(nbuckets > tamvet) {
			printf("ERRO! Numero de buckets nao pode ser maior que o numero de vetores \n");
			return 0;
		}
	}

	//  ************************************************************************
	//  Inicio do ambiente MPI com N processos
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	printf("Processo com rank %d criado\n", rank);

	int *vector = (int *) malloc (sizeof(int) * tamvet);

	//  Um unico processo seta os valores randomico (Visto que seria mais rapido do que separar a tarefa para todos os processo)
	//  Vetor com os valores randomicos é transmitido a todos os processos
	if (rank == 0) {
		setRandomValuesToVector(vector);
		MPI_Bcast(vector, tamvet, MPI_INT, 0, MPI_COMM_WORLD);
	}
	MPI_Bcast(vector, tamvet, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) {
		Bucket *buckets = (Bucket *) malloc (sizeof(Bucket) * nbuckets);  //  Aloca na memoria de todos os processos um vetor de buckets
	}





	MPI_Finalize();
	//  ************************************************************************
	//  Fim do ambiente MPI


	return 0;
}

void createInternalBucket(int *vector, Bucket *bucket) {

}

void createInternalBuckets(int  *vector, Bucket *buckets) {
	int iterator = 0, quant_plus = tamvet % nbuckets, inequal_buckets = 0;

	if (tamvet % nbuckets == 0) {
		iterator = nbuckets;
	} else {
		iterator = nbuckets - quant_plus;
		inequal_buckets = 1;
	}

	for (int i = 0; i < iterator; i++) {
		int count = 0;
		buckets[i].min = (i*(tamvet/nbuckets));
		buckets[i].max = ((i+1)*(tamvet/nbuckets))-1;

		for (int j = 0; j < tamvet; j++) {
			if ((vector[j] >= buckets[i].min) && (vector[j] <= buckets[i].max))
				count++;
		}

		if (count > 0) {
			buckets[i].quant = count;
			buckets[i].bucket_vector = (int *) malloc (sizeof(int) * count);
		} else {
			//  Função deleta bucket
		}
	}

	if (inequal_buckets == 1) {
		for (int i = iterator; i < nbuckets; i++) {
			int count = 0;
			 buckets[i].min = buckets[i-1].max + 1;
			 buckets[i].max =  buckets[i].min + (tamvet/nbuckets);

			for (int j = 0; j < tamvet; j++) {
				if(vector[j] >= buckets[i].min && vector[j] <= buckets[i].max)
					count++;
			}

			if (count > 0) {
				buckets[i].quant = count;
				buckets[i].bucket_vector = (int *) malloc (sizeof(int) * count);
			} else {
				//  Função deleta bucket
			}
		}
	}
}

void setIntervalValuesInBuckets(int *vector, Bucket *buckets) {
	for (int i = 0; i < nbuckets; i++) {
		int pos = 0;
		for (int j = 0; j < buckets[i].quant; j++) {
			for (int k = pos; k < tamvet; k++) {
				pos++;
				if ((vector[k] >= buckets[i].min) && (vector[k] <= buckets[i].max)) {
					buckets[i].bucket_vector[j] = vector[k];
					break;
				}
			}
		}
	}
}

void ordenateBuckets(Bucket *buckets) {
	for (int i = 0; i < nbuckets; i++) {
		for (int fim = buckets[i].quant-1; fim > 0; fim--) {
			for (int t = 0; t < fim; ++t) {
				if (buckets[i].bucket_vector[t] > buckets[i].bucket_vector[t+1]) {
					int aux = buckets[i].bucket_vector[t];
				    buckets[i].bucket_vector[t] = buckets[i].bucket_vector[t+1];
				    buckets[i].bucket_vector[t+1] = aux;
		    }
		  }
	   }
	}
}

void concatenateBuckets(int *vector, Bucket *buckets) {
	int cont = 0;
	for (int i = 0; i < nbuckets; i++) {
		for (int j = 0; j < buckets[i].quant; j++) {
			vector[cont] = buckets[i].bucket_vector[j];
			cont++;
		}
	}
}

void printVector(int vector[]) {
	printf("\nAbaixo será apresentado o vetor de %d posições com seus valores atuais:\n\n", tamvet);
	for(int i = 0; i < tamvet; i++){
			printf("Posição ( %d ), Valor = %d",i, vector[i]);
			printf("\n");
	}
	printf("\n");
}

void setRandomValuesToVector(int vector[]) {
	srand(time(NULL));
	for(int i = 0 ; i < tamvet ; i++)
		vector[i] = rand() % (tamvet-1);
};
