#include "queue.h"

/* Função queue_new - cria uma struct queue, inicializa os parâmetros necessários
*					  e returna uma apontador/referência de memória para essa estrutura.
*
* Variáveis:
*			- struct queue *created, apontador/referência de memória que vai ser returnado.
*/
struct queue *new_queue()
{
	struct queue *created = malloc(sizeof(struct queue));

	created->size = 0;
	created->head = 0;
	created->tail = 0;

	for(int i = 0; i < QUEUESIZE; i++)
	{
		bzero(created->queued[i], COLUMNSIZE);
		strcat(created->queued[i], GARBAGE);
	}

	return created;
}

/*Função queue_destroy - recebe um apontador para uma struct queue como argumento e liberta o espaço
*						em memória que esta ocupa.
*
* Argumentos:
*			- struct queue *to_destroy, um apontador/referência de memória para uma struct queue
*			o qual o seu espaço em memória vai ser liberto.
*/
void queue_destroy(queue *to_destroy)
{
	free(to_destroy);
}

/*Função queue_insert - recebe uma struct queue e um valor que insere nessa queue,
*						returna um valor booleano. True se conseguir inserir, false
*						se não conseguir.
*
* Argumentos:
*			- struct queue *queue, apontador/referência de memória de uma queue
*			para onde o valor vai ser inserido;
*			- char *val, valor que vai ser adicionado na queue.
*/
bool queue_insert(queue *queue, char *val)
{
	if(queue->size < QUEUESIZE)
	{
		strcat(queue->queued[queue->tail], val);
		queue->tail = (queue->tail + 1) % QUEUESIZE;
		queue->size = queue->size + 1;

		return true;
	}

	return false;
}

/*Função queue_remove - recebe uma struct queue e remove o valor que está à
*						cabeça da queue, returnando um booleano. Se conseguir
*						retirar alguma coisa então devolve true, caso contrário
*						devolve false.
*
* Argumentos:
*			- struct queue *queue, apontador/referência de memória de uma queue
*			de onde vai ser removido um valor.
*/
bool queue_remove(queue *queue)
{
	if(queue->size > 0)
	{
		bzero(queue->queued[queue->head], COLUMNSIZE);
		strcat(queue->queued[queue->head], GARBAGE);
		queue->head = (queue->head + 1) % QUEUESIZE;
		queue->size = queue->size - 1;

		return true;
	}

	return false;
}

/*Função queue_get_tail - recebe uma struct queue como argumento e retorna
*						  o último elemento a ser inserido na queue, caso não tiver
*						  devolve uma string vazia.
*
* Argumentos:
*			- struct queue *queue, apontador/referência de memória de uma queue
*			de onde se vai buscar o valor.
*
* Variáveis:
*			- short pos, posição do array de onde se vai buscar o valor que vai
*			ser returnado.
*/
char *queue_get_tail(queue *queue)
{
	if(queue->size != 0)
	{
		short pos;

		if(queue->tail == 0)
		{
			pos = 9;
		}else
		{
			pos = queue->tail - 1;
		}

		return queue->queued[pos];
	}

	return GARBAGE;
}

/*Função queue_is_full - recebe uma struct queue *queue como argumento e verifica
*						 se está cheia, se estiver devolve true, caso contrário
*						 devolve false.
*
* Argumentos:
*			- struct queue *queue, apontador/referência para uma struct queue.
*/
bool queue_is_full(queue *queue)
{
	return (queue->size == QUEUESIZE);
}

void queue_to_array(struct queue *queue, char *to_return)
{
	short current_position = queue->head;
	short position_array = 0;

	bzero(to_return, sizeof(&to_return));

	if(queue->size > 0)
	{
		do
		{
			strcat(to_return, queue->queued[current_position]);
			strcat(to_return, DELIMITER);

			current_position = (current_position + 1) % QUEUESIZE;
		}while(current_position != queue->tail);
	}

	if(position_array < QUEUESIZE)
		strcat(to_return, END);
}
