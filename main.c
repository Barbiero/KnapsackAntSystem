#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>

#define NUM_OBJS 300
#define MAX_PESO_OBJ 100
#define MAX_PROFIT 10
#define CAP_MOCHILA 3000

//feromonio
#define PHE_INICIAL 1
#define PHE_MAX 100

//variaveis que determinam o quanto o feromonio e a atratividade pesam
#define PHE_WEIGHT 1
#define ATTRACT_WEIGHT 3

//definir tipos para flexibilidade
typedef uint16_t Peso;
typedef uint16_t Lucro;
typedef uint8_t Feromonio;

typedef struct {
	Peso w;
	Lucro z;
	Feromonio thao;
} Objeto;

Objeto objetos[NUM_OBJS];

void gerar_objetos()
{
	srand(0xDEADBEEF);

	for(int i = 0; i < NUM_OBJS; i++)
	{
		Peso p = rand() % MAX_PESO_OBJ + 1;
		Lucro l = rand() % MAX_PROFIT + 1;
		
		objetos[i] = (Objeto) { .w = p, .z = l,
					.thao = PHE_INICIAL };
	}
}

typedef struct mochila_struct {
	Peso capacidade;
	Lucro lucro_total;
	bool has_obj[NUM_OBJS];
} Mochila;

void print_objs_na_mochila(Mochila *m)
{
	int n = 0;
	for(int i = 0; i < NUM_OBJS; i++)
	{
		
		if(m->has_obj[i]){	
			printf("(%2d,%2d),%c", objetos[i].w, objetos[i].z,
				(n%5)==4?'\n':' ');
			n++;
		}
	}
}

void mochila_init(Mochila *m)
{
	m->capacidade = CAP_MOCHILA;
	m->lucro_total = 0;
	memset(m->has_obj, false, sizeof(m->has_obj));
}

typedef enum ret_status_enum {
	RET_NULL,
	FALHA_ITEMADICIONADO,
	FALHA_CAPINSUF,
	SUCESSO
} RetStatus;

RetStatus mochila_add_item(Mochila *m, int obj_id)
{
	assert(obj_id < NUM_OBJS);

	if(m->has_obj[obj_id])
	{
		return FALHA_ITEMADICIONADO;
	}

	if(objetos[obj_id].w > m->capacidade)
	{
		return FALHA_CAPINSUF;
	}

	m->has_obj[obj_id] = true;
	m->capacidade -= objetos[obj_id].w;
	m->lucro_total += objetos[obj_id].z;
}

typedef struct obj_vizi_struct {
	int id;
	double coef; //coeficiente de probabilidade do obj
	long int p_ini;
	long int p_fim;
} obj_vizi;


typedef struct vizinhanca_struct {
	obj_vizi *objs_disponiveis;
	int num_objs; //objs_disponiveis tem tamanho dinamico

	double sum_coefs; //soma dos coeficientes de prob de cada obj
	long int range_prob; //alcance das probabilidades em uma resolucao de 100k
} Vizinhanca;

void vizinhanca_init(Vizinhanca* v, Mochila* m)
{
	int numobjs = 0;
	for(int i = 0; i < NUM_OBJS; i++)
	{
		if(m->has_obj[i]){
			continue;
		}
		else if(objetos[i].w > m->capacidade){
			continue;
		}
		else{
			numobjs++;
		}
	}

	v->sum_coefs = 0.0;
	v->num_objs = numobjs;
	v->objs_disponiveis = malloc(sizeof(obj_vizi) * numobjs);
	v->range_prob = 0;

	int j = 0;
	for(int i = 0; i < NUM_OBJS; i++)
	{
		if(m->has_obj[i] || objetos[i].w > m->capacidade) {}
		else{
			v->objs_disponiveis[j].id = i;
			double thao = pow((double)objetos[i].thao, PHE_WEIGHT);
			double attr = pow(
				((double) objetos[i].z /
				 (double)objetos[i].w*objetos[i].w), ATTRACT_WEIGHT);

			v->objs_disponiveis[j].coef = thao*attr;
			v->sum_coefs += v->objs_disponiveis[j].coef;
			j++;
		}
	}

	long int last_prob = 0;
	for(j = 0; j < numobjs; j++)
	{
		double prob_choice = v->objs_disponiveis[j].coef / v->sum_coefs;
		long int range = fmax(1,(lround(prob_choice * 100000.0)));
		
		v->objs_disponiveis[j].p_ini = last_prob;
		v->objs_disponiveis[j].p_fim = last_prob+range;
		last_prob += range;
	}

	v->range_prob = last_prob;
}

/**
 * Pega o ID do objeto cuja probabilidade encobre o numero prob
 * prob deve ser um valor entre [0, v.range_prob]
*/
int vizinhanca_search_prob(Vizinhanca *v, long int prob)
{
	assert(v != NULL);
	assert(prob <= v->range_prob);

	//busca binaria!
	int first = 0;
	int last = v->num_objs;
	int mid = (first+last)/2;
	while(first <= last){
		if(v->objs_disponiveis[mid].p_ini < prob){
			if(v->objs_disponiveis[mid].p_fim >= prob){
				return v->objs_disponiveis[mid].id;
			}
			else{ //p_ini < prob && p_fim < prob => direita
				first = mid + 1;
			}
		}
		else if(v->objs_disponiveis[mid].p_ini > prob){
			last = mid - 1;
		}
		else{
			return v->objs_disponiveis[mid].id;
		}

		mid = (first+last)/2;
	}
	return -1;
}


void vizinhanca_destroy(Vizinhanca* v)
{
	assert(v != NULL);
	free(v->objs_disponiveis);
}
	


int main(){
	gerar_objetos();
	
	printf("Objetos:\n");
	for(int i = 0; i < NUM_OBJS; i++)
	{
		printf("(%2d,%2d),%c", objetos[i].w, objetos[i].z,
				(i%5)==4?'\n':' ');
	}


	Mochila m;
	mochila_init(&m);

	Vizinhanca v;
	vizinhanca_init(&v, &m);

	for(int i = 0; i < v.num_objs; i++)
	{
		printf("%d:(%ld, %ld):%.2f\n",
					v.objs_disponiveis[i].id,
					v.objs_disponiveis[i].p_ini,
					v.objs_disponiveis[i].p_fim,
					100000*(v.objs_disponiveis[i].coef/v.sum_coefs));
	}
	printf("Alcance de prob: %ld\n", v.range_prob);

	int id = vizinhanca_search_prob(&v, 99295 );
	printf("Objeto ID %d!\n", id);

	vizinhanca_destroy(&v);

/*
	RetStatus ret = RET_NULL;
	while(ret != FALHA_CAPINSUF)
	{
		int obj_id = rand() % NUM_OBJS;
		ret = mochila_add_item(&m, obj_id);
	}

	printf("Mochila:\n");
	print_objs_na_mochila(&m);
	printf("\n");
	printf("Cap: %d Lucro: %d\n", m.capacidade, m.lucro_total);
*/

	return 0;
}


