/*
**	mysql.h	- 
**
**
** Copyright (c) 1993-95  David J. Hughes
** Copyright (c) 1995  Hughes Technologies Pty Ltd
** Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
**
** This software is provided "as is" without any expressed or implied warranty.
**
** ID = "$Id:"
**
** 06/04/96 (JEC) Modified to handle multiple inclusions and
**                also changed function parameters to const where applicable
**
*/
#ifndef XMYSQL_H
#define XMYSQL_H

#if defined(__STDC__) || defined(__cplusplus)
#  define __ANSI_PROTO(x)	x
#else
#  define __ANSI_PROTO(x)	()
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef	char	** MYSQL_ROW;

typedef struct field_s {
	char	*name,
		*table;
	int	type,
		length,
		flags;
} MYSQL_FIELD;



typedef	struct	m_data_s {
	int	width;
	MYSQL_ROW	data;
	struct	m_data_s *next;
} m_data;

typedef struct m_fdata_s {
	MYSQL_FIELD	field;
	struct m_fdata_s *next;
} m_fdata;



typedef struct result_s {
        m_data 	*queryData,
                *cursor;
	m_fdata	*fieldData,
		*fieldCursor;
	int	numRows,
		numFields;
} MYSQL_RES;


#define	mysql_nuMYSQL_ROWs(res) res->numRows
#define	mysql_nuMYSQL_FIELDs(res) res->numFields


#define INT_TYPE	1
#define CHAR_TYPE	2
#define REAL_TYPE	3
#define IDENT_TYPE	4
#define NULL_TYPE	5

#define NOT_NULL_FLAG   1
#define PRI_KEY_FLAG    2

#define IS_PRI_KEY(n)	(n & PRI_KEY_FLAG)
#define IS_NOT_NULL(n)	(n & NOT_NULL_FLAG)


/*
** Pre-declarations for the API library functions
*/
#ifndef _MYSQL_SERVER_SOURCE
//	extern  const char 'mysql_error(mysql)'[];
	int 	mysql_connect __ANSI_PROTO((const char *));
	int 	mysql_selectDB __ANSI_PROTO((int, const char*));
	int 	mysql_query __ANSI_PROTO((int, const char*));
	int 	mysqlCreateDB __ANSI_PROTO((int, const char*));
	int 	mysqlDropDB __ANSI_PROTO((int, const char*));
	int 	mysqlShutdown __ANSI_PROTO((int));
	int 	mysqlReloadAcls __ANSI_PROTO((int));
	int 	mysqlGetProtoInfo();
	char 	*mysqlGetServerInfo();
	char 	*mysqlGetHostInfo();
	void	mysql_close __ANSI_PROTO((int));
	void 	mysql_data_seek __ANSI_PROTO((MYSQL_RES*, int));
	void 	mysql_fieldSeek __ANSI_PROTO((MYSQL_RES*, int));
	void 	mysql_free_result __ANSI_PROTO((MYSQL_RES*));
        MYSQL_ROW   mysql_fetch_row __ANSI_PROTO((MYSQL_RES*));
	MYSQL_FIELD	*mysql_fetch_field __ANSI_PROTO((MYSQL_RES *));
	MYSQL_RES * mysql_list_dbs __ANSI_PROTO((int));
	MYSQL_RES *mysql_list_tables __ANSI_PROTO((int));
	MYSQL_RES *mysql_list_fields __ANSI_PROTO((int, const char*));
	MYSQL_RES *mysql_store_result();
#endif

#ifdef __cplusplus
	}
#endif

#endif
