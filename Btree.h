#ifndef BTREE_H
#define BTREE_H
#include "B_node.h"
#include <iostream>


using namespace std;

template <class Type, int order> class Btree {

protected: // dato members
	B_node<Type, order> *root;
private:
	void recInorder(B_node<Type, order> * current );
    bool  Encontrar( B_node<Type, order> *current, FileTable_d *&target);

public: // publics.
	Btree();
	// travesling
	void inOrder();
    bool buscar( FileTable_d *&searchitem );
	void mostrarArbol();
	// insertion
    bool buscarEnNodo( B_node<Type, order> *current, FileTable_d* &target, int &position );
    void insertar(FileTable_d* &new_entry);
    void insertarEnRecursion( B_node<Type, order> *current, FileTable_d* &new_entry,FileTable_d* &median,B_node<Type, order> * &rightchilds , bool &result );
    void insertarEnNodo(B_node<Type, order> *current, FileTable_d* &entry, B_node<Type, order> *rightchilds, int position);
    void dividirNodo( B_node<Type, order> *current,  FileTable_d* &extra_entry,   B_node<Type, order> *extra_childs,  int position,
        B_node<Type, order> * &right_half,   FileTable_d* &median);
	// eliminacion

    void remover( FileTable_d *&target );
    void removerEnRecursion( B_node<Type, order> *current, FileTable_d* &target );
	void removerDato(  B_node<Type, order> *current, int position  );
	void copiarDePredecesor(  B_node<Type, order> *current, int position );
	// funciones para restaurar despues de la eliminacion
	void restaurar( B_node<Type, order> *current ,int position  );
	void moverAlaIzquierda( B_node<Type, order> *current , int position );
	void moverAlaDerecha( B_node<Type, order> *current , int position );
	void combinar( B_node<Type, order> *current, int position );
    int hashCode(string text);
	
	void imprimirArbol( B_node<Type, order> *current );
};

template <class Type, int order> Btree<Type,order>::Btree(){
	root = NULL;
}

template <class Type, int order> int  Btree<Type,order>::hashCode(string text){
    int hash = 0, strlen = text.length(), i;
    char character;
    if (strlen == 0)
        return hash;
    for (i = 0; i < strlen; i++) {
        character = text[i];
        hash = (31 * hash) + int(character);
    }
    return hash;
}

template <class Type, int order> void Btree<Type,order>::inOrder(){  
	return recInorder( root );   
}

template <class Type, int order> void Btree<Type,order>::recInorder(B_node<Type, order> * current ){
	if ( current != NULL )
	{
		recInorder( current->childs[0]);
		for (int i = 0; i < current->count; i++)
		{
			cout << current->data[i] << " ";
			recInorder(current->childs[i + 1]);
		}
	}
}
template <class Type, int order> bool Btree<Type,order>::buscar(FileTable_d *&searchitem ){
	
	return Encontrar( root , searchitem );
}
template <class Type, int order> bool Btree<Type,order>::Encontrar(B_node<Type, order> *current, FileTable_d *&target ){
	
	bool result = false;
	int position;
	if (current != NULL) {
		result = buscarEnNodo(current, target, position);
		if (result == false)
			result = Encontrar(current->childs[position], target);
		else
            target = current->data[position];
	}
	return result;
}


template <class Type, int order> bool Btree<Type,order>::buscarEnNodo(B_node<Type, order> *current, FileTable_d *&target, int &position ){
	position=0;
    while (position < current->count && target->index > current->data[position]->index)
		position++; 
    if (position < current->count && target->index == current->data[position]->index)
		return true;
	else
		return false;
}
template <class Type, int order> void Btree<Type,order>::insertar(FileTable_d *&new_entry){
    FileTable_d* median;
	B_node<Type, order> *rightchilds, *new_root;
	bool  result ; 
	insertarEnRecursion(root, new_entry, median, rightchilds , result);
	if(result) { 
		new_root = new B_node<Type, order>;
		new_root->count = 1;
		new_root->data[0] = median;
		new_root->childs[0] = root;
		new_root->childs[1] = rightchilds;
		root = new_root;
	}
}
template <class Type, int order> void Btree<Type,order>::insertarEnRecursion(B_node<Type, order> *current,
    FileTable_d *&new_entry, FileTable_d *&median, B_node<Type, order> * &rightchilds , bool &result ){

		int position;
		if (current == NULL) {

			median = new_entry;
			rightchilds = NULL;
			result = true;
		}
		else { 
			bool fl = buscarEnNodo(current, new_entry, position);
			if ( fl ){
				cout<<"Item duplicado\n"; 
			    result = false; 
				return; 
			}
			else {
                FileTable_d *extra_entry;
				B_node<Type, order> *extra_childs;

				insertarEnRecursion(current->childs[position], new_entry,extra_entry, extra_childs , result);
				if ( result ) {
					if ( !current->NodeFull() ) {
						result = false;
						insertarEnNodo(current, extra_entry, extra_childs, position);
					}
					else 
						dividirNodo( current, extra_entry, extra_childs, position,rightchilds, median);
				}

			}
		}
}
template <class Type, int order> void Btree<Type,order>::insertarEnNodo(B_node<Type, order> *current,
    FileTable_d *&entry, B_node<Type, order> *rightchilds, int position){
		int i; 
		for(i=current->count-1;i>= position;i--){ 
			current->data[i+1]=current->data[i];
			current->childs[i+2]=current->childs[i+1]; 
		} 
		current->data[position]= entry; 
		current->childs[i+2]=rightchilds ;
		current->count++;
}
template <class Type, int order> void Btree<Type,order>::dividirNodo(B_node<Type, order> *current, FileTable_d *&extra_entry,   B_node<Type, order> *extra_childs,  int position,
    B_node<Type, order> * &right_half,   FileTable_d *&median){

		right_half = new B_node<Type,order>;
		int mid = order/2;
		if( position <= mid ){
			for(int i = mid;i < order-1;i++ ){
				right_half->data[i-mid] = current->data[i];
				right_half->childs[i+1-mid] = current->childs[i+1];
			}
			current->count = mid;
			right_half->count = order-1-mid;
			insertarEnNodo( current , extra_entry , extra_childs , position );
		} else {
			mid++;
			for(int i = mid;i < order-1;i++ ){
				right_half->data[i-mid] = current->data[i];
				right_half->childs[i+1-mid] = current->childs[i+1];
			}
			current->count = mid;
			right_half->count = order-1-mid;
			insertarEnNodo( right_half , extra_entry , extra_childs , position - mid);
		}
		median = current->data[current->count - 1];
		right_half->childs[0] = current->childs[current->count];
		current->count--;
}

template <class Type, int order> void Btree<Type,order>::remover(FileTable_d *&target ){

	removerEnRecursion( root , target );
	if( root != NULL && root->count == 0 ){
		B_node<Type, order> *delete_root = root;
		root = root->childs[0];
		delete delete_root;
	}
}
template <class Type, int order> void Btree<Type,order>::removerEnRecursion(B_node<Type, order> *current, FileTable_d *&target ){

	int position;
	if( current == NULL ){
		cout << "item no encontrado.\n";
		return;
	}
	else{
		bool fl = buscarEnNodo( current , target ,position );
		if( fl ){

			if( current->childs[position] != NULL ){
				copiarDePredecesor( current , position );
				removerEnRecursion( current->childs[position], current->data[position]);
			}else
				removerDato( current , position );

		}else
			removerEnRecursion( current->childs[position], target );

		if( current->childs[position] != NULL )
			if( current->childs[position]->NodeEmpty() )
				restaurar( current , position );
	}
}

template <class Type, int order> void Btree<Type,order>::removerDato(  B_node<Type, order> *current, int position  ){

	for(int i = position; i < current->count-1 ;i++)
		current->data[i] = current->data[i+1];
	current->count--;
}
template <class Type, int order> void Btree<Type,order>::copiarDePredecesor(  B_node<Type, order> *current, int position ){

	B_node<Type, order> * leaf = current->childs[position];

	while( leaf->childs[leaf->count] != NULL )
		leaf = leaf->childs[leaf->count]; //mover hacia al hijo mas derecho
	current->data[position] = leaf->data[leaf->count-1];
}

template <class Type, int order> void Btree<Type,order>::restaurar( B_node<Type, order> *current ,int position  ){

	if( position == current->count )
		if( current->childs[position-1]->count > (order-1)/2 )
			moverAlaDerecha( current , position-1);
		else
			combinar( current , position );
	else if( position == 0 )
		if( current->childs[1]->count > (order-1)/2)
			moverAlaIzquierda( current , 1 );
		else
			combinar( current , 1 );
	else
		if( current->childs[position-1]->count > (order-1)/2 )
			moverAlaDerecha( current , position-1 );
		else if( current->childs[position+1]->count > (order-1)/2 )
			moverAlaIzquierda( current , position+1 );
		else
			combinar( current, position );
}

template <class Type, int order> void Btree<Type,order>::moverAlaIzquierda( B_node<Type, order> *current , int position ){

	B_node<Type, order> *leftchilds = current->childs[position-1],
		*rightchilds = current->childs[position];
	leftchilds->data[leftchilds->count] = current->data[position-1];

	leftchilds->childs[++leftchilds->count] = rightchilds->childs[0];
	current->data[position-1] = rightchilds->data[0];

	rightchilds->count--;
	for(  int i =0;i < rightchilds->count;i++ ){
		rightchilds->data[i] = rightchilds->data[i+1];
		rightchilds->childs[i] = rightchilds->childs[i+1];
	}
	rightchilds->childs[rightchilds->count] = rightchilds->childs[rightchilds->count +1];

}

template <class Type, int order> void Btree<Type,order>::moverAlaDerecha( B_node<Type, order> *current , int position ){

	B_node<Type, order>*rightchilds = current->childs[position+1],
		*leftchilds = current->childs[position];

	rightchilds->childs[rightchilds->count+1] = rightchilds->childs[rightchilds->count];

	for( int i=rightchilds->count; i > 0;i-- ){
		rightchilds->data[i] = rightchilds->data[i-1];
		rightchilds->childs[i] = rightchilds->childs[i-1];
	}

	rightchilds->count++;
	rightchilds->data[0] = current->data[position];

	rightchilds->childs[0] = leftchilds->childs[leftchilds->count--];
	current->data[position] = leftchilds->data[leftchilds->count];
}

template <class Type, int order> void Btree<Type,order>::combinar( B_node<Type, order> *current, int position ){

	int i;
	B_node<Type, order> *leftchilds = current->childs[position-1],
		*rightchilds = current->childs[position];

	leftchilds->data[leftchilds->count] = current->data[position-1];
	leftchilds->childs[++leftchilds->count] = rightchilds->childs[0];

	for( i =0;i < rightchilds->count;i++){
		leftchilds->data[leftchilds->count] = rightchilds->data[i];
		leftchilds->childs[++leftchilds->count] = rightchilds->childs[i+1];
	}
	current->count--;

	for( i=position-1; i < current->count; i++ ){
		current->data[i] = current->data[i+1];
		current->childs[i+1] = current->childs[i+2];
	}

	delete rightchilds;
}

template <class Type, int order> void Btree<Type,order>::imprimirArbol( B_node<Type, order> *current ){
	
	int i;
	if(!current) 
		return; 
	static int k = 1;
	cout << k <<" ";
	current->mostrarNodo(); 
	k++;
	for( i=0; i<= current->count ;i++) 
		imprimirArbol( current->childs[i] );
}
template <class Type, int order> void Btree<Type,order>::mostrarArbol(){
	
	if ( root == NULL ) 
		cout<<"BTree is empty..."; 
	else{ 
		cout <<"PAG\tKeys Nodo\n";
		imprimirArbol( root );
	} 
}


#endif
