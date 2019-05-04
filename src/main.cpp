#include <csignal>
#include <fstream>

#include "MY_MACROS.hpp"

#include "NOC.hpp"
#include "NOC_FAULT.hpp"
#include "NOC_MPI.hpp"

#include "NOC_GLPK.hpp"
#include "GLPK_SOLVER.hpp"

#include "ENGINE.hpp"
#include "APP_INCLUDE.hpp"

using namespace std;

void sighandler(int signal)
{
    APP_LED_OFF();
    APP_PWM_OFF();
    exit(1);
}

int main (int argc, char* argv[]) // TODO try...catch... for checking if all arguments to all functions are valid
{
    /** User-Initialized Parameters **/

    int N_Row_CRs = 4, N_Col_CRs = 4;
    int N_apps = 7;

    int N_Row_apps[N_apps], N_Col_apps[N_apps], app_color[N_apps];
    N_Row_apps[0] = 2;    N_Col_apps[0] = 1;
    N_Row_apps[1] = 2;    N_Col_apps[1] = 1;
    N_Row_apps[2] = 2;    N_Col_apps[2] = 1;
    N_Row_apps[3] = 1;    N_Col_apps[3] = 1;
    N_Row_apps[4] = 1;    N_Col_apps[4] = 1;
    N_Row_apps[5] = 1;    N_Col_apps[5] = 1;
    N_Row_apps[6] = 1;    N_Col_apps[6] = 2;

    void (*app_ptr[N_apps + 1])(NOC*, ENGINE*, int);
    app_ptr[0] = &APP_LED;    app_color[0] = LED_WHITE;   // IDLE
    app_ptr[1] = &APP_PID;    app_color[0] = LED_RED;     // 1st priority app
    app_ptr[2] = &APP_PID;    app_color[1] = LED_GREEN;   // and so on...
    app_ptr[3] = &APP_PID;    app_color[2] = LED_BLUE;
    app_ptr[4] = &APP_LED;    app_color[3] = LED_YELLOW;
    app_ptr[5] = &APP_LED;    app_color[4] = LED_YELLOW;
    app_ptr[6] = &APP_LED;    app_color[5] = LED_YELLOW;
    app_ptr[7] = &APP_LED;    app_color[6] = LED_MAGENTA;

    int allocator_app_ind = 4, allocator_app_num = 3;

    const char* LP_file = "NoC.lp"; // problem file name
    const char* Sol_file = "sol.txt"; // solution file name

    /** End of User Initialization **/

    /*
     * Initialization
     */

    NOC NoC = NOC(N_Row_CRs, N_Col_CRs, N_apps, N_Row_apps, N_Col_apps, app_color, allocator_app_ind, allocator_app_num); // NoC Object
    NoC.CreateTopology("square");

    NOC_MPI NoC_MPI = NOC_MPI(); // NoC MPI Object
    NOC_FAULT NoC_Fault = NOC_FAULT(&NoC, NoC_MPI.world_rank); // NoC Fault Detection Object

    NOC_GLPK NoC_GLPK = NOC_GLPK(); // NoC to GLPK Object
    GLPK_SOLVER prob_GLPK = GLPK_SOLVER(LP_file, Sol_file); // Solver Object

    ENGINE Engine = ENGINE(NoC.N_CRs); // Engine controller being the 1st priority app

#ifndef __x86_64__
    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGINT, &sighandler);
    wiringPiSetup();
#endif

    /** End of Initialization **/

    /*
     * Main Loop
     */
    if (NoC_MPI.world_rank == 1)
    {
        NoC_GLPK.write_LP(&NoC, LP_file);
        prob_GLPK.solve(&NoC_GLPK);
        NoC_GLPK.read_Sol(&NoC, Sol_file);
        NoC.Update_State();
        NoC.Disp();
    }
    NoC_MPI.run(&NoC, &Engine);
    delay(100);

    int step = 0;
    while (true)
//    while (step < 5)
    {
        cout << "Step: " << step << ", ";
        if (NoC_MPI.world_rank == 0)
        {
            Engine.run(NoC.N_CRs);
        }
        else // computer resource nodes
        {
            NoC_Fault.Fault_Detection(&NoC, NoC_MPI.world_rank); // read switch
            cout << "My Rank: " << NoC_MPI.world_rank;
//            cout << ", My Fault: " << NoC.fault_internal_status;
            cout << ", My Node: " << NoC.node_to_run;
//            cout << ", My Sensor: " << Engine.sensor_data;
//            cout << ", My App: ";
            cout << endl;
            NoC.app_to_run = NoC.get_app_from_node(NoC.node_to_run);
            if(NoC.app_to_run == -1) // dead
            {
                APP_LED_OFF();
            }
            else
            {
//                app_ptr[NoC.app_to_run](&NoC, &Engine, NoC.app_color[NoC.app_to_run]);

                // Allocators
                if(NoC.app_to_run >= NoC.allocator_app_ind && NoC.app_to_run < NoC.allocator_app_ind + NoC.allocator_app_num)
                {
                    if (NoC_Fault.Fault_Gathering(&NoC)) // get fault data from others
                    {
                        NoC_GLPK.write_LP(&NoC, LP_file);
                        if(prob_GLPK.solve(&NoC_GLPK))
                        {
                            NoC_GLPK.read_Sol(&NoC, Sol_file);
                            NoC.solver_status = 1;
                        }
                        else
                        {
                            cout << "Infeasible Solution" << endl;
                            NoC.solver_status = 0;
                        }
                        NoC.Update_State();
                    }
                    NoC.Disp();
                }
                else
                {
                    for (int i = 0; i < NoC.N_CRs; i++)
                    {
                        NoC.nodes_on_CRs[i] = 0;
                    }
                }
            }
        }

        NoC_MPI.run(&NoC, &Engine);
        delay(1000);
        step++;
    }

    while (true){delay(1);}; // does nothing, but smiling at you :)

    return 0;

}  // END main