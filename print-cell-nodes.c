#include "udf.h"

#include <limits.h>

/*!
    @brief Print the node coordinates per cell in a format useful in `c`/`c++`.

    Prints the coordinates of the nodes that compose each cell (using the connectivity
    order, described in section 6.1.2.3 of teh Fluent User's Guide).

    Currently prints the data in Fluent TUI.
*/
DEFINE_ON_DEMAND(print_cell_nodes)
{
#if !RP_NODE
    Message("\n...Printing cell node coordinates:");
    Message("\n...format: cell-index, cell-id, cell-type, cell-number-nodes, ");
    Message("node-0-x, node-0-y, node-0-z, ..., node-n-x, node-n-y, node-n-z");
#endif  // !RP_NODE
    Domain * domain = NULL;
    Thread * thread = NULL;
    cell_t cell = NULL_CELL;
    Node * node = NULL;
    cxindex node_index = NULL_INDEX;
    cxindex cells_labeling = NULL_INDEX;
    cxindex faces_labeling = NULL_INDEX;

    domain = Get_Domain(ROOT_DOMAIN_ID);

#if !PARALLEL
    // When running in serial, activate the global cell numbering.
    Label_Cells(domain);
    Label_Faces(domain, FALSE);
    Label_Nodes(domain, FALSE);
#endif /*!PARALLEL*/

#if !RP_HOST
    FILE * csv_file = NULL;
    char csv_file_name[UCHAR_MAX] = "cell-nodes-";
    sprintf(csv_file_name + strlen(csv_file_name), "%06d.csv", myid);
#ifdef VERBOSE
    Message("\n...node %d cell data (%s):", myid, csv_file_name);
#endif  // VERBOSE
    csv_file = fopen(csv_file_name, "w");
    fprintf(csv_file, "# cell-index, cell-id, cell-type, cell-number-nodes, ");
    fprintf(csv_file, "node-0-x, node-0-y, node-0-z, ..., node-n-x, node-n-y, node-n-z\n");
    thread_loop_c(thread, domain)
    {
#ifdef VERBOSE
        Message("\n...thread %d cell data:", THREAD_ID(thread));
#endif  // VERBOSE
        if (THREAD_N_ELEMENTS_INT(thread) > 0)
        {
            begin_c_loop_int(cell, thread)
            {
#ifdef VERBOSE
                Message(
                    "%d, %d, %d, %d",
                    cell,
                    C_ID(cell, thread),
                    C_TYPE(cell, thread),
                    C_NNODES(cell, thread)
                );
#endif  // VERBOSE
                fprintf(
                    csv_file,
                    "%d, %d, %d, %d",
                    cell, C_ID(cell, thread),
                    C_TYPE(cell, thread),
                    C_NNODES(cell, thread)
                );
                c_node_loop(cell, thread, node_index)
                {
                    node = C_NODE(cell, thread, node_index);
#ifdef VERBOSE
                    Message("\n\t%.15e, %.15e, %.15e", NODE_X(node), NODE_Y(node), NODE_Z(node));
#endif  // VERBOSE
                    fprintf(csv_file, ", %.15e, %.15e, %.15e", NODE_X(node), NODE_Y(node), NODE_Z(node));
                }
                fprintf(csv_file, "\n");
            } end_c_loop_int(cell, thread)
        }
    }
    fclose(csv_file);
#endif  // RP_NODE
}
