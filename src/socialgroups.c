/**
 *  file: main.c
 *  program: SocialGroups
 *
 *  @author João Pinho
 *  @date   10/03/14.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* constants */
#define UNDEFINED -1

/* data types */
typedef struct node		 { int value; struct node* next;						} node;
typedef struct list		 { node* head; node* tail; int length;					} list;
typedef struct vertex	 { int value; list* edges; int scc_index; int low_link;	} vertex;
typedef struct graph	 { vertex**	vertices; int length;                       } graph;
typedef struct scc_graph { int length; int count_scc; int* vertices; int* scc_s, *scc_f; int count_nonsocial; int scc_max_size;	} scc_graph;

/* globals */
vertex** vertices;
int *sc_vertices, *scc_s, *scc_f, *sc_stack, *sp, *visit_nos, *low_link_nos, *unvisited, *p;
int n_visited, n_written, count_scc, stack_pos, n_unvisited;

/* prototypes */
node* node_new();
list* list_new();
void list_add(list*, int);

vertex* vertex_new();
graph* graph_new(int);
void graph_add(graph*, int, int);

scc_graph* scc_graph_new(int);
scc_graph* tarjan_scc(graph*);
void tarjan_scc_recursive(int,scc_graph*);
int scc_classify_nonsocial();

/* functions - node */
node* node_new(){
	node* n = (node*)malloc(sizeof(node));
	n->next = NULL;
	n->value=0;
	return n;
}

/* functions - vertex */
vertex* vertex_new(){
	vertex* v = (vertex*)malloc(sizeof(vertex));
	v->edges = NULL;
	v->value = 0;
	return v;
}

/* functions - list */
list* list_new(){
	list* lst = (list*)malloc(sizeof(list));
	lst->head = lst->tail = NULL;
	lst->length=0;
	return lst;
}

void list_add(list* lst, int value){
	node* n = node_new();
	n->value=value;
	
	if(lst->head==NULL) lst->head = n;
	else lst->tail->next=n;
    
	lst->tail = n;
	lst->length++;
}

/* functions - graph */
graph* graph_new(int vertices){
	graph* grp = (graph*)malloc(sizeof(graph));
	grp->vertices = (vertex**)calloc(vertices, sizeof(vertex*));
	grp->length = vertices;
	return grp;
}

void graph_add(graph* grp, int vertice, int edge){
	if(grp->vertices[vertice-1] == NULL){
		grp->vertices[vertice-1] = vertex_new();
		grp->vertices[vertice-1]->value = vertice;
		grp->vertices[vertice-1]->edges = list_new();
	}
	list_add(grp->vertices[vertice-1]->edges, edge);
}

/* functions - scc graph */
scc_graph* scc_graph_new(int length){
	scc_graph* g = (scc_graph*)malloc(sizeof(scc_graph));
	g->vertices = (int*)malloc(length * sizeof(int));
	g->scc_s = (int*)malloc(length * sizeof(int));
	g->scc_f = (int*)malloc(length * sizeof(int));
	g->length = length;
    g->scc_max_size=0;
	return g;
}

/* functions - tarjan scc */
scc_graph* tarjan_scc(graph* g){
	int i, v, length;
	scc_graph* result;
	
	length = n_unvisited = g->length;
	vertices = g->vertices;
	
	result = scc_graph_new(length);
    sc_vertices = result->vertices;
    scc_s = result->scc_s;
    scc_f = result->scc_f;
    
	/* support structures init */
    sc_stack = (int*)malloc(length * sizeof(int));
    sp = (int*)malloc(length * sizeof(int));
    visit_nos = (int*)malloc(length * sizeof(int));
    low_link_nos = (int*)malloc(length * sizeof(int));
	unvisited = (int*)malloc(length *sizeof(int));
    p = (int*)malloc(length *sizeof(int));
    
	/* integers init */
    for(i = 0; i < length; i++) {
		scc_s[i] = scc_f[i] = visit_nos[i] = sp[i] = UNDEFINED;
		p[i] = unvisited[i] = i;
    }
    count_scc = stack_pos = n_written = n_visited = 0;
    
	do {
		v = unvisited[0];
        tarjan_scc_recursive(v, result);
    } while(n_unvisited);
    
    result->count_scc = count_scc;
	result->count_nonsocial = scc_classify_nonsocial();
    
    free(sc_stack);
    free(sp);
    free(visit_nos);
    free(low_link_nos);
	free(unvisited);
	free(p);
    
    return result;
}

void tarjan_scc_recursive(int v, scc_graph* result){
    node* edge;
    int w, scc_vertex, count;
    
    /* add vertex v to the result, and increase the counter for the number of
     * vertices in the result. */
    low_link_nos[v] = visit_nos[v] = n_visited;
    n_visited++;
    
    /* remove the visited vertex from the array containing unvisited vertices. */
    n_unvisited--;
    scc_vertex = unvisited[p[v]] = unvisited[n_unvisited];
    p[scc_vertex] = p[v];
    
    /* place v on the stack. */
    sc_stack[stack_pos] = v;
    sp[v] = stack_pos;
    stack_pos++;
    
    /* note the algorithm is like a recursive DFS from each w in v that is still unvisited. */
    edge = vertices[v] == NULL ? NULL : vertices[v]->edges->head;
    while(edge) {
        w = (edge->value - 1);
        
        if(visit_nos[w] == UNDEFINED) {
			tarjan_scc_recursive(w, result);
            
			/* update low_link no. */
			if(low_link_nos[w] < low_link_nos[v])
				low_link_nos[v] = low_link_nos[w];
		}
		else if(visit_nos[w] < visit_nos[v] && sp[w] != UNDEFINED) {
			/* update low_link no. */
		    if(visit_nos[w] < low_link_nos[v])
		    	low_link_nos[v] = visit_nos[w];
		}
        edge = edge->next;
    }
    
    /* if v is a root node, pop the stack and generate an SCC */
    if(low_link_nos[v] == visit_nos[v]) {
		scc_s[count_scc] = n_written;
        count=0;
        
        /* The SC component vertices are stored from the top of the stack, down
		 * to v.  Remove these vertices, and write them to the result structure. */
        do {
			stack_pos--;
			scc_vertex = sc_vertices[n_written] = sc_stack[stack_pos];
			sp[scc_vertex] = UNDEFINED;
            
			if(vertices[sc_vertices[n_written]]==NULL)
				vertices[sc_vertices[n_written]] = vertex_new();
            
			vertices[sc_vertices[n_written]]->scc_index = count_scc;
			n_written++;
            count++;
		} while(scc_vertex != v);
        
        if(count > result->scc_max_size)
            result->scc_max_size = count;
        
		scc_f[count_scc] = n_written-1;
		count_scc++;
    }
}

int scc_classify_nonsocial(){
	int scc_connect=0, i, j, count=0, issocial;
	vertex* the_vertice = NULL;
	node* edge;
	int* connections = (int*)calloc(count_scc, sizeof(int));
    
	for(i=0; i<count_scc; i++){
        issocial=0;
        
		for(j=scc_s[i]; j<=scc_f[i]; j++){
			the_vertice = vertices[sc_vertices[j]];
			
            if(the_vertice == NULL || the_vertice->edges == NULL) continue;

			edge = the_vertice->edges->head;
			while(edge != NULL){
				scc_connect = vertices[(edge->value-1)]->scc_index;
                
				if(scc_connect != i){
                    issocial=1;
                    break;
                }
                
				edge = edge->next;
			}
            
            if(issocial) break;
		}
        
        if(!issocial) count++;
	}
    
    free(connections);
	return count;
}

/* main - socialgroups */
int main(int argc, char* argv[]) {
	graph* socials = NULL;
	int lineno=0, npeople=0, sharedfrom=0, sharedto=0;
	long nshares=0;
    scc_graph* result;
    
    clock_t tic = clock();
	clock_t toc;
    
    fscanf(stdin, "%d %ld\n", &npeople, &nshares);
    socials = graph_new(npeople);
    
    for(lineno=0; (lineno < nshares) && (fscanf(stdin, "%d %d\n", &sharedfrom, &sharedto) != EOF); lineno++)
        graph_add(socials, sharedfrom, sharedto);
    
    result = tarjan_scc(socials);
    printf("%d\n", result->count_scc);
    printf("%d\n", result->scc_max_size);
    printf("%d\n", result->count_nonsocial);

	toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    free(result);
	free(socials);

	return 0;
}
