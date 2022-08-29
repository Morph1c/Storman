// definisco le strutture date da usare in block_alloc
//

#include <stdlib.h>
#include <stdio.h>

// TIPI USATI DA STORMAN

// ptrAddrs lista dinamica con tutti i puntatori che puntano a quel blocco
typedef struct ptrAddrs{
	void *pointer_block; // indirizzo del byte di memoria del blocco a cui punta tale puntatore
	void **pointer_variable; // indirizzo del puntatore
	struct ptrAddrs* next;

}ptrAddrs;

// ptrBlock è una lista dinamica che rappresenta un blocco con la sua grandezza e tutti i puntatori che puntano a tale blocco
typedef struct ptrBlock{
	void* block_start;
	int block_size;
	int block_alignment;
	ptrAddrs ptr_block_list; // array che contiene tutti i puntatori che puntano a quel blocco che sono tanti quanti sono i byte di grandezza del blocco
	int ptr_block_count; // contatore che conta il numero di puntatori che puntano al blocco
	struct ptrBlock* next;

}ptrBlock;

// ptrZone gestice tutte le zone in cui si possono memorizzare blocchi
typedef struct ptrZone{
		void* zone_start; // puntatore dell'inizio della zona
		int zone_size; // grandezza della zona, ossia il numero di tutti i byte di tutti i blocchi che la zona può contenere
		int free_space_zone; // spazio libero della zona, che è inizializzato con la stessa grandezza della zona
		ptrBlock BlockZone; // lista dinamica di blocchi che sono contenuti nella zona e al più sono i singoli byte di zoneSize
		int number_block_zone; // l'indice che mi dice quanti blocchi sono stati memorizzati in tale zona
		struct ptrZone* next; // puntatore alla zona successiva
}ptrZone;


// dichiaro alcune variabili globali
extern ptrZone zoneMan;
ptrZone zoneMan;
extern int not_release_ptr;
int not_release_ptr = 1;


// dichiaro funzioni da usare in A.h

int is_power_two(int num){
	int is_pow = 1; // 0 sta per false e 1 per true
	int res = num;
	while (res > 1){
		if (res % 2 != 0){
			is_pow = 0;
			return is_pow;
		}

		res = res / 2;
	}
	return is_pow;
}



// search_pnt ricerca l'indirizzo di un puntatore tra tutti i blocchi gestiti da storman e 
// se trova tale blocco ritorna un puntatore ad esso altrimenti ritorna NULL

ptrBlock * search_pnt(void ** pointer_address, ptrZone * pointer_zone){
	ptrZone * actual_zone_ptr = pointer_zone;
	ptrBlock * actual_block_ptr = &actual_zone_ptr -> BlockZone;
	
	while(actual_zone_ptr -> zone_start != NULL){
		while(actual_block_ptr -> block_start != NULL){
			ptrAddrs * actual_ptr = &actual_block_ptr -> ptr_block_list;
			while(actual_ptr -> pointer_variable != NULL){
				if (actual_ptr -> pointer_variable == pointer_address){
					//is_managed = 1;
					return actual_block_ptr;
				}

				actual_ptr = actual_ptr -> next;

			}

			actual_block_ptr = actual_block_ptr -> next;
		}
		actual_zone_ptr = actual_zone_ptr -> next;
	}
	return NULL;
}

// valid_address controlla che dato un puntatore esso sia contenuto nel range di memoria di un blocco
// se trova un blocco per cui val è un indirizzo contenuto nel suo range di memoria, ritorna un puntatore ad esso

ptrBlock * valid_address(void * val, ptrZone * pointer_zone){
	ptrZone * actual_zone_ptr = pointer_zone;
	ptrBlock * actual_block_ptr = &actual_zone_ptr -> BlockZone;


	while(actual_zone_ptr -> zone_start != NULL){
		while(actual_block_ptr -> block_start != NULL){
			// controllo che val è nel range di ampiezzza del blocco
			// se lo è vuol dire che val è un possibile indirizzo del blocco e ritorno il blocco
			if((long int) val > (long int) actual_block_ptr -> block_start &&  (long int) val < ( ((long int) actual_block_ptr -> block_start) + ((long int ) actual_block_ptr -> block_size)));{
				return actual_block_ptr;
			}
			actual_block_ptr = actual_block_ptr -> next;
		}

		actual_zone_ptr = actual_zone_ptr -> next;
	}

	return NULL;
			
}

// controlla se dato un indirizzo di partenza e una size, tutti i gli indirizzi da ptr fino ptr + size non siano gestiti da storman

int is_free_space(void * ptr, int size, ptrZone * pointer_zone){
	int free_address = 0;
	void * actual_ptr = ptr;
	while(free_address < size){
		if(search_pnt(ptr, pointer_zone) != NULL){
			return 0;
		}

		ptr++;
		free_address++;
	}

	return 1;
}