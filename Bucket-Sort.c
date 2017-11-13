#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int tamvet;
unsigned int nbuckets;
//unsigned int nprocs;
unsigned int flag;

typedef struct Bucket {
	unsigned int min, max, quant;
	int* bucket_vector;
} Bucket;

void printVector(int vector[]);  //  Usado para printar tanto o vetor original desordenado quanto o vetor ordenado posteriormente.
void setRandomValuesToVector(int vector[]);  //  Seta valores aleatorios ao vetor desordenado.
void createInternalBuckets(int *vector, Bucket *buckets);  //  Usado pelos processos MPI para criar buckets (Cada processo cria 1 at� alcan�ar o numero de buckets necessarios).
void setIntervalValuesInBuckets(int *vector, Bucket *buckets);  //  Usado pelos processos MPI para vasculhar o vetor desordenado e colocar os numeros nos buckes segundo o intervalo de tal bucket.
void ordenateBuckets(Bucket *buckets);  //  Usado pelos processos MPI para ordenar cada bucket.
void concatenateBuckets();  // Usado pelos processo MPI para concatenar os buckets (Cada processo ir� modificar uma parte do vetor, sem problemas de condi��o de corrida).

int main(int argc, char *argv[]) {
	//  Recebendo dados via Linha de Comando.
	//  Verifica se todos os dados est�o inseridos, caso contrario, pede ao usuario para reexecutar o programa segundo as instru��es.
	if(argc != 4){
		printf("Uso:\n");
		printf("\t%s <Tamanho Vetor> <Numeros de Buckets> <Flag(1 ou 0)> \n", argv[0]);
		printf("Lembrete: Deixe um espaco entre cada numero \n");
		return 1;
	}

	//  Passando dados para sua determinadas variaveis.
	tamvet = atoi(argv[1]);
	nbuckets = atoi(argv[2]);
	flag = atoi(argv[3]);

	//  Alocando espa�o na memoria para o vetor desordenado.
	int *vector = (int *) malloc (sizeof(int) * tamvet);

	//int rank;

	//  ************************************************************************
	//  Inicio do ambiente MPI com N processos
	//MPI_Init(&argc, &argv);
	//MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	setRandomValuesToVector(vector);  //  Cada processo MPI seta em um intervalo de posi��es diferentes no vetor desordenado

	if (flag == 1) // && (rank == 0 ))  // Só o rank mestre irá printar
		printVector(vector);

	Bucket *buckets = (Bucket *) malloc (sizeof(Bucket) * nbuckets);  //  Aloca na memoria um vetor de buckets com o numero de buckets requisitado pelo usuario

	createInternalBuckets(vector, buckets);

	setIntervalValuesInBuckets(vector, buckets);

	for (int i = 0; i < nbuckets; i++) {
		if (buckets[i].quant > 0) {
			printf ("\n Bucket (%d) : Minino = %d, Maximo = %d, Quantidade = %d", i , buckets[i].min, buckets[i].max, buckets[i].quant);
			for (int j = 0; j < buckets[i].quant; j++)
				printf ("\n        Valor (%d) = %d", j, buckets[i].bucket_vector[j]);
		}
	}

	printf("\n\n\n");

	ordenateBuckets(buckets);

	for (int i = 0; i < nbuckets; i++) {
		if (buckets[i].quant > 0) {
			printf ("\n Bucket (%d) : Minino = %d, Maximo = %d, Quantidade = %d", i , buckets[i].min, buckets[i].max, buckets[i].quant);
			for (int j = 0; j < buckets[i].quant; j++)
				printf ("\n        Valor (%d) = %d", j, buckets[i].bucket_vector[j]);
		}
	}

	printf("\n\n\n");

	for (int i = 0; i < nbuckets; i++) {
		for (int j = 0; j < buckets[i].quant; j++) {
			printf ("%d\n", buckets[i].bucket_vector[j]);
		}
	}



	//MPI_Finalize();
	//  ************************************************************************
	//  Fim do ambiente MPI


	return 0;
}

void createInternalBuckets(int  *vector, Bucket *buckets) {
	if (tamvet % nbuckets == 0) {  //  Se a quantidade de numeros em cada bucket for igual (Resto da divisao de tamvet por nbuckets = 0)
		for (int i = 0; i < nbuckets; i++) {
			int count = 0;
			for (int n = 0; n < tamvet; n++) {
				if(vector[n] >= (i*(tamvet/nbuckets)) && vector[n] <= ((i+1)*(tamvet/nbuckets))-1 )
					count++;
			}
			if (count > 0) {
				buckets[i].quant = count;
				buckets[i].min = (i*(tamvet/nbuckets));
				buckets[i].max = ((i+1)*(tamvet/nbuckets))-1;
				buckets[i].bucket_vector = (int *) malloc (sizeof(int) * count);
			}
		}
	} else {
		int quant_plus = tamvet % nbuckets;
		for (int i = 0; i < nbuckets - quant_plus; i++) {
			int count = 0;
			for (int n = 0; n < tamvet; n++) {
				if(vector[n] >= (i*(tamvet/nbuckets)) && vector[n] <= ((i+1)*(tamvet/nbuckets))-1 )
					count++;
			}
			if (count > 0) {
				buckets[i].quant = count;
				buckets[i].min = (i*(tamvet/nbuckets));
				buckets[i].max = ((i+1)*(tamvet/nbuckets))-1;
				buckets[i].bucket_vector = (int *) malloc (sizeof(int) * count);
			}
		}
		for (int i = nbuckets - quant_plus; i < nbuckets; i++) {
			int count = 0;
			 buckets[i].min = buckets[i-1].max + 1;
			 buckets[i].max =  buckets[i].min + (tamvet/nbuckets);

			for (int n = 0; n < tamvet; n++) {
				if(vector[n] >= buckets[i].min && vector[n] <= buckets[i].max)
					count++;
			}
			if (count > 0) {
				buckets[i].quant = count;
				buckets[i].bucket_vector = (int *) malloc (sizeof(int) * count);
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

void concatenateBuckets() {

}

void printVector(int vector[]) {
	printf("Abaixo será apresentado o vetor de %d posições com seus valores atuais:\n\n", tamvet);
	for(int i = 0; i < tamvet; i++){
			printf("Posição ( %d ), Valor = %d",i, vector[i]);
			printf("\n");
	}
	printf("\n");
}

void setRandomValuesToVector( int vector[]) {
	srand(time(NULL));
	for(int i = 0 ; i < tamvet ; i++)
		vector[i] = rand() % (tamvet-1);
};
