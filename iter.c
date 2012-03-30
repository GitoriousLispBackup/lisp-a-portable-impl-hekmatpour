#include "kernel.h"

kerncell Uprog()/*(prog (...vars...) ...body...)*/
{
	kerncell list=argstk[argtop];
	kerncell vars,save;
	kerncell fox;
	kernsym sym;

	if(list==NIL || (!ISlist(vars=list->CELLcar) && vars!=NIL))
		error(progsym,"bad variable list",vars);
	save=vars;
	while(ISlist(vars)){
		if(ISsym(fox=vars->CELLcar) && !ISconst(CONVsym(fox))){
			VARpush(sym=CONVsym(fox),sym->flag,sym->bind);
			sym->flag=VARIABLE;
			sym->bind=NIL;
		}
		else if(ISlist(fox)){
			if(ISsym(sym=CONVsym(fox->CELLcar)) && !ISconst(sym))
				VARpush(sym,sym->flag,sym->bind);
			else
				error(progsym,err_var,sym);
			sym->flag=VARIABLE;
			sym->bind=eval(fox->CELLcdr->CELLcar);
		}
		else
			error(progsym,err_var,fox);
		vars=vars->CELLcdr;
	}
	fox=klcatch(prog,_rettagsym,list->CELLcdr);
	while(ISlist(save)){
		VARpop();
		save=save->CELLcdr;
	}
	return(fox);
}

kerncell prog(kerncell list)
{
	kerncell save=list;
start:
	if(klcatch(progaux,_gotagsym,list)==CONVcell(_gotagsym)){
		for(list=save;ISlist(list);list=list->CELLcdr)
			if(list->CELLcar==golabel){
				list=list->CELLcdr;
				goto start;
			}
		error(gosym,"no such label",golabel);
	}
	else
		return(NIL);
	return(NIL);
}

kerncell progaux(kerncell list)
{
	while(ISlist(list)){
		if(ISlist(list->CELLcar))
			eval(list->CELLcar);
		list=list->CELLcdr;
	}
	return(NIL);
}

kerncell Ugo()
{
	if(!ISlist(argstk[argtop]))
		error(gosym,"label required",0);
	golabel=argstk[argtop]->CELLcar;
	klthrow(_gotagsym,_gotagsym);
	return NULL;
}

kerncell Vreturn()
{
	CHECKvargs2(returnsym,1);
	klthrow((ARGidx1==argtop ? NIL : ARGnum1),_rettagsym);
	return NULL;
}

kerncell Udo()
{
	kerncell list=argstk[argtop];
	kerncell res=NIL;
	kerncell fox;
	int times;

	if(list->CELLcdr==NIL)
		error(dosym,err_args,0);
	fox=eval(list->CELLcar);
	times=GETint(dosym,fox);
	list=list->CELLcdr;
	while(times-- > 0){
		fox=list;
		while(ISlist(fox)){
			res=eval(fox->CELLcar);
			fox=fox->CELLcdr;
		}
	}
	return(res);
}
