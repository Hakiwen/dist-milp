
/* wrtnode.c - Using the node, branch and solve callbacks
                for optimizing a MIP problem.  This example
                is a modification of Example mipex2.c   */

/* To run this example, command line arguments are required.
   i.e.,   wrtnode   filename [startnode] [endnode] [prmfile]
   where 
       filename is the name of the file, with .mps, .lp, or .sav extension
       startnode and endnode are optional node numbers indicating the range
       of nodes for which the program will write node files.
       prmfile is an optional permissions file to specify non default 
       parameter settings without recompiling.
   Example:
       wrtnode  mexample.mps 100 200
 */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with the following single include. */

#include <ilcplex/cplex.h>

/* Bring in the declarations for the string and character functions,
   malloc and floor */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Include declarations for functions in this program */

#ifndef  CPX_PROTOTYPE_MIN

static int CPXPUBLIC 
   usersolve       (CPXCENVptr env, void *cbdata, int wherefrom,
                    void *cbhandle, int *useraction_p);

static void
   free_and_null (char **ptr),
   usage         (char *progname);

#else

static int CPXPUBLIC 
   usersolve       ();

static void
   free_and_null (),
   usage         ();

#endif


struct noderange {
  int startnode;
  int endnode;
};
typedef struct noderange NODERANGE;

#ifndef  CPX_PROTOTYPE_MIN
int
main (int argc, char *argv[])
#else
int
main (argc, argv)
int   argc;
char  *argv[];
#endif
{
/* Declare and allocate space for the variables and arrays where we will
   store the optimization results including the status, objective value,
   and variable values. */

int      solstat;
double   objval;
double   *x = NULL;

CPXENVptr     env = NULL;
CPXLPptr      lp = NULL;
int           status = 0;
int           j;
int           cur_numcols;
char          *prmfile = NULL;
NODERANGE     nodeswritten;

   nodeswritten.startnode = -1;
   nodeswritten.endnode   = 2100000000;

   /* Check the command line arguments */

   if ( argc >= 2 ) {
      if (argc == 3) {
 	 nodeswritten.startnode = atoi(argv[2]);
      }
      else if (argc >= 4) {
 	 nodeswritten.startnode = atoi(argv[2]);
 	 nodeswritten.endnode   = atoi(argv[3]);
         if (argc == 5) {
	    prmfile = argv[4];
	 }
         else if (argc > 5) {
	    usage (argv[0]);
	    goto TERMINATE;
         }
      }
      if ( nodeswritten.startnode > nodeswritten.endnode) {
	fprintf (stderr, "Error in write node range, startnode = %d,",
		 nodeswritten.startnode, " endnode = %d\n", 
		 nodeswritten.endnode);
      }
   }
   else {
     usage (argv[0]);
     goto TERMINATE;
   }

   /* Initialize the CPLEX environment */

   env = CPXopenCPLEX(&status);

   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXgeterrorstring will produce the text of
      the error message.  Note that CPXopenCPLEXdevelop produces no output,
      so the only way to see the cause of the error is to use
      CPXgeterrorstring.  For other CPLEX routines, the errors will
      be seen if the CPX_PARAM_SCRIND indicator is set to CPX_ON.  */

   if ( env == NULL ) {
     char  errmsg[1024];
     fprintf (stderr, "Could not open CPLEX environment.\n");
     CPXgeterrorstring (env, status, errmsg);
     fprintf (stderr, "%s", errmsg);
     goto TERMINATE;
   }

   /* Turn on output to the screen */

   status = CPXsetintparam (env, CPX_PARAM_SCRIND, CPX_ON);
   if ( status ) {
     fprintf (stderr, 
	      "Failure to turn on screen indicator, error %d.\n", status);
     goto TERMINATE;
   }

   status = CPXsetintparam (env, CPX_PARAM_MIPINTERVAL, 1);
   if ( status ) {
     fprintf (stderr, 
	      "Failure to set MIP interval, error %d.\n", status);
     goto TERMINATE;
   }

   if (prmfile != NULL) {
      status = CPXreadcopyparam(env, prmfile);
      if (status) {
	 fprintf (stderr, "Failed to read PRM file, error %d\n", status);
	 goto TERMINATE;
      }
   }

   /*  status = CPXsetintparam (env, CPX_PARAM_BRDIR, -1);
       if ( status ) {
       fprintf (stderr, 
       "Failure to set branching direction, error %d.\n", status);
       goto TERMINATE;
       }
       status = CPXsetintparam (env, CPX_PARAM_VARSEL, 3);
       if ( status ) {
       fprintf (stderr, 
       "Failure to set variable select parameter, error %d.\n", status);
       goto TERMINATE;
       }*/



   /* Create the problem, using the filename as the problem name */

   lp = CPXcreateprob (env, &status, argv[1]);

   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of
      failure, an error message will have been written to the error
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPX_PARAM_SCRIND causes the error message to
      appear on stdout.  Note that most CPLEX routines return
      an error code to indicate the reason for failure.   */

   if ( lp == NULL ) {
     fprintf (stderr, "Failed to create LP.\n");
     goto TERMINATE;
   }

   /* Now read the file, and copy the data into the created lp */

   status = CPXreadcopyprob (env, lp, argv[1], NULL);
   if ( status ) {
     fprintf (stderr, "Failed to read and copy the problem data.\n");
     goto TERMINATE;
   }

   /* Set up to use MIP callbacks */

   status = CPXsetsolvecallbackfunc (env, usersolve, &nodeswritten);
   if ( status )  goto TERMINATE;

   printf ("Node files written starting with node %d,", 
	   nodeswritten.startnode);
   printf (" ending with node %d\n", nodeswritten.endnode);

   /* Optimize the problem and obtain solution. */

   status = CPXmipopt (env, lp);

   if ( status ) {
      fprintf (stderr, "Failed to optimize MIP.\n");
      goto TERMINATE;
   }

   solstat = CPXgetstat (env, lp);
   printf ("Solution status %d.\n", solstat);

   status  = CPXgetmipobjval (env, lp, &objval);

   if ( status ) {
      fprintf (stderr,"Failed to obtain objective value.\n");
      goto TERMINATE;
   }

   printf ("Objective value %.10g\n", objval);

   /* The size of the problem should be obtained by asking CPLEX what
      the actual size is, rather than using what was passed to CPXloadlp.
      cur_numcols stores the current number of columns. */

   cur_numcols = CPXgetnumcols (env, lp);

   /* Allocate space for solution */

   x = (double *) malloc (cur_numcols*sizeof(double));

   if ( x == NULL ) {
      fprintf (stderr, "No memory for solution values.\n");
      goto TERMINATE;
   }

   status = CPXgetmipx (env, lp, x, 0, cur_numcols-1);
   if ( status ) {
      fprintf (stderr, "Failed to obtain solution.\n");
      goto TERMINATE;
   }

   /* Write out the solution */

   for (j = 0; j < cur_numcols; j++) {
      printf ( "Column %d:  Value = %17.10g\n", j, x[j]);
   }

   
TERMINATE:

   /* Free up the solution */

   free_and_null ((char **) &x);

   /* Free up the problem as allocated by CPXloadlpwnames,
      if necessary */

   if ( lp != NULL ) {
      status = CPXfreeprob (env, &lp);
      if ( status ) {
         fprintf (stderr, "CPXfreeprob failed, error code %d.\n",
                  status);
      }
   }

   /* Free up the CPLEX environment, if necessary */

   if ( env != NULL ) {
      status = CPXcloseCPLEX (&env);

      /* Note that CPXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPX_PARAM_SCRIND indicator is set to CPX_ON. */

      if ( status ) {
      char  errmsg[1024];
         fprintf (stderr, "Could not close CPLEX environment.\n");
         CPXgeterrorstring (env, status, errmsg);
         fprintf (stderr, "%s", errmsg);
      }
   }
     
   return (status);

}  /* END main */



/* This simple routine frees up the pointer *ptr, and sets *ptr to NULL */

#ifndef  CPX_PROTOTYPE_MIN
static void
free_and_null (char **ptr)
#else
static void
free_and_null (ptr)
char  **ptr;
#endif
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */ 


#ifndef  CPX_PROTOTYPE_MIN
static void
usage (char *progname)
#else
static void
usage (progname)
char *progname;
#endif
{
   fprintf (stderr,"Usage: %s filename [startnode] [endnode] [prmfile]\n", 
            progname);
   fprintf (stderr,"   where filename is a file with extension \n");
   fprintf (stderr,"      MPS, SAV, or LP (lower case is allowed)\n");
   fprintf (stderr,"      startnode and endnode optionally specify a \n");
   fprintf (stderr,"      range of nodes in which we write the node LPs. \n");
   fprintf (stderr,"      prmfile is an optional parameter file; only \n");
   fprintf (stderr,"      specify if startnode and endnode are also set.\n");
   fprintf (stderr,"  This program uses the CPLEX MIP optimizer.\n");
   fprintf (stderr," Exiting...\n");
} /* END usage */


#ifndef  CPX_PROTOTYPE_MIN
static int CPXPUBLIC 
usersolve (CPXCENVptr env, void *cbdata, int wherefrom,
           void *cbhandle,int *useraction_p)
#else
static int CPXPUBLIC 
usersolve (env, cbdata, wherefrom, cbhandle, useraction_p)
CPXCENVptr env;
void       *cbdata;
int        wherefrom;
void       *cbhandle;
int        *useraction_p;
#endif
{
int       status = 0;
int       nodecount;
static    int count = 0;
CPXLPptr  nodelp;
NODERANGE *nodeswritten;

   *useraction_p = CPX_CALLBACK_DEFAULT;
   nodeswritten = (NODERANGE *) cbhandle;

   /* Find out what node is being processed */

   status = CPXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODE_COUNT, &nodecount);
   if ( status )  goto TERMINATE;
   
   if ( nodecount >= nodeswritten->startnode && 
        nodecount <= nodeswritten->endnode ) {
   char filename[128];

      /* Get pointer to LP subproblem, then write a SAV file. */

      status = CPXgetcallbacknodelp (env, cbdata, wherefrom, &nodelp);
      if ( status )  goto TERMINATE;

      sprintf (filename, "nodelp%d_%d", count, nodecount);
      strcat (filename, ".sav");
 
      status = CPXwriteprob (env, nodelp, filename, NULL);
      if ( status )  goto TERMINATE;

      if ( nodecount == nodeswritten->endnode )  status = 1;      
      count++;
   }


TERMINATE:
   return (status);

} /* END usersolve */


