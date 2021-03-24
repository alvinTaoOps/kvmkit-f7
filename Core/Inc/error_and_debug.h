/*
 * error_and_debug.h
 *
 *  Created on: Mar 23, 2021
 *      Author: Bowen Shaner
 */

#ifndef INC_ERROR_AND_DEBUG_H_
#define INC_ERROR_AND_DEBUG_H_

void Error_Handler_Context(char * file, int line);
void Error_Handler_Context_Description(char * file, int line, char * description);

#endif /* INC_ERROR_AND_DEBUG_H_ */
