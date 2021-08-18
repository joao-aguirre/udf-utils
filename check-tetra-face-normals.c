#include "udf.h"


DEFINE_ON_DEMAND(check_tetra_face_normals)
{
    const int fluid_domain = 1;
    const int invalid_index = -1;

    /* Tetrahedron cell face connectivity list given in section 6.1.2.3 of the Fluent
    User's Guide. Changed to use 0 as the starting node index. */
    const int face_connectivity[4][3] =
    {
        { 3, 2, 1 },
        { 2, 3, 0 },
        { 1, 0, 3 },
        { 2, 0, 1 }
    };

    Domain* domain = NULL;
    Thread* cell_thread = NULL;
    Thread* face_thread = NULL;
    cell_t cell = invalid_index;
    face_t face = invalid_index;
    Node* cell_node[4] = { NULL, NULL,NULL, NULL };
    cxboolean are_all_vectors_ok = FALSE;
    int face_index = invalid_index;
    int node_index = invalid_index;
    real NV_VEC(cell_centroid);
    real NV_VEC(face_centroid);
    real NV_VEC(face_normal);
    real NV_VEC(face_tangent_vector_01);
    real NV_VEC(face_tangent_vector_02);
    real NV_VEC(cell_face_centroids_vector);

#if !RP_NODE
    Message("\n...checking tetrahedron cells face normal orientation:")
#endif /*RP_NODE*/
    PRF_GSYNC();

#if !RP_HOST
    are_all_vectors_ok = TRUE;
    domain = Get_Domain(fluid_domain);
    thread_loop_c(cell_thread, domain)
    {
        if (THREAD_N_ELEMENTS_INT(cell_thread) > 0)
        {
            begin_c_loop_int(cell, cell_thread)
            {
                /* Only perform the face normal check for tetrahedrons. */
                if (C_TYPE(cell, cell_thread) == TET_CELL)
                {
                    C_CENTROID(cell_centroid, cell, cell_thread);
                    c_face_loop(cell, cell_thread, face_index) {
                        face = C_FACE(cell, cell_thread, face_index);
                        face_thread = C_FACE_THREAD(cell, cell_thread, face_index);
                        F_CENTROID(face_centroid, face, face_thread);
                        NV_VV(cell_face_centroids_vector, =, face_centroid, -, cell_centroid);
                        /* Here I am using the face connectivity described in the
                        documentation instead of using the nodes of the face themselves
                        (in the order defined on the face structure). That is because
                        Fluent does not duplicate face data in memory, so for each
                        internal face, there will be one cell with the face normal
                        pointing outward of the cell and another one with the face
                        normal pointing inside the cell (because the face was created
                        using the neighbor data, pointing out of the neighbor cell). */
                        NV_VV(
                            face_tangent_vector_01,
                            =,
                            C_NODE(cell, cell_thread, face_connectivity[face_index][1])->x,
                            -,
                            C_NODE(cell, cell_thread, face_connectivity[face_index][0])->x
                        );
                        NV_VV(
                            face_tangent_vector_02,
                            =,
                            C_NODE(cell, cell_thread, face_connectivity[face_index][2])->x,
                            -,
                            C_NODE(cell, cell_thread, face_connectivity[face_index][0])->x
                        );
                        NV_CROSS(face_normal, face_tangent_vector_01, face_tangent_vector_02);
                        /* If the face normal is pointed out of the cell, the dot
                        product of the normal vector calculated above and the vector
                        from the cell centroid to the face centroid will be positive.
                        Otherwise there is an inconsistency on the connectivity. */
                        if (NV_DOT(cell_face_centroids_vector, face_normal) < 0.0)
                        {
                            Message(
                                "\n...face connectivity inconsistency in cell %d, face %d!",
                                cell,
                                face_index
                            );
                            are_all_vectors_ok = FALSE;
                        }
                    }
                }
            } end_c_loop_int(cell, cell_thread)
        }
    }
#endif /*!RP_HOST*/
    PRF_GBAND1(are_all_vectors_ok);
    node_to_host_boolean_1(are_all_vectors_ok);
#if !RP_NODE
    if (are_all_vectors_ok)
    {
        Message("\n...no inconsistencies found.");
    }
#endif /*RP_NODE*/
}