/*
 * cmd_ladderlib.h
 *
 *  Created on: 25 feb 2025
 *      Author: egonzalez
 */

#ifndef CMD_LADDERLIB_H_
#define CMD_LADDERLIB_H_

void register_ladder_status(void);
void register_fs_ls(void);
void register_fs_rm(void);
void register_ladder_dump(void);
void register_ladder_load(void);
void register_ladder_start(void);
void register_ladder_stop(void);
void register_ftpserver(void);
void register_output_test(void);
void register_input_test(void);

#endif /* CMD_LADDERLIB_H_ */
