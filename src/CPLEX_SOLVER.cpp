//
// Created by tkhamvilai on 4/12/19.
//

#include "CPLEX_SOLVER.hpp"

#ifdef CPLEX_AS_SOLVER
CPLEX_SOLVER::CPLEX_SOLVER(const char* LP_file, const char* Sol_file)
{
    this->LP_file = LP_file;
    this->Sol_file = Sol_file;
}

int CPLEX_SOLVER::solve()
{
    IloEnv env;
    int status;
    try
    {
        IloModel model(env);
        IloCplex cplex(model);
        IloObjective   obj(env);
        IloNumVarArray var(env);
        IloRangeArray  con(env);
        cplex.importModel(model, this->LP_file, obj, var, con);
        cplex.solve();
        status = cplex.getStatus();
        cplex.writeSolutions(this->Sol_file);
        cplex.clear();
    }
    catch (IloException& e)
    {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught" << std::endl;
    }
    env.end();

    return status;
}
#endif