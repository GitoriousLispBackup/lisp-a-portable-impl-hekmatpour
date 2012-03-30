#include "kernel.h"
#include <setjmp.h>
#include <stdlib.h>

#define CATpush()\
	if(++cattop<CATSTKSIZE){\
		catstk[cattop].evaltop=evaltop;\
		catstk[cattop].celltop=celltop;\
		catstk[cattop].vartop=vartop;\
		catstk[cattop].argtop=argtop;\
		catstk[cattop]._argtop=_argtop;\
	}else faterr(err_catstk)
#define CATpop() --cattop

struct catframe{/*catch frame*/
	jmp_buf jmp;/*for setjmp and longjmp*/
	int evaltop;/*evaltop at the time of setjmp*/
	int celltop;/*celltop at the time of setjmp*/
	int vartop;/*vartop at the time of setjmp*/
	int argtop;/*argtop at the time of setjmp*/
	int _argtop;/*argtop of the last vlam*/
};
struct catframe catstk[CATSTKSIZE];/*the catch stack*/
int cattop=-1;/*top of jump stack*/
int errtrap=0;/*no error capture when zero*/
int errshow=1;/*errors are reported when non-zero*/
int errocc=0;/*set when an error occurs*/
int level=0;/*kernel level*/

void cleanup()/*clean up stacks after a throw*/;
void errlevel()/*error level's read-eval-print*/;
kerncell klcatch(kerncell expr,kerncell tag,word more)/*catch throws during evaluation*/;
char* faterr(char* message)/*fatal error handling*/;

kerncell Ucatch()/*(catch 'expr ['tag])*/
{
	kerncell list=argstk[argtop];
	word dontcare;
	dontcare.i=0;
	if(list==NIL || list->CELLcdr->CELLcdr!=NIL)
		error(catchsym,err_args,0);
	return(klcatch(list->CELLcar,eval(list->CELLcdr->CELLcar),dontcare));
}

kerncell klcatch(kerncell expr,kerncell tag,word more)/*catch throws during evaluation*/
{
	CATpush();
	/*get ready for throws:*/
	if(!CONVcell(setjmp(catstk[cattop].jmp)))
		if(more.i){
			kerncell(*cfun)()=expr;
			expr=(*cfun)(more);/*cfun may have throws*/
		}
		else
			expr=eval(expr);/*expr may contain throws*/
	else if(tag==NIL && !ISinternal(CONVsym(catres->CELLcar))
		|| catres->CELLcar==NIL && !ISinternal(tag) && !ISlist(tag)
		|| catres->CELLcar==tag
		|| ISlist(tag) && memq(catres->CELLcar,tag)){
			cleanup();
			CATpop();/*catch the throw and*/
			return(catres->CELLcdr);/*return its result*/
	}
	else if(cattop<1)/*one catch is reserved for top level*/
		error(catchsym,"no catch for this tag",catres->CELLcar);
	else{
		cleanup();
		longjmp(catstk[CATpop()].jmp,catres);/*try another catch*/
	}
	CATpop();/*there was no throw, so ignore the catch*/
	return(expr);
}

void cleanup()/*clean up stacks after a throw*/
{
	int vtop=catstk[cattop].vartop;
	while(vtop<vartop)/*restore non-global vars*/
		VARpop();
	evaltop=catstk[cattop].evaltop;/*restore eval stack*/
	celltop=catstk[cattop].celltop;/*restore cell stack*/
	argtop=catstk[cattop].argtop;/*restore arg stack*/
	_argtop=catstk[cattop]._argtop;/*restore _argtop*/
}

kerncell Vthrow()/*(throw 'obj ['tag])*/
{
	CHECKvargs(throwsym,1,2);
	return(klthrow(ARGnum1,(argtop-ARGidx1 == 1? NIL : ARGnum2)));
}

kerncell klthrow(kerncell expr,kerncell tag)
{
	catres->CELLcar=tag;
	catres->CELLcdr=expr;
	longjmp(catstk[cattop].jmp,catres);
}

kerncell Ucaperr()/*(caperr 'exr [hide])*/
{
	kerncell list=argstk[argtop];
	if(list==NIL || list->CELLcdr->CELLcdr!=NIL)
		error(caperrsym,err_args,0);
	return(caperr(list->CELLcar,list->CELLcdr->CELLcar,NULL));
}

kerncell caperr(kerncell expr,kerncell hide,word more)/*captures errors during evalution of expr*/
{
	int savetrap=errtrap;
	int saveshow=errshow;
	int saveocc=errocc;
	kerncell res;

	errtrap=1;
	errshow=eval(hide)==NIL;
	errocc=0;
	res=klcatch(expr,_errtagsym,more);
	res=(errocc?NIL : mkcell(res,NIL));
	errtrap=savetrap;
	errshow=saveshow;
	errocc=saveocc;
	return(res);
}

kerncell Verror()/*(error 'source 'message ['extra])*/
{
	kerncell arg2;
	CHECKvargs(errorsym,2,3);
	arg2=ARGnum2;
	error(ARGnum1,GETstr(errorsym,arg2),(argtop-ARGidx1==2 ? 0 : ARGnum3));
	return(TTT);
}

kerncell error(kerncell source,char* message,kerncell extra)/*error handling routine*/
{
	errocc=1;
	if(errshow){
		bufprint(PRINT,_errchan,"ERROR, ");
		if(source!=0){
			PRINTchan(source,errchan);
			bufprint(PRINT,_errchan,": ");
		}
		bufprint(PRINT,_errchan,"%s",message);
		if(extra!=0){
			bufprint(PRINT,_errchan,": ");
			PRINTchan(extra,errchan);
		}
		bufprint(PRINT,_errchan,"\n");
	}
	if(errtrap)
		klthrow(NIL,_errtagsym);
	else
		EVALpush(CONVcell(errorsym));
	errlevel();
	return NIL;
}

void errlevel()/*error level's read-eval-print*/
{
	kerncell obj;
	word dontcare;
	dontcare.i=0;
	++level;
	for(;;){
		bufprint(PRINT,_outchan,"=%1d=> ",level);
		obj=klcatch(read_and_eval,_errtagsym,dontcare);
		if(obj==CONVcell(eofsym)){
			--level;
			klthrow(NIL,_errtagsym);
		}
		PRINTout(obj);
		bufprint(PRINT,_outchan,"\n");
	}
}

char* faterr(char* message)/*fatal error handling*/
{
	printf("FATAL ERROR: %s\n",message);
	exit(1);
	return NULL;
}

void topexec()/*kernel executive*/
{
	word dontcare;
	dontcare.i=0;
	bufprint(PRINT,_outchan,"KERNEL V1, AUG87\n");
	for(;;){
		errtrap=errocc=0;
		errshow=1;
		level=0;
		klcatch(top_lev_call,top_lev_tags,dontcare);
		bufprint(PRINT,_outchan,"\n[KERNEL top level]\n");
	}
}

kerncell Ltoplevel()/*(toplevel)*/
{
	kerncell obj;
	CHECKlargs(toplevelsym,0);
	for(;;){
		bufprint(PRINT,_outchan,"=> ");
		if((obj=eval(read_and_eval)) == CONVcell(eofsym)){
			bufprint(PRINT,_outchan,"\n");
			exit(0);
		}
		PRINTout(obj);
		bufprint(PRINT,_outchan,"\n");
	}
}

kerncell Lreset()
{
	CHECKlargs(resetsym,0);
	klthrow(NIL,_toptagsym);
	return NULL;
}

kerncell Vexit()
{
	kerncell arg;
	int idx1=ARGidx1;
	int exitcode=0;

	CHECKvargs2(exitsym,1);
	if(argtop==idx1+1){
		arg=ARGnum1;
		exitcode=GETint(exitsym,arg);
	}
	exit(exitcode);
}