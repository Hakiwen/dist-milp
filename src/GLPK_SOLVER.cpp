//
// Created by tkhamvilai on 4/12/19.
//

#include "GLPK_SOLVER.hpp"

GLPK_SOLVER::GLPK_SOLVER(const char* LP_file, const char* Sol_file)
{
    this->LP_file = LP_file;
    this->Sol_file = Sol_file;
}

int GLPK_SOLVER::solve()
{
    glp_prob *model = glp_create_prob();
    glp_read_lp(model, NULL, this->LP_file);

    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
#ifndef __x86_64__
    parm.msg_lev = GLP_MSG_OFF;
#endif
    int status_relax = glp_intopt(model, &parm);
    int status_MIP = glp_mip_status(model);

    std::cout << "relax: " << status_relax << std::endl;
    std::cout << "MIP"  << status_MIP << std::endl;

    glp_print_mip(model, Sol_file);
    glp_write_mip(model, "sol_glpk.txt");

    return (status_relax == 0 && status_MIP == GLP_OPT);
}

int GLPK_SOLVER::solve(NOC_GLPK *NoC_GLPK)
{
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    int status_relax = glp_intopt(NoC_GLPK->model, &parm);
    int status_MIP = glp_mip_status(NoC_GLPK->model);

    std::cout << "relax: " << status_relax << std::endl;
    std::cout << "MIP"  << status_MIP << std::endl;

    glp_print_mip(NoC_GLPK->model, Sol_file);
    glp_write_mip(NoC_GLPK->model, "sol_glpk.txt");

    return (status_relax == 0 && status_MIP == GLP_OPT);
}
