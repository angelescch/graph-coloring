#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "FuncionesColoreo.h"
#include "queue.h"

typedef struct tuple_s* tuple;

struct tuple_s {
    u32 v1;
    u32 v2;
};

static int sort_tuple(const void *a, const void *b) {
    u32 l = ((struct tuple_s *)a)->v1;
    u32 r = ((struct tuple_s *)b)->v1;
    int res;
    if (l < r) {
        res = -1;
    }
    else if (l > r) {
        res = 1;
    }
    else {
        res = 0;
    }
    return res;
}


u32* Bipartito(Grafo G) {
    u32 n = NumeroDeVertices(G);
    u32 * colores = calloc(n, sizeof(u32));
    queue vertex_queue = queue_empty();
    colores[0u] = 1;
    u32 v_colored = 1;
    vertex_queue = queue_enqueue(vertex_queue, 0u);

    u32 breaking = 0;
    u32 k = 1;

    while (!queue_is_empty(vertex_queue) && v_colored < n && !breaking) {
        u32 i = queue_first(vertex_queue);
        vertex_queue = queue_dequeue(vertex_queue);

        for (u32 j=0u; j < Grado(i,G) && !breaking ;++j) {
            u32 vec_index = IndiceONVecino(j,i,G);
            if (colores[vec_index]==0u) {
                colores[vec_index] = 3 - colores[i];
                v_colored++;
                vertex_queue = queue_enqueue(vertex_queue, vec_index);
            } else if (colores[vec_index]==colores[i]) {
                breaking = 1;
            }
        }

        if (queue_is_empty(vertex_queue) && v_colored < n) {
            while(colores[k]!=0u){ 
                k++;
            }
            if (k < n) {
                colores[k] = 1u;
                v_colored++;
                vertex_queue = queue_enqueue(vertex_queue, k);
            }
        }
    }
    queue_destroy(vertex_queue);

    if(breaking){
        free(colores);
        colores = NULL;
    }

    return colores;
}


u32 Greedy(Grafo G,u32* Orden,u32* Coloreo) {
    u32 n = NumeroDeVertices(G);
    u32 cant_colores = 1u;

    Coloreo[Orden[0]] = 0u;

    for (unsigned int u = 1u; u < n; ++u) {
        Coloreo[Orden[u]] = UINT32_MAX;
    }

    bool * available = calloc(n,sizeof(bool));
    if (available != NULL) {
        for (unsigned int cr = 0u; cr < n; ++cr) {
            available[cr] = true;
        }

        for (unsigned int i = 1u; i < n; ++i) {
            u32 index_i = Orden[i];

            u32 grado_i = Grado(index_i,G);
            for (u32 j = 0u; j < grado_i; ++j) {
                if (Coloreo[IndiceONVecino(j,index_i,G)]!=UINT32_MAX) {
                    available[Coloreo[IndiceONVecino(j,index_i,G)]] = false;
                }
            }

            unsigned int cr=0u;
            while (cr < n) {
                if (available[cr])
                    break;
                ++cr;
            }

            Coloreo[index_i] = cr;

            if (cr+1u > cant_colores)
                ++cant_colores;

            for (u32 j = 0u; j < grado_i; ++j) {
                if (Coloreo[IndiceONVecino(j,index_i,G)]!=UINT32_MAX) {
                    available[Coloreo[IndiceONVecino(j,index_i,G)]] = true;
                }
            }
        }
        free(available);
        available = NULL;
    } else {
        cant_colores =  UINT32_MAX;
    }

    // Sheogorath insistió en que obviamente esto es absurdo...
    if (Delta(G)+1<cant_colores) {
        cant_colores = UINT32_MAX;
    }

    return cant_colores;
}


//ordenamiento a partir de clave
char OrdenFromKey(u32 n,u32* key,u32* Orden) {
    tuple value_index = calloc(n, sizeof(struct tuple_s));
    if (value_index == NULL)
        return '1';
    for(u32 i = 0u; i < n; ++i) {
        value_index[i].v1 = key[i];
        value_index[i].v2 = i;
    }
    qsort((void*)value_index, n, sizeof(struct tuple_s), sort_tuple);
    u32 i = n-1u;
    u32 j = 0u;
    while(j < n) {
        Orden[j] = value_index[i].v2;
        ++j;
        --i;
    }
    free(value_index);
    value_index = NULL;
    return '0';
}


//claves aleatorias
void AleatorizarKeys(u32 n,u32 R,u32* key) {
    for (u32 i=0u; i < n ;++i) {
        srand(R+i);
        u32 random = rand();
        key[i] = random % n;
    }
}


//permutadores de colores
u32* PermutarColores(u32 n,u32* Coloreo,u32 R) {
    u32* ColoreoNuevo = calloc(n, sizeof(u32));

    if (ColoreoNuevo != NULL) {
        u32 r = 0u;
        for(u32 i = 0u; i < n; ++i) {
            if (Coloreo[i] > r) {
                r = Coloreo[i];
            }
        }
        ++r;
        u32* PermC = calloc(r, sizeof(u32));
        if (PermC == NULL) return NULL;
        srand(R);
        u32 random = rand();
        for (u32 i = 0u ; i < r ; ++i) {
            PermC[i] = (random+i) % r;
        }
        for (u32 i = 0u ; i<n ; ++i) {
            ColoreoNuevo[i] = PermC[Coloreo[i]];
        }
        free(PermC);
    }

    return ColoreoNuevo;
}


u32* RecoloreoCardinalidadDecrecienteBC(u32 n,u32* Coloreo) {
    u32* NuevoColoreo = calloc(n,sizeof(u32));
    tuple ConteoColores = calloc(n,sizeof(struct tuple_s));


    if (NuevoColoreo!=NULL && ConteoColores!=NULL) {
        // contar iterando una vez los colores: ConteoColoreo[Coloreo[i]] = cantidad de veces que se repite el color Coloreo[i]
        for (u32 i = 0; i < n; ++i) {
            ++ConteoColores[Coloreo[i]].v1;
            ConteoColores[Coloreo[i]].v2 = Coloreo[i];
        }

        u32 r = Coloreo[0];      // cuenta colores
        for (u32 i = 1; i < n; ++i) {
            if (r < Coloreo[i])
                r = Coloreo[i];
        }
        ++r;

        qsort((void*)ConteoColores, r, sizeof(struct tuple_s), sort_tuple);

        u32* CambiarColor = calloc(r,sizeof(u32));
        if (CambiarColor == NULL) return NULL;

        u32 i = r-1u;
        u32 j = 0u;
        while(j < r) {
            CambiarColor[ConteoColores[i].v2] = j;
            ++j;
            --i;
        }

        for (u32 i = 0; i < n; ++i) {
            NuevoColoreo[i] = CambiarColor[Coloreo[i]];
        }

        free(CambiarColor);

    } else {
        NuevoColoreo = NULL;
    }

    free(ConteoColores);

    return NuevoColoreo;
}