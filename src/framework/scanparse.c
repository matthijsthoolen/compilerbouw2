/* ---------------------------------------------------------------------------
 *
 * SAC Compiler Construction Framework
 *
 * ---------------------------------------------------------------------------
 *
 * SAC COPYRIGHT NOTICE, LICENSE, AND DISCLAIMER
 *
 * (c) Copyright 1994 - 2011 by
 *
 *   SAC Development Team
 *   SAC Research Foundation
 *
 *   http://www.sac-home.org
 *   email:info@sac-home.org
 *
 *   All rights reserved
 *
 * ---------------------------------------------------------------------------
 *
 * The SAC compiler construction framework, all accompanying
 * software and documentation (in the following named this software)
 * is developed by the SAC Development Team (in the following named
 * the developer) which reserves all rights on this software.
 *
 * Permission to use this software is hereby granted free of charge
 * exclusively for the duration and purpose of the course
 *   "Compilers and Operating Systems"
 * of the MSc programme Grid Computing at the University of Amsterdam.
 * Redistribution of the software or any parts thereof as well as any
 * alteration  of the software or any parts thereof other than those
 * required to use the compiler construction framework for the purpose
 * of the above mentioned course are not permitted.
 *
 * The developer disclaims all warranties with regard to this software,
 * including all implied warranties of merchantability and fitness.  In no
 * event shall the developer be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data, or profits, whether in an action of contract, negligence, or
 * other tortuous action, arising out of or in connection with the use or
 * performance of this software. The entire risk as to the quality and
 * performance of this software is with you. Should this software prove
 * defective, you assume the cost of all servicing, repair, or correction.
 *
 * ---------------------------------------------------------------------------
 */

 #include <errno.h>
 #include <stdio.h>
 #include <string.h>
 #include <sys/wait.h>
 #include <unistd.h>
 #include "scanparse.h"
 #include "dbug.h"
 #include "globals.h"
 #include "ctinfo.h"
 #include "str.h"
 #include "memory.h"


 /*
  * file handle for parsing
  */
 FILE *yyin;

 /*
  * external parser function from fun.y
  */
 extern node *YYparseTree();

 node *SPdoScanParse( node *syntax_tree)
 {
   node *result = NULL;

   DBUG_ENTER("SPdoScanParse");

   DBUG_ASSERT( syntax_tree == NULL,
                "SPdoScanParse() called with existing syntax tree.");

   if (global.cpp_pid == 0) {
       yyin = fopen( global.infile, "r");
       if (yyin == NULL) {
         CTIabort( "Cannot open file '%s'.", global.infile);
       }
   }

   result = YYparseTree();

   if (global.cpp_pid != 0) {
       int status;
       if (waitpid(global.cpp_pid, &status, 0) == -1) {
           CTIabort("CPP waitpid failed: %s", strerror(errno));
       }
       if (status != 0) {
           CTIerror("Preprocessor terminated with a non-zero exit code: %d",
                    status);
       }
   }

   DBUG_RETURN( result);
 }


 node *SPdoRunPreProcessor( node *syntax_tree)
 {
   DBUG_ENTER("SPdoRunPreProcessor");

   int pipes[2];
   if (pipe(pipes) == -1) {
       CTIabort("Failed to create CPP pipe: %s", strerror(errno));
   }

   int cpp_pid = fork();
   if (cpp_pid == -1) {
       CTIabort("Failed to fork CPP: %s", strerror(errno));
   } else if (cpp_pid == 0) {
       close(pipes[0]);
       if (dup2(pipes[1], STDOUT_FILENO) == -1) {
           CTIabort("CPP dup2 failed: %s", strerror(errno));
       }
       close(pipes[1]);
       execl("/usr/bin/env", "/usr/bin/env", "gcc", "-x", "c", "-E", global.infile, NULL);
       CTIabort("CPP execl(/usr/bin/env gcc ...) failed: %s", strerror(errno));
   } else {
       close(pipes[1]);
       global.cpp_pid = cpp_pid;
       yyin = fdopen(pipes[0], "r");
       if (yyin == NULL) {
           CTIabort("CPP fdopen failed: %s", strerror(errno));
       }
   }

   DBUG_RETURN( syntax_tree);
 }
