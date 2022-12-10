/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _CALC_H_RPCGEN
#define _CALC_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


#define CALC_PROG 0x21111111
#define CALC_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define CONVERT_EXPRESSION 1
extern  char * convert_expression_1(char *, CLIENT *);
extern  char * convert_expression_1_svc(char *, struct svc_req *);
#define CALCULATE_EXPRESSION 2
extern  long * calculate_expression_1(char *, CLIENT *);
extern  long * calculate_expression_1_svc(char *, struct svc_req *);
extern int calc_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define CONVERT_EXPRESSION 1
extern  char * convert_expression_1();
extern  char * convert_expression_1_svc();
#define CALCULATE_EXPRESSION 2
extern  long * calculate_expression_1();
extern  long * calculate_expression_1_svc();
extern int calc_prog_1_freeresult ();
#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_CALC_H_RPCGEN */
