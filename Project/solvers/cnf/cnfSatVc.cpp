#include "cnfSatVc.hpp"
#include <algorithm>
using namespace std;

std::vector<std::vector<Minisat::Lit>> construct_matrix(std::vector<std::vector<Minisat::Lit>> &matrix, Minisat::Solver &solver, int &mid)
{
    std::generate(matrix.begin(), matrix.end(), [=, &solver]()
                  {
        std::vector<Minisat::Lit> row(mid);
        std::transform(row.begin(), row.end(), row.begin(), [=, &solver](Minisat::Lit&) {
            return Minisat::mkLit(solver.newVar());
        });
        return row; });
    return matrix;
}

void solver_add_clause(std::vector<std::vector<Minisat::Lit>> &matrix, Minisat::Solver &solver, int &mid, int &vertex, bool is_swap)
{
    for (int i = 0; i < vertex; i++)
    {
        for (int j = 0; j < mid - 1; j++)
        {
            for (int k = j + 1; k < mid; k++)
            {
                solver.addClause(~matrix[is_swap ? j : i][is_swap ? i : j], ~matrix[is_swap ? k : i][is_swap ? i : k]); // use variables as mentioned in encoding to prevent confusion Cat!!!
            }
        }
    }
}

std::vector<int> output_vector(std::vector<std::vector<Minisat::Lit>> &matrix, Minisat::Solver &solver, int &vertex, int &mid)
{
    std::vector<int> result(vertex * mid);
    auto it = result.begin();

    std::generate(result.begin(), result.end(), [&]()
                  {int i = std::distance(result.begin(), it) / mid;
                           int j = std::distance(result.begin(), it) % mid;
                          ++it;
                        return Minisat::toInt(solver.modelValue(matrix[i][j])) == 0 ? i : -1; });

    result.erase(std::remove(result.begin(), result.end(), -1), result.end());
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<int> cnfSatVc(int &vertex, std::vector<std::pair<int, int>> &edges)
{
    int low = 0;
    int high = vertex;
    std::vector<int> result;

    while (low <= high)
    {
        int mid = float((low + high) / 2);
        Minisat::Solver solver;
        std::vector<std::vector<Minisat::Lit>> matrix(vertex);

        // creating literals create a 2D array
        matrix = construct_matrix(matrix, solver, mid);

        // 1: At least one vertex is the ith vertex in the vertex cover: ∀i ∈ [1,k], a clause (x1,i ∨x2,i ∨···∨xn,i)
        for (int i = 0; i < mid; i++)
        {
            Minisat::vec<Minisat::Lit> literals;
            for (int j = 0; j < vertex; j++)
            { // n is changing first
                literals.push(matrix[j][i]);
            }
            solver.addClause(literals);
            literals.clear();
        }

        // 2: No one vertex can appear twice in a vertex cover: ∀m ∈ [1,n],∀p,q ∈ [1,k] with p < q,a clause (¬Xm,p ∨ ¬Xm,q)
        solver_add_clause(matrix, solver, mid, vertex, false);

        // 3: No more than one vertex appears in the mth position of the vertex cover : ∀m ∈ [1,k],∀p,q ∈ [1,n] with p < q, a clause (¬Xp,m ∨ ¬Xq,m)
        solver_add_clause(matrix, solver, vertex, mid, true);

        // 4: Every edge is incident to at least one vertex in the vertex cover.
        for (auto &s : edges)
        {
            Minisat::vec<Minisat::Lit> literals;
            for (int i = 0; i < mid; i++)
            {
                literals.push(matrix[s.first][i]);
                literals.push(matrix[s.second][i]);
            }
            solver.addClause(literals);
            literals.clear();
        }

        bool sat = solver.solve();

        if (sat)
        {
            result.clear();
            result = output_vector(matrix, solver, vertex, mid);
            high = mid - 1;
        }
        else
            low = mid + 1;
    }
    return result;
    // Print the result
    // print_result(result);
}

std::vector<int> cnf3SatVc(int &vertex, std::vector<std::pair<int, int>> &edges)
{
    vector<int> vertex_cover;
    vertex_cover.clear();
    int n = vertex;
    bool res;

    for(int k=1; k<=vertex; k++) 
    {
        res=false; //initially UNSAT
        unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
        if(!res)
        {
            vector<vector<Minisat::Lit>> matrix;
            matrix.resize(vertex, vector<Minisat::Lit> (k));
            // Create n x k literals matrix and initialize
            for(int i=0; i<n; i++)
            {
                for(int j=0; j<k; j++)
                {
                    matrix[i][j] = Minisat::mkLit(solver->newVar());
                }
            }

            //First clause
            Minisat::vec<Minisat::Lit> clause1;
            for(int i=0; i<k; i++)
            {
                for(int j=0; j<vertex; j++)
                {
                    clause1.push(matrix[j][i]);
                }
                // solver->addClause(clause1);

                // Reduction to 3-CNF
                //introduce new literals
                vector<Minisat::Lit> u(clause1.size()-3);
                int u_ptr=0;
                for(int i=0; i<(int)u.size(); i++)
                {
                    u[i] = Minisat::mkLit(solver->newVar());
                }
                solver->addClause(clause1[0], clause1[1], u[u_ptr]);
                for(int l=2; l<clause1.size()-2; l++)
                {
                    solver->addClause(clause1[l], ~u[u_ptr], u[u_ptr+1]);
                    u_ptr++;
                }
                solver->addClause(clause1[clause1.size()-2], clause1[clause1.size()-1],  ~u[u_ptr]);
                //reduction ends here
                clause1.clear();
            }

            //Second clause - No one vertex can appear twice in a vertex cover
            for(int m=0; m<vertex; m++)
            {
                for(int p=0; p<k-1; p++)
                {
                    for(int q=p+1; q<k; q++)
                    {
                        solver->addClause(~matrix[m][p], ~matrix[m][q]);
                    }
                }
            }
            //clause2.clear();

            // Third clause - No more than one vertex appears in the mth position of the vertex cover
            for(int m=0; m<k; m++)
            {
                for(int p=0; p<n-1; p++)
                {
                    for(int q=p+1; q<n; q++)
                    {
                        solver->addClause(~matrix[p][m], ~matrix[q][m]);
                    }
                }
            }

            // Fourth clause - Every edge is incident to at least one vertex in the vertex cover
            // Reduced to 3-CNF
            Minisat::vec<Minisat::Lit> clause4;
            for(int i=0; i<(int)edges.size(); i++)
            {
                int v1 = edges[i].first;
                int v2 = edges[i].second;

                for(int j=0; j<k; j++)
                {
                    clause4.push(matrix[v1][j]);
                    clause4.push(matrix[v2][j]);
                }
                // solver->addClause(clause4);
                
                // Reduction to 3-CNF
                if(clause4.size() > 3)
                {
                    vector<Minisat::Lit> nlit(clause4.size()-3);
                    for(int i=0; i<(int)nlit.size(); i++)
                    {
                        nlit[i] = Minisat::mkLit(solver->newVar());
                    }
                    int ptr=0;
                    solver->addClause(clause4[0], clause4[1], nlit[ptr]);
                    for(int l=2; l<clause4.size()-2; l++)
                    {
                        solver->addClause(clause4[l], ~nlit[ptr], nlit[ptr+1]);
                        ptr++;
                    }
                    solver->addClause(clause4[clause4.size()-2], clause4[clause4.size()-1],  ~nlit[ptr]);
                    // clause4.clear();
                }
                else
                {
                    solver->addClause(clause4);
                    // clause4.clear();
                }
                //reduction ends here
                clause4.clear();
            }

            //cout<<"Start of solve(): "<<endl;
            res = solver->solve();
            //cout <<"The result is: "<<res<<endl;
            if(res)
            {
                for(int i=0; i<vertex; i++)
                {
                    for(int j=0; j<k; j++)
                    {
                        if(solver->modelValue(matrix[i][j]) == Minisat::l_True)
                        {
                            vertex_cover.push_back(i);
                        }
                    }
                }
                sort(vertex_cover.begin(), vertex_cover.end());
                return vertex_cover;
            }
        }
        //reset solver for next loop
        solver.reset(new Minisat::Solver());
    }
    // cerr<<"Error: F is unsat for all sizes of k"<<endl;
    return vertex_cover; //will be empty if code reaches this point
}