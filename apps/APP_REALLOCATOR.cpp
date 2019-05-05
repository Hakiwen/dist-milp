//
// Created by tkhamvilai on 5/4/19.
//

#include "APP_REALLOCATOR.hpp"

void APP_REALLOCATOR(NOC *NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE *Engine, int color)
{
    APP_LED(NoC, NoC_Fault, NoC_GLPK, prob_GLPK, Engine, color);

    if (NoC_Fault->Fault_Gathering(NoC)) // get fault data from others
    {
        NoC_GLPK->write_LP(NoC);
        if(prob_GLPK->solve(NoC_GLPK))
        {
            NoC_GLPK->read_Sol(NoC);
            NoC->solver_status = 1;
        }
        else
        {
            std::cout << "Infeasible Solution" << std::endl;
            NoC->solver_status = 0;
        }
        NoC->Update_State();
    }
    NoC->Disp();
}