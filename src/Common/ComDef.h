/*
 * ComDef.h
 *
 *  Created on: Dec 2, 2012
 *      Author: Scorpion
 */

#ifndef COMDEF_H_
#define COMDEF_H_
#define WIN_F_SEQ "\\"
#define UNIX_F_SEQ "/"
#ifdef WIN32
#define F_SEQ WIN_F_SEQ
#else
#define F_SEQ UNIX_F_SEQ
#endif
#define SHOW_SQL_LOG 0
#endif /* COMDEF_H_ */
