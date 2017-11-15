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
void createInternalBuckets(int  *vector, Bucket *buckets, int nbuckets_aux, int actual);  // Usado pelos processos MPI para setar os atributos MAX e MIN e QUANT de seus buckets
void setIntervalValuesInBuckets(int *vector, Bucket *buckets);  //  Usado pelos processos MPI para vasculhar o vetor desordenado e colocar os numeros nos buckes segundo o intervalo de tal bucket.
void ordenateBuckets(Bucket *buckets);  //  Usado pelos processos MPI para ordenar cada bucket.
void concatenateBuckets(int *vector, Bucket *buckets);  // Usado pelos processo MPI para concatenar os buckets (Cada processo ir� modificar uma parte do vetor, sem problemas de condi��o de corrida).
int getNumOfBuckets();  //  Usado pelos processos MPI para criar buckets (Cada processo cria 1 at� alcan�ar o numero de buckets necessarios).
int checkParameters(int argc, int tam_vet, int nbuck, char *program_name);  //  Usada para verificar se o inicio da execução e os parametros passados estão corretos

int main(int argc, char *argv[]) {
	//  Recebendo dados via Linha de Comando.
	//  Verifica se todos os dados est�o inseridos, caso contrario, pede ao usuario para reexecutar o programa segundo as instru��es.
	if (checkParameters(argc, atoi(argv[1]), atoi(argv[2]), argv[0]) == 1)
		return 0;

	//  Passando dados para sua determinadas variaveis.
	tamvet = atoi(argv[1]);
	nbuckets = atoi(argv[2]);
	flag = atoi(argv[3]);

	//  ************************************************************************
	//  Inicio do ambiente MPI com N processos
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	int *vector = (int *) malloc (sizeof(int) * tamvet);

	//  Um unico processo seta os valores randomico (Visto que seria mais rapido do que separar a tarefa para todos os processo)
	//  Vetor com os valores randomicos é transmitido a todos os processos
	if (rank == 0) {
		setRandomValuesToVector(vector);
		MPI_Bcast(vector, tamvet, MPI_INT, 0, MPI_COMM_WORLD);
	} else {
		MPI_Bcast(vector, tamvet, MPI_INT, 0, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	//  Cada processo cria um numero de buckets
	int nbuckets_aux = getNumOfBuckets();
	Bucket *buckets = (Bucket *) malloc (sizeof (Bucket) * nbuckets_aux);
	//  Deleta um bucket de um processo caso o nbuckets_aux deste seja 0 (Ou seja, ele não necessita ter nenhum bucket)
	if (nbuckets_aux == 0) {
		free (buckets);
		buckets = NULL;
	}

	//  Cada processo seta o minimo, maximo, quantidade e o vetor de inteiros nos seus buckets (Se ele possuir buckets);
	if (nbuckets_aux > 0) {
		int num_ant = 0, num_atual = 0;
		if (rank > 0)
			MPI_Recv(&num_ant, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		num_atual = num_ant + nbuckets_aux;

		createInternalBuckets(vector, buckets, nbuckets_aux, num_ant);

		if (rank < nprocs-1)
			MPI_Send(&num_atual, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	//  ************************************************************************
	//  Fim do ambiente MPI


	return 0;
}

void createInternalBuckets(int  *vector, Bucket *buckets, int nbuckets_aux, int actual) {
	int iterator = 0, quant_plus = tamvet % nbuckets;

	if (tamvet % nbuckets == 0)
		iterator = nbuckets;
	else
		iterator = nbuckets - quant_plus;

	int index = 0;
	for (int i = actual; i < actual + nbuckets_aux; i++) {
		if (i < iterator) {
			int count = 0;
			buckets[index].min = (i*(tamvet/nbuckets));
			buckets[index].max = ((i+1)*(tamvet/nbuckets))-1;
			for (int j = 0; j < tamvet; j++) {
				if ((vector[j] >= buckets[index].min) && (vector[j] <= buckets[index].max))
					count++;
			}
			if (count > 0) {
				buckets[index].quant = count;
				buckets[index].bucket_vector = (int *) malloc (sizeof(int) * count);
			} else {
			//  Função deleta bucket
			}

			if ((rank < nprocs-1) && (i == iterator - 1) && (index == nbuckets_aux - 1))
				MPI_Send(&buckets[index].max, 1, MPI_INT, rank+1, 1, MPI_COMM_WORLD);
		} else {
			int max_ant, count = 0;

			if (index == 0)
				MPI_Recv(&max_ant, 1, MPI_INT, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			 if (index > 0)
				 buckets[index].min = buckets[index-1].max + 1;
			 else
				 buckets[index].min = max_ant + 1;
			 buckets[index].max =  buckets[index].min + (tamvet/nbuckets);

			 if ((index == nbuckets_aux - 1) && (rank < nprocs -1))
				 MPI_Send(&buckets[index].max, 1, MPI_INT, rank+1, 1, MPI_COMM_WORLD);

			for (int j = 0; j < tamvet; j++) {
				if(vector[j] >= buckets[index].min && vector[j] <= buckets[index].max)
					count++;
			}

			if (count > 0) {
				buckets[index].quant = count;
				buckets[index].bucket_vector = (int *) malloc (sizeof(int) * count);
			} else {
				//  Função deleta bucket
			}

		}
		if (flag == 1)
			printf("Rank ( %d ) setou Min = %u e Max = %u no Bucket %d\n",rank, buckets[index].min, buckets[index].max, i);
		index++;
	}
}

int getNumOfBuckets() {
	int qnt;
	if (nprocs < nbuckets) {
		qnt = nbuckets/nprocs;
		if (nbuckets % nprocs != 0) {
			int quant_plus = nbuckets % nprocs, min, max;
			int iterator = nprocs - quant_plus;
			if (rank < iterator) {
				min = (rank*(nbuckets/nprocs));
				max = ((rank+1)*(nbuckets/nprocs))-1;
				if (rank == ((nprocs - quant_plus)-1))
					MPI_Send(&max, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			} else {
				MPI_Recv(&min, 1, MPI_INT, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				min = min + 1;
				max =  min + (nbuckets/nprocs);
				if (rank < (nprocs-1))
					MPI_Send(&max, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			}
			qnt = (max-min)+1;
		}
	} else {
		if (rank < nbuckets)
			qnt = 1;
		else
			qnt = 0;
	}
	if (flag == 1)
		printf("Processo Rank ( %d ) criou ( %d ) bucket(s)\n",rank, qnt);
	return qnt;
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

int checkParameters(int argc, int tam_vet, int nbuck, char *program_name) {
	int resp = 0;
	if(argc != 4) {
		printf("Uso:\n");
		printf("\t%s <Tamanho Vetor> <Numeros de Buckets> <Flag(1 ou 0)> \n", program_name);
		printf("Lembrete: Deixe um espaco entre cada numero \n");
		resp = 1;
	} else {
		if(nbuck > tam_vet) {
			printf("ERRO! Numero de buckets nao pode ser maior que o numero de vetores \n");
			resp = 1;
		}
	}
	return resp;
}
