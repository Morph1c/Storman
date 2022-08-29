/* PARTE A di storman
* in essa sono implementate block_alloc
							block_release
							pointer_release
							pointer_assign
implementata da
Toccaceli Andrea
numero matricola: 18935580
*/

#include </home/andrea/storman/storman.h>
#include </home/andrea/storman/fun_struct.h>
#include <stdlib.h>
#include <stdio.h>

// BLOCK ALLOC

int block_alloc(void ** ptr_addr, size_t alignment, size_t size){
	int is_free = 0;
	int found = 0;

	if (is_power_two(alignment) == 0 || alignment % sizeof(void *) != 0){ // i vincoli non sono rispettati
		return 2;
	}
	if(search_pnt(ptr_addr, &zoneMan) != NULL){ // ossia il puntatore a cui punta ptr_addr è gia gestito da storman
		not_release_ptr = 1;
		block_release(ptr_addr); // ma senza rilasciare il blocco
		return 1;
	}

	else{
		// cerca una zona disponibile che abbia freesize > size

		// inizializzo puntatore di spostamento per cercare una zona libera
		ptrZone * actual_zone_ptr = &zoneMan;
		void * ptr = actual_zone_ptr -> zone_start;

		// cerco tra le zone disponibili quelle che verificano la richiesta di:
		
		while (actual_zone_ptr -> zone_start != NULL){ 
			printf("sto avanzando nella ricerca delle zone\n");
			void * ptr = actual_zone_ptr -> zone_start;
			
			if(actual_zone_ptr -> free_space_zone > size){ // la zona ha spazio libero sufficente
				while(((long int) ptr + (long int) size) < ((long int) actual_zone_ptr -> zone_start + (long int) actual_zone_ptr -> zone_size)){ // controllo che con questo
																																	// indirizzo di partenza il blocco non esca fuori
																																	// dalla zona
					if((long int) ptr % alignment == 0){ // l'allineamento va bene
						is_free = is_free_space(ptr, size, actual_zone_ptr); // controlla se c'è spazio libero sufficente a partire da ptr ossia controlla tra i buchi
					}

					if(is_free == 1){ // ho trovato l'indirizzo che va bene
						break;
					}
					ptr++; // scorro il puntatore per tutta la zona affinchè trovi un indirizzo di partenza valido
				}
			}

			if(is_free == 0){ // non ho trovato ancora una zona
				actual_zone_ptr = actual_zone_ptr -> next;
			}

			else{ // l'ho trovata ed esco
				break;
			}		
		}		
		
		// ho trovato una zona adatta
		if (actual_zone_ptr -> free_space_zone > size && is_free == 1){
			printf("Zona disponibile\n");

			// la memorizzo in tale zona
			// partendo da ptr

			// trovo la coda della lista dei blocchi della zona
			ptrBlock * actual_block_ptr = &actual_zone_ptr -> BlockZone;
			while (actual_block_ptr -> next -> block_start != NULL){
				actual_block_ptr = actual_block_ptr -> next;
			}

			// ho trovato la coda della lista di blocchi e aggiungo il primo elemento della lista alla coda con indirizzo ptr
			actual_block_ptr -> next = (ptrBlock *) malloc(sizeof(ptrBlock));
			actual_block_ptr -> next -> block_start = ptr; 																				
			actual_block_ptr -> next -> block_size = size;
			actual_block_ptr -> next -> block_alignment = alignment;

			// acquisisco il puntatore iniziale del blocco come puntatore che punta al blocco
			ptrAddrs * new_block_list = &actual_block_ptr -> next -> ptr_block_list;
			new_block_list -> pointer_block = ptr;
			new_block_list -> pointer_variable = ptr_addr;

			actual_block_ptr -> next -> ptr_block_count = 1;

			actual_zone_ptr -> free_space_zone -= size;
			actual_zone_ptr -> number_block_zone++;

			*ptr_addr = ptr; 
		}										 
		// non ho trovato una zona che abbia spazio disponibile e allineamento multiplo
		else{
			printf("Zona non disponibile , sto creando una nuova zona...\n");
			// vado alla coda della lista delle zone
			while(actual_zone_ptr -> next != NULL){
				actual_zone_ptr = actual_zone_ptr -> next;
			}
			// ne creo una con capacità doppia della grandezza del blocco da allocare 
			int size_zone_to_create = 2* size;
			
			void * new_zone = malloc(size_zone_to_create); // 			

			printf("zona inizializzata\n");

			// cerco un indirizzo di partenza che sia un multiplo di aligment 
			ptr = new_zone;
			while((long int) ptr - (long int) new_zone <= (long int) size_zone_to_create){ // trovo un indirizzo di partenza multiplo di alligment e che vada bene
				if((long int) ptr % alignment == 0 && (long int) ptr + (long int) size < (long int) new_zone + (long int) size_zone_to_create){// il puntatore va bene ed esco
					found = 1;
					break;
				}

				ptr++;
			}
			// aggiungo tale zona alla lista

			// creo la nuova zona e aggiungo il blocco facendolo partire da ptr
			if(found == 1){
				actual_zone_ptr -> next = (ptrZone *) malloc(sizeof(ptrZone));
				actual_zone_ptr -> next -> zone_start = new_zone;
				actual_zone_ptr -> next -> zone_size = size_zone_to_create; // nuova zona ha grandezza doppia rispetto alla size del blocco da allocare

				printf("zona creata\n");
				printf("indirizzo zona: %ld\n",(long int) actual_zone_ptr -> next -> zone_start);
				printf("indirizzo iniziale blocco: %ld\n", (long int) ptr);

				// aggiungo il blocco alla nuova zona
				actual_zone_ptr -> next -> BlockZone.block_start = ptr; 
				actual_zone_ptr -> next -> BlockZone.block_size = size;
				actual_zone_ptr -> next -> BlockZone.block_alignment = alignment;

				// memorizzo il puntatore del nuovo blocco come primo punatatore che punta al blocco
				actual_zone_ptr -> next -> BlockZone.ptr_block_list.pointer_block = actual_zone_ptr -> next -> BlockZone.block_start;
				actual_zone_ptr -> next -> BlockZone.ptr_block_list.pointer_variable = ptr_addr;

				// aumento di 1 l'indice che conto nel memorizzare nuovi blocchi e nel memorizzare nuovi puntatori nel blocco
				actual_zone_ptr -> next -> BlockZone.ptr_block_count++;
				actual_zone_ptr -> next -> number_block_zone++;

				// aggiorno lo spazio disponibile nella zona
				actual_zone_ptr -> next -> free_space_zone = actual_zone_ptr -> next -> zone_size - size;

				printf("Nuovo spazio disponibile: %d\n", actual_zone_ptr -> next -> free_space_zone);

				//inizializza il valore di *ptr_addr al primo byte del blocco aggiunto e la funzione esce ritornando 0
				*ptr_addr = actual_zone_ptr -> next -> BlockZone.block_start;
			}

			else{ 
				free(new_zone);
				return 1;
			}
		}
	}

	return 0;
}


// BLOCK RELEASE
int block_release(void ** ptr_addr){
	ptrBlock * actual_block_ptr = search_pnt(ptr_addr, &zoneMan); // trova il blocco dove è contenuto ptr_addr

	if(actual_block_ptr == NULL){
		return 2;
	}

	else{ 
		if (actual_block_ptr -> ptr_block_count == 1){ // ptr_addr è l'unico puntatore che punta al blocco
			// rilascio il puntatore e blocco
			// attaccando nella lista dinamica il blocco precedente del blocco da rilasciare al successivo di tale blocco

			// cerco il blocco precedente ad actual_block_ptr, se esiste
			ptrZone * actual_zone_ptr = &zoneMan;
			ptrBlock * prev_block_ptr =  &actual_zone_ptr -> BlockZone;
			while(actual_zone_ptr -> zone_start != NULL){
				// cerco finchè non è il precedente o finchè i blocchi non finiscono, la seconda condizione mi dice che ho visto tutti i blocchi della zona
				if(prev_block_ptr == actual_block_ptr){ // il blocco da eliminare è il primo della zona
					break;
				}
				else{
					while(prev_block_ptr -> next != actual_block_ptr && prev_block_ptr -> block_start != NULL){ // ossia finchè il blocco non è il precedente o non lo trova
		
						prev_block_ptr = prev_block_ptr -> next;
					}
				}

				if(prev_block_ptr -> next == actual_block_ptr){ // ho trovato il precedente
						break;
				}

				else{ // non l'ho trovato e vado avanti nelle zone a cercare
					actual_zone_ptr = actual_zone_ptr -> next;
					prev_block_ptr =  &actual_zone_ptr -> BlockZone;
				}
			}

			// attacca il blocco precedente del blocco da rilasciare al successivo di tale blocco 
			if(prev_block_ptr -> next == actual_block_ptr){ // ossia il blocco da eliminare non è il primo della lista
				prev_block_ptr -> next = actual_block_ptr -> next;
			}

			else{ // il blocco da eliminare è il primo nella zona e quindi lo levo dalla lista blockZone
				actual_zone_ptr -> BlockZone = *actual_block_ptr -> next;
			}

			// altrimenti se il precedente non esiste e neanche il successivo vuol dire che il blocco è l'unico della zona
			// libero il blocco e aggiorno la memoria
			actual_zone_ptr -> free_space_zone += actual_block_ptr -> block_size;
			actual_zone_ptr -> number_block_zone -= 1; 

			actual_block_ptr -> block_start = NULL;
			actual_block_ptr -> block_size = 0;
			free(&actual_block_ptr -> ptr_block_list);	
			actual_block_ptr -> ptr_block_count = 0;

			if(not_release_ptr == 0){// ossia block release non è stato chiamato da block alloc o pointer assign
				*ptr_addr = NULL;
			}

			free(actual_block_ptr); // libero la memoria della struttura per il blocco e non il blocco stesso
			return 0;
		}
	

		else{ // il puntatore non è l'unico che punta al blocco
			// rilascio solo il puntatore
			// e attacca il precedente di tale puntatore al successivo di tale puntatore
			// cerca il puntatore precedente a ptr_addr in actual_block_ptr
			ptrAddrs * prev_ptr = &actual_block_ptr -> ptr_block_list;
			ptrAddrs * actual_ptr;
			if(prev_ptr -> pointer_variable != ptr_addr){ // ossia il puntatore da levare non è il primo della lista
				while (prev_ptr -> next -> pointer_variable != ptr_addr){ 
					prev_ptr = prev_ptr -> next;
				}			
				actual_ptr = prev_ptr -> next; // che sarebbe appunto il puntatore da liberare

				// attacco il precedente di tale puntatore al successivo di tale puntatore
				prev_ptr -> next = actual_ptr -> next;
			}

			else{ // vuol dire che ptr_addr è l'indirizzo del primo puntatore che punta al blocco e quindi non ha precedente
				actual_block_ptr -> ptr_block_list = *prev_ptr -> next; // faccio partire la lista di puntatori dal successivo del puntatore da rilasciare
				actual_ptr = prev_ptr;
			}

			actual_ptr -> pointer_variable = NULL;
			actual_ptr -> pointer_block = NULL;

			// aggiorno il numero di puntatori nel blocco
			actual_block_ptr -> ptr_block_count -= 1;

			if(not_release_ptr == 0){// ossia block release non è stato chiamato da block alloc o pointer assign e quindi rilascio il puntatore puntato da ptr_addr
				*ptr_addr = NULL;
			}

			free(actual_ptr);
			return 1;
		}
			
	}

	return 2;
}


// POINTER RELEASE
int pointer_release(void ** ptr_addr){
	ptrBlock * actual_block_ptr = search_pnt(ptr_addr, &zoneMan); // ossia cerco il blocco dove sta il puntatore
	
	if(actual_block_ptr == NULL){
		return 1;
	}

	else{ 

		if (actual_block_ptr -> ptr_block_count == 1){ // il blocco non è puntato da altri puntatori
			return 2;
		}

		else{
			// rilascio solo il puntatore
			// e attacca il precedente di tale puntatore al successivo di tale puntatore
			ptrAddrs * actual_ptr = &actual_block_ptr -> ptr_block_list;
			ptrAddrs * prev_ptr = actual_ptr;
			if(prev_ptr -> pointer_variable != ptr_addr){
				while(prev_ptr -> next -> pointer_variable != ptr_addr){ // la seconda condizione può essere vera solo alla prima iterazione
					prev_ptr = prev_ptr -> next;
				}

				actual_ptr = prev_ptr -> next; // ossia il puntatore che ha come indirizzo ptr_addr
				prev_ptr -> next = actual_ptr -> next;
			}

			else{ // il puntatore è il primo di ptr_block_list ossia actual_ptr == prev_ptr e quindi non ha precedente
				actual_block_ptr -> ptr_block_list = *prev_ptr -> next;
			}

			actual_ptr -> pointer_variable = NULL;
			actual_ptr -> pointer_block = NULL;
			*ptr_addr = NULL;
			free(actual_ptr);
			return 0;
		}
		
	}

}


// POINTER ASSIGN
int pointer_assign(void ** ptr_addr , void * val){
	ptrBlock * possible_block = valid_address(val, &zoneMan);
	if(possible_block == NULL){
		return 1;
	}
	ptrBlock * ptr_block = search_pnt(ptr_addr, &zoneMan);
	if(ptr_block != NULL){ // ptr_addr è già gestito da storman
		if(val == *ptr_addr){
			return 0;
		}

		else{
			// controllo se val non è un indirizzo che può stare nel blocco dove si trova ptr_addr
			// ipotizzando che *ptr_addr coincida con pointer_block associato a ptr_addr che sta dentro il blocco trovato
			if(val > ptr_block -> block_start + ptr_block -> block_size || val < ptr_block -> block_start){
				not_release_ptr = 1; // ossia non rilascio l'indirizzo in ptr_addr
				block_release(ptr_addr);
				// ma senza rilasciare il blocco
			}

			else{
				return 1;
			}
		}

	}

	else{
		ptrAddrs * actual_ptr = &possible_block -> ptr_block_list;
		while(actual_ptr -> next != NULL){
			actual_ptr = actual_ptr -> next;
		}

		// acquisisco ptr_addr e lo attacco alla coda di ptr_block_list
		actual_ptr -> next = (ptrAddrs *) malloc(sizeof(ptrAddrs));
		actual_ptr -> next -> pointer_variable = ptr_addr;
		actual_ptr -> next -> pointer_block = val;

		*ptr_addr = val;

		return 0;
	}

}