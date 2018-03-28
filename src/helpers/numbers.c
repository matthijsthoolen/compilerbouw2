#include "stdio.h"
#include "stdlib.h"
#include "limits.h"
#include "string.h"
#include "dbug.h"
#include "float.h"

#include "numbers.h"

/**
 * @param  val char value to convert
 * @return     Returns a float if valid float, else returns NULL
 */
float *convertToFloat(char * val) {
    DBUG_ENTER("convertToFloat");

    double value = 0;
    sscanf(val, "%lf", &value);

    if (value >= -FLT_MAX && value <= FLT_MAX) {
        float* floatNumber = malloc(sizeof(float));
        *floatNumber = (float)value;
        DBUG_RETURN(floatNumber);
    }

    DBUG_RETURN(NULL);
}

/**
 * allowed range [−2147483647, +2147483647].
 *
 * @param  val char value to convert
 * @return     Returns an int if valid float, else returns NULL
 */
int *convertToInt(char * val) {
    DBUG_ENTER("convertToInt");

    long convVal = atol(val);
    if (convVal >= INT_MIN && convVal <= INT_MAX) {
        char* origVal = malloc(12);
        sprintf(origVal, "%ld", convVal);

        if (strcmp(val, origVal) == 0) {
            free(origVal);
            int* validInt = malloc(sizeof(int));
            *validInt = (int)convVal;
            DBUG_RETURN(validInt);
        }

        free(origVal);
    }

    DBUG_RETURN(NULL);
}
