#include "kernel.h"

void load(kerncell name,int verbose);

kerncell Uvoid()
{
	kerncell list=argstk[argtop];
	if(list->CELLcdr!=NIL)
		error(voidsym,err_args,0);
	return(list->CELLcar);
}

kerncell Uquote()
{
	kerncell list=argstk[argtop];
	if(list==NIL || list->CELLcdr!=NIL)
		error(quotesym,err_args,0);
	return(mkcell(quotesym,list));
}

kerncell Lkwote()
{
	CHECKlargs(kwotesym,1);
	return(mkcell(quotesym,mkcell(ARGnum1,NIL)));
}

kerncell Udef()
{
	kerncell list=argstk[argtop];
	kernsym sym=CONVsym(list->CELLcar);
	CHECKsym2(setplistsym,sym);
	sym->flag=FUNCTION;
	sym->bind=list->CELLcdr->CELLcar;
	return(CONVcell(sym));
}

kerncell Mfun()
{
	kerncell list=argstk[argtop];
	return(mkcell(defsym,
		(ISsym(list->CELLcdr->CELLcar)&&
		list->CELLcdr->CELLcar!=NIL
		?mkcell(list->CELLcar,mkcell(list->CELLcdr,NIL))
		:mkcell(list->CELLcar,mkcell(mkcell(lamsym,list->CELLcdr),NIL)))));
}

kerncell Larg()
{
	kerncell arg=ARGnum1;
	int argnum,idx;

	CHECKlargs(argsym,1);
	argnum=GETint(argsym,arg);
	if(_argtop<0)
		error(argsym,"outside a vlam scope",0);
	idx=CONVint(argstk[_argtop]);
	if(argnum<1 || argnum>_argtop-idx)
		error(argsym,"argument outside range",arg);
	return(argstk[idx+argnum-1]);
}

kerncell Mlet()
{
	kerncell list=argstk[argtop];
	kerncell pars=NIL;
	kerncell inits=NIL;
	kerncell vars,var;

	vars=list->CELLcar;
	if(!ISlist(vars) && vars!=NIL)
		error(letsym,"bad variable list",vars);
	while(ISlist(vars)){
		if(ISlist(var=vars->CELLcar)){
			if(!ISsym(var->CELLcar) || var->CELLcdr->CELLcdr!=NIL)
				error(letsym,"bad variable form",var);
			pars=mkcell(var->CELLcar,pars);
			inits=mkcell(var->CELLcdr->CELLcar,inits);
		}
		else if(ISsym(var)){
			pars=mkcell(vars,pars);
			inits=mkcell(NIL,inits);
		}
		else
			error(letsym,"bad variable form",var);
		vars=vars->CELLcdr;
	}
	return(mkcell(mkcell(lamsym,mkcell(dreverse(pars),list->CELLcdr)),
		dreverse(inits)));
}

kerncell Lset()
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(setsym,2);
	CHECKsym2(setsym,arg1);
	CONVsym(arg1)->flag=VARIABLE;
	return(CONVsym(arg1)->bind=arg2);
}

kerncell Usetq()
{
	kerncell list=argstk[argtop];
	kernsym sym;
	kerncell res;

	if(list->CELLcdr==NIL)
		error(setqsym,err_args,0);
	while(ISlist(list)){
		sym=CONVsym(list->CELLcar);
		CHECKsym2(setqsym,sym);
		if(!ISlist(list=list->CELLcdr))
			error(setqsym,err_args,0);
		sym->flag=VARIABLE;
		res=sym->bind=eval(list->CELLcar);
		list=list->CELLcdr;
	}
	return(res);
}

kerncell Uconst()
{
	kerncell list=argstk[argtop];
	kernsym sym;
	kerncell res;

	if(list->CELLcdr==NIL)
		error(constsym,err_args,0);
	while(ISlist(list)){
		sym=CONVsym(list->CELLcar);
		CHECKsym2(constsym,sym);
		if(!ISlist(list=list->CELLcdr))
			error(constsym,err_args,0);
		sym->flag=CONSTANT;
		res=sym->bind=eval(list->CELLcar);
		list=list->CELLcdr;
	}
	return(res);
}

kerncell Vss()
{
	int nframes,evalidx;
	int len;
	kerncell arg,prev=NULL;
	kerncell frame;
	CHECKvargs2(sssym,1);
	nframes=(argtop==ARGidx1 ? EVALSTKSIZE+1
		:(arg=ARGnum1, GETint(sssym,arg)));
	for(evalidx=evaltop-2;nframes>0 && evalidx>=0;--nframes,--evalidx){
		len=bufprint(PRINT,_outchan,"[%02d] ",evalidx);
		frame=evalstk[evalidx];
		if(!ISlist(frame))
			printaux(PRINT,frame,_outchan);
		else{
			len+=bufprint(PRINT,_outchan,"(");
			while(ISlist(frame)){
				if(frame->CELLcar==prev)
					len+=bufprint(PRINT,_outchan,"<**>");
				else if(len+printaux(LENGTH,frame->CELLcar,_outchan,
					MAXCOLS-len)>MAXCOLS){
						bufprint(PRINT,_outchan,"...");
						break;
				}
				else
					len+=printaux(PRINT,frame->CELLcar,_outchan);
				if(ISlist(frame=frame->CELLcdr))
					bufprint(PRINT,_outchan," ");
			}
			bufprint(PRINT,_outchan,")");
		}
		prev=evalstk[evalidx];
		TERPRIout();
	}
	return(TTT);
}

kerncell Vload()
{
	int verbose;
	kerncell arg1=ARGnum1;

	CHECKvargs(loadsym,1,2);
	verbose=(argtop-ARGidx1==2 && ARGnum2!=NIL);
	if(ISlist(arg1))
		while(ISlist(arg1)){
			load(arg1->CELLcar,verbose);;
			arg1=arg1->CELLcdr;
		}
	else
		load(arg1,verbose);
	return(TTT);
}

void load(kerncell name,int verbose)
{
	char* str=GETstr(loadsym,name);
	int len=strlen(str);
	FILE *file, *fopen();
	iochan chan,openchan();
	kerncell obj;

	if(len<3 || str[len-2]!= '.' || str[len-1]!='k')
		error(loadsym,"bad file name",name);
	if((file=fopen(str,"r"))==NULL)
		error(loadsym,"can't open file",name);
	chan=openchan(file,INCHAN);
	while((obj=readaux(chan,0))!=CONVcell(eofsym)){
		obj=eval(obj);
		if(verbose && obj!=NIL){
			PRINTout(obj);
			TERPRIout();
		}
	}
	closechan(chan);
}

#ifdef UNIX
#include <signal.h>

void subshell(char* str);

kerncell Ushell()
{
	kerncell list=argstk[argtop];
	char *str,*sbuf=strbuf;
	int len=0;

	while(ISlist(list)){
		str=GETstr(shellsym,list->CELLcar);
		if((len+=strlen(str)+1) > STRBUFSIZE)
			error(shellsym,"string buffer overflow",0);
		sprintf(sbuf,"%s ",str);
		sbuf=strbuf+len;
		list=list->CELLcdr;
	}
	sbuf[len]=0;
	return(mkinum(subshell(strbuf)));
}

void subshell(char* str)
{
	int (*save_intr)(),procid,status;
	int iwait;

	save_intr=signal(SIGINIT,SIG_IGN);
	if((procid=fork()) == 0){
		signal(SIGINIT,org_interrupt);
		execl("/bin/sh","sh","-c",str,0);
		exit(127);
	}
	while((iwait=wait(&status)) != procid && iwait!=-1)
		;
	signal(SIGINT,save_intr);
	return(iwait==-1 ? -1 : status);
}

#endif /*UNIX*/