/*****************************************************************************************
 *              MIT License                                                              *
 *                                                                                       *
 * Copyright (c) 2022 G. Cherchi, M. Livesu, R. Scateni, M. Attene and F. Pellacini      *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION     *
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                *
 *                                                                                       *
 * Authors:                                                                              *
 *      Gianmarco Cherchi (g.cherchi@unica.it)                                           *
 *      https://people.unica.it/gianmarcocherchi/                                        *
 *                                                                                       *
 *      Marco Livesu (marco.livesu@ge.imati.cnr.it)                                      *
 *      http://pers.ge.imati.cnr.it/livesu/                                              *
 *                                                                                       *
 *      Riccardo Scateni (riccardo@unica.it)                                             *
 *      https://people.unica.it/riccardoscateni/                                         *
 *                                                                                       *
 *      Marco Attene (marco.attene@ge.imati.cnr.it)                                      *
 *      https://www.cnr.it/en/people/marco.attene/                                       *
 *                                                                                       *
 *      Fabio Pellacini (fabio.pellacini@uniroma1.it)                                    *
 *      https://pellacini.di.uniroma1.it                                                 *
 *                                                                                       *
 * ***************************************************************************************/

#ifndef FASTTRIMESH_H
#define FASTTRIMESH_H

#include <implicit_point.h>
#include "tree.h"
#include "common.h"
#include <tbb/tbb.h>

#include "phmap.h"

#include <absl/container/inlined_vector.h>
template<typename T>
using fmvector = absl::InlinedVector<T, 16>;

struct iVtx
{
    iVtx() : p(nullptr), info(0){}
    iVtx(const genericPoint *_p, uint _info) : p(_p), info(_info){}

    const genericPoint* p;
    uint info;
};

struct iEdge
{
    iEdge() : v(0,0), constr(false) { }
    iEdge(uint _v0, uint _v1, const bool _b) : v(_v0, _v1), constr(_b){}

    std::pair<uint, uint> v;
    bool constr = false;
};

struct iTri
{
    iTri() : v{0,0,0}, info(0) { }
    iTri(uint _v0, uint _v1, uint _v2, uint _n) : info(_n)
    {
        v[0] = _v0;
        v[1] = _v1;
        v[2] = _v2;
    }

    uint v[3];
    uint info = 0;
};

class FastTrimesh
{
    public:

        FastTrimesh(){}

        FastTrimesh(const genericPoint* tv0, const genericPoint* tv1, const genericPoint *tv2, const uint *tv_id, const Plane &ref_p);

        FastTrimesh(const std::vector<genericPoint*> &in_verts, const std::vector<uint> &in_tris, bool parallel);


        void preAllocateSpace(uint estimated_num_verts);

        void resetTrianglesInfo();

        uint numVerts() const;
        uint numEdges() const;
        uint numTris() const;
        Plane refPlane() const;

        // VERTICES
        const genericPoint* vert(uint v_id) const;

        uint vertOrigID(uint new_v_id) const; // from new_id to original mesh id

        uint vertNewID(uint orig_v_id) const; // from original mesh id to new_id

        uint vertValence(uint v_id) const;

        const fmvector<uint> &adjV2E(uint v_id) const;

        fmvector<uint> adjV2T(uint v_id) const;

        void resetVerticesInfo();

        void setVertInfo(const uint v_id, const uint info);

        uint vertInfo(const uint v_id) const;

        // EDGES
        const std::pair<uint, uint> &edge(uint e_id) const;

        uint edgeVertID(uint e_id, uint off) const;

        int edgeID(uint ev0_id, uint ev1_id) const;

        bool edgeIsConstr(uint e_id) const;

        void setEdgeConstr(uint e_id);

        uint edgeOppToVert(uint t_id, uint v_id) const;

        bool edgeIsBoundary(uint e_id) const;

        bool edgeIsManifold(uint e_id) const;

        const fmvector<uint> &adjE2T(uint e_id) const;

        void edgeSetVisited(uint e_id, const bool &vis);

        bool edgeIsVisited(uint e_id) const;


        // TRIANGLES
        const uint *tri(uint t_id) const;

        int triID(uint tv0_id, uint tv1_id, uint tv2_id) const;

        uint triVertID(uint t_id, uint off) const;

        const genericPoint *triVert(uint t_id, uint off) const;

        int triEdgeID(uint t_id, uint off) const;

        uint triNodeID(uint t_id) const;

        void setTriNodeID(uint t_id, uint n_id);

        uint triVertOppositeTo(uint t_id, uint v0_id, uint v1_id) const;

        int triOppToEdge(uint e_id, uint t_id) const;

        fmvector<uint> adjT2E(uint t_id) const;
        std::vector<std::array<uint, 3>> adjT2EAll(bool parallel) const;

        fmvector<uint> adjT2T(uint t_id) const;

        bool triVertsAreCCW(uint t_id, uint curr_v_id, uint prev_v_id) const;

        int triOrientation(uint t_id) const;

        bool triContainsVert(uint t_id, uint v_id) const;

        uint triVertOffset(uint t_id, uint v_id) const;

        uint triInfo(uint t_id) const;

        void setTriInfo(uint t_id, uint val);

        // MESH MANIPULATION
        uint addVert(const genericPoint *v, uint orig_v_id);

        void addVert(const genericPoint *v);

        uint addTri(uint tv0_id, uint tv1_id, uint tv2_id);

        void removeEdge(uint e_id);

        void removeTri(uint t_id);

        void removeTris(const std::vector<uint> &t_ids);
        void removeTris(const fmvector<uint> &t_ids);

        void splitEdge(const uint  &e_id, uint v_id);

        void splitEdge(const uint  &e_id, uint v_id, Tree &tree);

        void splitTri(uint t_id, uint v_id);

        void splitTri(uint t_id, uint v_id, Tree &tree);

        void flipTri(uint t_id);

    private:
        std::vector<iVtx>    vertices;
        std::vector<iEdge>   edges;
        std::vector<iTri>    triangles;

        std::vector< fmvector<uint> >    v2e;
        std::vector< fmvector<uint> >    e2t;

        phmap::flat_hash_map <uint, uint> rev_vtx_map;

        Plane triangle_plane;

        // PRIVATE METHODS
        int addEdge(uint ev0_id, uint ev1_id);

        bool edgeContainsVert(uint e_id, uint v_id) const;

        void removeFromVec(fmvector<uint> &vec, uint elem);

        void triSwitch(uint t0_id, uint t1_id);

        void edgeSwitch(uint e0_id, const uint e1_id);

        void removeEdgeUnref(uint e_id);

        void removeTriUnref(uint t_id);
};

// #include "fast_trimesh.cpp"

#endif // FASTTRIMESH_H


