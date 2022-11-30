//project SIR EPEDEMICS
//to check the correctness of algorithm work with N,E,T setting them lesser values.. because i found running time is more
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define N 10000
#define E 3000
#define T 300
typedef struct que * linker; //giving synonym to struct que pointer
struct que{
	int event_id;   //is it recovery or transmission
	int person_id;  //to whom its happening
	int t;          //at what time is it hapening
	int rec_t;      //what is recovery time of that person
	linker next;    //whats next?
};
typedef struct que que;
typedef struct person* interlink; //for maitaining list of persons
struct person{
	int person_id;   //identity
	interlink link; 
	int status;    //is he priorly infected-- wht is his status i.e is it 1-s,2-i,3-r
};
interlink s;
linker e;
typedef struct person person;
//function prototypes
void generate_graph(int adj_matrix[N][N]);
int existing_edges(int adj_matrix[N][N],int node);
void changeStatus(int id,int event_id);
void printInfected();
int existevent(int id);
void printRecovered();
void printSuspected();
void add_event(linker *l,int t,int event,int person_id,int rec_t);
void append_list(interlink *l,int k);
void initialise(int *l);
int check_status(int id,interlink *l);
int number_events();
void pop_event(linker *l);
void fast_sir(int *k) ;
void process_trans_sir(int *k);
void process_recover_sir(int *k);
//void print_events(linker *l);  in checking the corectness of the algorithm this function helps
void delete_node(interlink *l,int id);
void processing(int *k);
int adj_matrix[N][N]; //global declaration of matrix
int main()
{
	int k,j;
	for(k=0;k<N;k++)
	{
		for(j=0;j<N;j++)           
		{
			adj_matrix[k][j]=0;            //initialising all entries to 0
		}
	}
	generate_graph(adj_matrix); //generating random undirected graph with max number of edges per vertex
	int t=0;
	initialise(&t); // getting initial conditions
	fast_sir(&t);	
}
void generate_graph(int adj_matrix[N][N])
{
	int count,j;
	int uns_edges;  //unsure edges
	int endnode,edgecounter; //count edges based on the node which recieves edges(though undirected means both sides points)
	for(j=0;j<N-1;j++)
	{
		count=0;
		uns_edges=rand()%E + 1; //setting the number of unsure edges
		edgecounter=existing_edges(adj_matrix,j); //counts the existing number of edges that vertex has at that instant
		if(uns_edges > edgecounter) //are the existing edges less than unsure edges
		{
			count=edgecounter;
			while(count < uns_edges)  //if so increase the number of edges by giving new ones randomly
			{
				endnode=rand()%N;
				if(adj_matrix[j][endnode]==0 && endnode!=j && existing_edges(adj_matrix,endnode)<E)
				{
					adj_matrix[j][endnode]=1;
					adj_matrix[endnode][j]=1;
					count++;
				}
				else
				{
					continue;
				}
			}
		}
		else if(uns_edges < edgecounter) //already if the edges cross limit this will take care 
		{
			count=edgecounter;
			while(count > uns_edges)
			{
				srand(time(NULL));
				endnode=rand()%N;
				if(adj_matrix[j][endnode]==1) //by reducing the number of edges till the treshold comes(unsure edges)
				{
					adj_matrix[j][endnode]=0;
					adj_matrix[endnode][j]=0;
					count--;
				}
			}
		}			
	}
}
/*i called them unsure edges because they can be gone if  the endnode exceeds its unsure edges*/
int existing_edges(int adj_matrix[N][N],int node) //counts the existing edges
{
	int j;
	int count=0;
	for(j=0;j<N-1;j++)
	{
		if(adj_matrix[node][j]==1)
		{
			count++;
		}
	}
	return count;
}
// this algorithm is mainly event based
void add_event(linker *l,int t,int event,int person_id,int rec_t)   //priority wise with time as priority
{
	linker new;
	new=(linker)malloc(sizeof(que));   //give space to create event in event space
	new->t = t; 					   //the rest of this is simple linked lists implementation of priority que
	new->event_id=event;
	new->person_id=person_id;
	new->rec_t=rec_t;
	linker aux;
	aux = *l;
	if(aux!=0)
	{
		if(aux->t > new->t)
	    {
		    new->next=aux;
		    *l=new;
	    }
	    else if(aux->t == new->t)
	    {
			new->next=(*l)->next;
			(*l)->next=new->next;
		}
	    else if(aux->next!=0)
	    {
			linker temp;
		    temp=aux->next;
		    while(temp!=0 && temp->t <= new->t)
		    {
			    aux=aux->next;
			    temp=temp->next;
		    }
		    aux->next=new;
		    new->next=temp;
	    }
	    else
	    {
			aux->next=new;
		}
	}
	else
	{
		*l=new;
	}
}
void append_list(interlink *l,int k) //for initial creation of list of people and keeping track of their status
{
	interlink aux;                  //this is simple linked lists append function
	aux=*l;
	interlink temp;
	temp=(interlink)malloc(sizeof(person));
	temp->person_id=k;
	temp->status=1;
	if(aux==0)
	{
		*l=temp;
	}
	else
	{
		while(aux->link!=0)
		{
			aux=aux->link;
		}
		aux->link=temp;
	}
}
void print_state() //prints s,i,r lists
{
	printf("S: ");
	printSuspected();
	printf("I: ");
	printInfected();
	printf("R: ");
	printRecovered();
}
//event_id 1-transmit; 2-recover;
void initialise(int *l) //setting initial conditions
{
	int m=*l;
	int k;
	k=rand()%N + 1; //chosing a random frst person to be infected
	int count=1;
	while(count<=N)
	{
		append_list(&s,count); //add to list all peoples information
		count++;
	}
	count=1;
	int toss;                
	toss=rand()%10 ;
	while(toss >= 2)  //tossing a coin with a biase 0.2 which is gamma
	{
		toss=rand()%10;
		count++;
	}
	add_event(&e,m,1,k,count); //adding these events to events list
	add_event(&e,count,2,k,count);	
}
int number_events() //number of events
{
	linker aux=e;
	int count=0;
	while(aux!=0)
	{
		aux=aux->next;
		count++;
	}
	return count;
}
void pop_event(linker *l) //popping the  earliest event
{
	linker aux=*l;
	*l=aux->next;
	free(aux);
}
void fast_sir(int *k)  //this is a simple straight forward readable function
{
	int t=*k;
	while(number_events()!=0 && t<T)
	{
		if(e->event_id==1)
		{
			changeStatus(e->person_id,e->event_id); //change status of person from suspected to infected
			process_trans_sir(&t);
		}
		else if(e->event_id==2) 
		{
			changeStatus(e->person_id,e->event_id); //change status of person from infected to recovered
			process_recover_sir(&t);
		}
	}
}
void process_trans_sir(int *k)  //transmission function
{
	int t=*k;
	int aux,cutoff;
	aux=e->person_id;
	int temp;
	cutoff=e->rec_t; //to ensure that the node being processed infects others before its recovery
	int l=0;
	int j,counter;
	counter=1;
	for(j=0;j<N;j++)
	{
		if(adj_matrix[aux-1][j]==1) //are they neighbours
		{
			if(check_status(j,&s)) //if so is the neighbour already infected?
			{
				l=rand()%2;  //tossing a biased coin  (Tau=0.5)
				while(l!=0)
				{
					l=rand()%2;
					counter++;
				}
				counter=t+counter; 
				if(counter <= cutoff) //is it trasnmitted before recovery of original node
				{
					l=rand() % 10;
					temp=1;
					while(l >= 2)  //recovery (gamma=0.2)
					{
						l=rand() % 10;
						temp++;
					}
					temp=counter+temp;
					if(existevent(j)) //isnt it already in the eventspace?
					{
						add_event(&e,counter,1,j,temp);
					    add_event(&e,temp,2,j,temp);
					}
				}
			}
		}
	}
	if(e->next!=0)
	{
		//print_events(&e);
		pop_event(&e);
		while(t < (e)->t && t<T) //to make sure for every day things are printed
		{
			printf("day %d:\n",t);
			print_state();
			t++;
		}	
	}
	else
	{
		pop_event(&e);
		printf("day %d:\n",t);
		print_state();
	}
	*k=t;
}
void process_recover_sir(int *k) //recovery function simple readable
{
	int t=*k;
	if(e->next!=0)
	{
		//print_events(&e);
		pop_event(&e);
		while(t < (e)->t && t<T)
		{
			printf("day %d:\n",t);
			print_state();
			t++;
		}	
	}
	else
	{
		pop_event(&e);
		printf("day %d:\n",t);
		print_state();
	}
	*k=t;
}
/*void print_events(linker *l)
{
	linker aux=*l;
	while(aux!=0)
	{
		printf("(%d,%d,%d)--",aux->event_id,aux->t,aux->person_id);
		aux=aux->next;
	}
	printf("\n");
}*/
int check_status(int id,interlink *l) //checcking whether its already infected
{
	interlink aux;
	aux=*l;
	while(aux!=0 && aux->person_id!=id)
	{
		aux=aux->link;
	}
	if(aux!=0)
	{
		if(aux->status==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
void printSuspected() //simple print function
{
	interlink aux=s;
	while(aux!=0)
	{
		if(aux->status==1)
		{
			printf("%d--",aux->person_id);
		}
		aux=aux->link;
	}
	printf("\n");
}
void printInfected() //simple print function
{
	interlink aux=s;
	while(aux!=0)
	{
		if(aux->status==2)
		{
			printf("%d--",aux->person_id);
		}
		aux=aux->link;
	}
	printf("\n");
}
void printRecovered() //simple print function
{
	interlink aux=s;
	while(aux!=0)
	{
		if(aux->status==3)
		{
			printf("%d--",aux->person_id);
		}
		aux=aux->link;
	}
	printf("\n");
}
void changeStatus(int id,int event_id) //changing status of person being processed
{
	interlink aux;
	aux=s;
	while(aux->person_id!=id)
	{
		aux=aux->link;
	}
	if(event_id==1)
	{
		aux->status=2;
	}
	else if(event_id==2)
	{
		aux->status=3;
	}
}
int existevent(int id) //checking whether the event we want to add is already existing i.e is the persons event covered
{
	linker aux;
	aux=e;
	while(aux!=0 && aux->person_id!=id)
	{
		aux=aux->next;
	}
	if(aux==0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
		
	
	
