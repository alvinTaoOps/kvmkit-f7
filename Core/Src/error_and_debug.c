/*
 * error_and_debug.c
 *
 *  Created on: Mar 23, 2021
 *      Author: Bowen Shaner
 */
#include <error_and_debug.h>
#include "stdio.h"


/**
  * @brief  This function is executed in case of error occurrence and includes file and line context
  */
void Error_Handler_Context(char * file, int line)
{
	printf("Fatal error at %s, %d\r\n", file, line);
	Error_Handler();
}

void Error_Handler_Context_Description(char * file, int line, char * description)
{
	printf("Fatal error at %s, %d: %s\r\n", file, line, description);
	Error_Handler();
}
