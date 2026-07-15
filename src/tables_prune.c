#include "tables.h"
#include "index.h"
#include "tables_ptable_data.h"

#include "thread_pool.h"
#include <unistd.h>
#include <time.h>

// dummy value to fill table with. this is the "NULL" value.
#define DUMMY_PTABLE_VALUE 15

// generic functions for genning pruning tables
// first DLS to fill the smallest values, then
// switch to a "neighbour scan", where we for depth N
// scan though all indeces (coordinates), and see if there is a 
// neighbour with stored value N - 1. Then we know that that
// coordinate (index) has value N. When to start 
// this neighbour scan is just based on my "gefühl".

struct ptable_gen_ctx {
    ptable_data_t* ptable_data;

    /* Table-specific behavior (4 function pointers): */
    bool     (*setup)(struct ptable_gen_ctx*);
    uint64_t (*init)(struct ptable_gen_ctx*, cube_t*, uint64_t*);
    uint64_t (*apply_move)(struct ptable_gen_ctx*, uint64_t*, int);
    void     (*decompose_index)(struct ptable_gen_ctx*, uint64_t, uint64_t*);

    /* Configuration: */
    int     dls_max_depth;        // H:10, DR:9
    int     nbhr_min_depth;       // H:10, DR:9
    int     nbhr_max_depth_excl;  // H:13, DR:12
    int     num_components;       // H:4, DR:3

    /* H-specific data (unused by DR): */
    uint64_t* sym_table_e_index;
    uint64_t* cclass_index_cindex_rep;
};

typedef struct {
    uint64_t start_index, end_index;
    int  depth;
    uint8_t* ptable;
    struct ptable_gen_ctx* ctx;
} NeighbourScanTask;

// DLS = Depth limited search
void table_prune_gen_DLS(
    int num_moves_done,
    int remaining_moves,
    int previous_move,
    struct ptable_gen_ctx* ctx,
    uint64_t previous_index,
    uint64_t* previous_components,
    uint8_t* ptable
){
    if (remaining_moves == 0) {
        return;
    }

    uint32_t mm = move_mask[previous_move];

    for (int move = 0; move < NMOVES; move++)
    {
        if (!move_in_move_mask(mm, move)){ continue; }

        uint64_t components[ctx->num_components];
        memcpy(components, previous_components, ctx->num_components * sizeof(uint64_t));
        uint64_t index = ctx->apply_move(ctx, components, move);

        if (index == previous_index) {
            continue;
        }

        if (ctx->ptable_data->read_value_ptable_func(index, ptable) > num_moves_done + 1) {
            ctx->ptable_data->set_value_ptable_func(index, (uint8_t) (num_moves_done + 1), ptable);
        }

        table_prune_gen_DLS(num_moves_done + 1, remaining_moves - 1, move, ctx, index, components, ptable);
    } 
}

void neighbour_scan_task(int thread_id, void* task_ptr, void* local){
    NeighbourScanTask* task = (NeighbourScanTask*)task_ptr;
    struct ptable_gen_ctx* ctx = task->ctx;

    for (uint64_t index = task->start_index; index < task->end_index; index++) {

        // check if table already filled for this value (that is, if it is different than the dummy value)
        if (ctx->ptable_data->read_value_ptable_func(index, task->ptable) != DUMMY_PTABLE_VALUE) continue;

        uint64_t components[ctx->num_components];
        ctx->decompose_index(ctx, index, components);
        
        bool found = false;
        for (int move = 0; move < NMOVES; move++) {
            uint64_t next_components[ctx->num_components];

            memcpy(next_components, components, ctx->num_components * sizeof(uint64_t));
            uint64_t next_index = ctx->apply_move(ctx, next_components, move);

            if (ctx->ptable_data->read_value_ptable_func(next_index, task->ptable) == task->depth - 1) {
                ctx->ptable_data->set_value_ptable_func(index, task->depth, task->ptable);
                found = true; break;
            }
        }

        // set the remaining cosets to the max depth
        // NOTE: We need to know that max_depth is actually
        // "gods number" for this particular table
        if (!found && task->depth == ctx->nbhr_max_depth_excl - 1) {
            ctx->ptable_data->set_value_ptable_func(index, ctx->nbhr_max_depth_excl, task->ptable);
        }
    }
}

void table_prune_gen(struct ptable_gen_ctx* ctx){
    if (!ctx->setup(ctx)){
        fprintf(stderr, "Setup for table generation failed. Aborting...\n");
        return;
    }

    cube_t cube = cube_create_new_cube();
    uint64_t components[ctx->num_components], index = ctx->init(ctx, &cube, components);
    uint8_t* ptable = malloc(ctx->ptable_data->ptable_size);

    if (ptable == NULL)
    {
        fprintf(stderr, "Could not allocate memory for ptable.\n");
        return;
    }

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    for (uint64_t i = 0; i < ctx->ptable_data->number_of_elements; i++) {
        ctx->ptable_data->set_value_ptable_func(i, DUMMY_PTABLE_VALUE, ptable);
    }
    ctx->ptable_data->set_value_ptable_func(index, 0, ptable);
    
    // Start DLS sweep
    for (int depth = 0; depth < ctx->dls_max_depth; depth++)
    {
        fprintf(stderr, "Searching at depth %i\n", depth);
        table_prune_gen_DLS(0, depth, NULLMOVE, ctx, index, components, ptable);
    }

    // start multithreaded neighbour filling sweep
    int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    ThreadPool* pool = thread_pool_create(num_threads, 0);

    uint64_t total = ctx->ptable_data->number_of_elements;
    uint64_t chunk = (total + num_threads - 1) / num_threads;

    NeighbourScanTask* tasks = malloc(num_threads * sizeof(NeighbourScanTask));
    void** task_ptrs        = malloc(num_threads * sizeof(void*));

    for (int i = 0; i < num_threads; i++) {
        tasks[i] = (NeighbourScanTask){
            .start_index = (uint64_t)i * chunk,
            .end_index   = (i == num_threads - 1) ? total : (uint64_t)(i + 1) * chunk,
            .depth       = (uint8_t)0,
            .ptable      = ptable,
            .ctx         = ctx,
        };
        task_ptrs[i] = &tasks[i];
    }

    for (int depth = ctx->nbhr_min_depth; depth < ctx->nbhr_max_depth_excl; depth++) {
        fprintf(stderr, "Searching at depth %i\n", depth);

        for (int i = 0; i < num_threads; i++) {
            tasks[i].depth = depth;
        }

        thread_pool_execute(pool, task_ptrs, num_threads, neighbour_scan_task);
    } 

    timespec_get(&end, TIME_UTC);
    double elapsed = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time used (in seconds): %f\n", elapsed);
    
    // Save and clean
    char fname[strlen(tabledir) + FILENAME_MAX];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, ctx->ptable_data->filename);

    save_table_to_file(fname, ptable, ctx->ptable_data->ptable_size);

    free(tasks);
    free(task_ptrs);

    thread_pool_destroy(pool);

    free(ptable);
    free(ctx->cclass_index_cindex_rep);
}

/* ----------------------------------------------------- */
/* implementations for functions that are table specific */
/* ----------------------------------------------------- */

/* opt1 table */
bool setup_opt1(struct ptable_gen_ctx* ctx){
    cube_tables_load_sym_table_e_index();
    uint64_t* sym_table_e_index = (uint64_t*) get_sym_table_e_index();
    if (!sym_table_e_index)
    {
        fprintf(stderr, "Could not load sym_table_e_index. Have you initialized it?\n");
        return false;
    }
    ctx->sym_table_e_index = sym_table_e_index;

    gen_move_table_ece_index();
    gen_move_table_coud_index();
    gen_move_table_eofb_index();
    gen_move_table_ccu_index();
    gen_c_sym_index_tables();

    // we fill a cclass -> cindex_rep table here. TODO: reconsider this.
    // (corner conjugacy class -> corner index representative)
    fprintf(stderr, "Preparing a cclass -> cindex_rep table. TODO: look into this!\n");
    uint64_t* cclass_index_cindex_rep = malloc(NCCLASS * sizeof(uint64_t));
    for (uint64_t k = 0; k < NCCLASS; k++)
    {
        for (uint64_t m = 0; m < NCCU * NCO; m++)
        {
            if (cclass_table[m].cclass_i == k)
            {
                cclass_index_cindex_rep[k] = cclass_table[m].cclass;
                break;
            }
        }
    }

    ctx->cclass_index_cindex_rep = cclass_index_cindex_rep;
    return true;
}
uint64_t init_opt1(struct ptable_gen_ctx* ctx, cube_t* cube, uint64_t* components){
    uint64_t ccu  = cube_to_cc_index(cube, UD);
    uint64_t coud = cube_to_co_index(cube, UD);
    uint64_t ece  = cube_to_ec_index(cube, UD);
    uint64_t eofb = cube_to_eo_index(cube, UD);
    uint64_t index    = cube_to_H_index(cube, UD);
    components[0] = ccu;
    components[1] = coud;
    components[2] = ece;
    components[3] = eofb;
    return index;
}

uint64_t apply_move_opt1(struct ptable_gen_ctx* ctx, uint64_t* components, int move){
    uint64_t ccu = components[0];
    uint64_t coud = components[1];
    uint64_t ece = components[2];
    uint64_t eofb = components[3];

    uint64_t ccu2  = move_table_ccu_index[ccu][move];
    uint64_t coud2 = move_table_coud_index[coud][move];
    uint64_t ece2  = move_table_ece_index[ece][move];
    uint64_t eofb2 = move_table_eofb_index[eofb][move];

    struct c_index_cclass_sym c = cclass_table[cc_co_to_c_index(ccu2, coud2)];

    uint64_t e2     = ec_eo_to_e_index(ece2, eofb2);
    uint64_t e2_sym = ctx->sym_table_e_index[e2 * NSYMS + c.sym];

    uint64_t p2 = cclass_i_e_to_H_index(c.cclass_i, e2_sym);

    components[0] = ccu2;
    components[1] = coud2;
    components[2] = ece2;
    components[3] = eofb2;

    return p2;
}

void decompose_index_opt1(struct ptable_gen_ctx* ctx, uint64_t index, uint64_t* components){
    uint64_t cclass_i = index % NCCLASS;
    uint64_t ei       = index / NCCLASS;

    uint64_t cindex_rep = ctx->cclass_index_cindex_rep[cclass_i];
    uint64_t ccu        = cindex_rep % NCCU;
    uint64_t coud       = cindex_rep / NCCU;

    uint64_t ece  = ei % NECE;
    uint64_t eofb = ei / NECE;

    components[0] = ccu;
    components[1] = coud;
    components[2] = ece;
    components[3] = eofb;
}

/* DR table */

bool setup_DR(struct ptable_gen_ctx* ctx){
    gen_move_table_coud_index();
    gen_move_table_eofb_index();
    gen_move_table_ece_index();
    return true;
}

uint64_t init_DR(struct ptable_gen_ctx* ctx, cube_t* cube, uint64_t* components){
    uint64_t coud  = cube_to_co_index(cube, UD);
    uint64_t eofb  = cube_to_eo_index(cube, UD);
    uint64_t eceud = cube_to_ec_index(cube, UD);
    uint64_t index = cube_to_DR_index(cube, UD);

    components[0] = coud;
    components[1] = eofb;
    components[2] = eceud;

    return index;
}

uint64_t apply_move_DR(struct ptable_gen_ctx* ctx, uint64_t* components, int move){
    uint64_t coud = components[0];
    uint64_t eofb = components[1];
    uint64_t ece = components[2];

    uint64_t coud2 = move_table_coud_index[coud][move];
    uint64_t eofb2 = move_table_eofb_index[eofb][move];
    uint64_t ece2 = move_table_ece_index[ece][move];

    components[0] = coud2;
    components[1] = eofb2;
    components[2] = ece2;

    uint64_t p2 = coud2 + NCO * (eofb2 + NEO * ece2);

    return p2;
}
void decompose_index_DR(struct ptable_gen_ctx* ctx, uint64_t index, uint64_t* components){
    uint64_t coud = index % NCO;
    uint64_t eofb = (index / NCO) % NEO;
    uint64_t ece  = (index / NCO) / NEO;

    components[0] = coud;
    components[1] = eofb;
    components[2] = ece;
}

void gen_ptable_DR(){
    struct ptable_gen_ctx ctx = {
        .ptable_data = &ptable_data_dr,
        .num_components = 3,
        .nbhr_max_depth_excl = 12,
        .nbhr_min_depth = 9,
        .dls_max_depth = 9,
        .apply_move = &apply_move_DR,
        .init = &init_DR,
        .setup = &setup_DR,
        .decompose_index = &decompose_index_DR,
    };

    table_prune_gen(&ctx);
}

void gen_ptable_opt1(){
    struct ptable_gen_ctx ctx = {
        .ptable_data = &ptable_data_opt1,
        .num_components = 4,
        .nbhr_max_depth_excl = 13,
        .nbhr_min_depth = 9,
        .dls_max_depth = 9,
        .apply_move = &apply_move_opt1,
        .init = &init_opt1,
        .setup = &setup_opt1,
        .decompose_index = &decompose_index_opt1,
    };

    table_prune_gen(&ctx);
}

/* ------------------------------------------------------ */
/* ------------------------- other ---------------------- */
/* ------------------------------------------------------ */

void analyze_ptable(ptable_data_t ptable_data){
    if (!ptable_data.ptable_is_loaded){
        fprintf(stderr, "UPPSI, we aint got tabl\n");
        return;
    }

    uint8_t* ptable = (uint8_t*) ptable_data.ptable;

    if (!ptable)
    {
        fprintf(stderr, "UPPSI, we aint got tabl 2\n");
        return;
    }

    uint64_t stats[21] = {0};

    uint64_t other = 0;
    for (uint64_t i = 0; i < ptable_data.number_of_elements; i++)
    {
        uint8_t val = ptable_data.read_value_ptable_func(i, ptable);
        // if (val == 15)
        // {
        //     other++;
        // }
        // else
        // {
            stats[val]++;
        // }
    }

    for (int i = 0; i <= 20; i++)
    {
        printf("%i: %lu\n", i, stats[i]);
    }
    printf("other: %lu\n", other);
}

// I genned a file with cp <-> dr_subset for all cp using GAP.
bool parse_cp_to_dr_subset_file_and_save_dr_subset_table(char* filename){
    int cp_subsets[FACTORIAL8];

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    int count = 0;
    while (count < FACTORIAL8) {
        int arr[NCORNERS];
        int label;

        int n = fscanf(fp,
            " [ %d , %d , %d , %d , %d , %d , %d , %d ] : %d",
            &arr[0], &arr[1], &arr[2], &arr[3],
            &arr[4], &arr[5], &arr[6], &arr[7],
            &label
        );

        if (n != 9) {
            break;  // EOF or malformed line
        }

        for (int i = 0; i < NCORNERS; i++){
            arr[i]--; // GAP uses 1..8 instead of 0..7.
        }

        count++;

        cp_subsets[perm_to_fact(arr, NCORNERS)] = label;
    }


    /* end of TODO */
    char fname[strlen(tabledir) + FILENAME_MAX];

    strcpy(fname, tabledir);
    strcat(fname, "/");
    strcat(fname, "dr_subsets.dat");

    save_table_to_file(fname, cp_subsets, FACTORIAL8 * sizeof(int));

    fclose(fp);

    return 0;


}
