#include "kernel.h"

kerncell Lconvset()/*(convset 'list)*/
{
	kerncell arg=ARGnum1;
	kerncell res=NIL;

	CHECKlargs(convsetsym,1);
	CHECKlist(convsetsym,arg);
	while(ISlist(arg)){
		if(!member(arg->CELLcar,arg->CELLcdr))
			res=mkset(arg->CELLcar,res);
		arg=arg->CELLcdr;
	}
	return(res);
}

kerncell Ldconvset()/*(*convset 'list)*/
{
	kerncell arg=ARGnum1;
	kerncell res;
	CHECKlargs(dconvsetsym,1);
	CHECKlist(dconvsetsym,arg);
	while(member(arg->CELLcar,arg->CELLcdr))
		arg=arg->CELLcdr;
	res=arg;
	while(ISlist(arg->CELLcdr)){
		if(member(arg->CELLcdr->CELLcar,arg->CELLcdr->CELLcdr))
			arg->CELLcdr=arg->CELLcdr->CELLcdr;
		else{
			arg->flag=SETOBJ;
			arg=arg->CELLcdr;
		}
	}
	if(ISlist(arg))
		arg->flag=SETOBJ;
	return(res);
}

kerncell Lconvlist()/*(convlist 'set)*/
{
	kerncell arg=ARGnum1;
	kerncell res=NIL;
	CHECKlargs(convlistsym,1);
	CHECKlist(convlistsym,arg);
	while(ISlist(arg)){
		res=mkcell(arg->CELLcar,res);
		arg=arg->CELLcdr;
	}
	return(res);
}

kerncell Ldconvlist()/*(*convlist 'set)*/
{
	kerncell arg=ARGnum1;
	kerncell res=arg;
	CHECKlargs(dconvlistsym,1);
	while(ISlist(arg)){
		arg->flag=LISTOBJ;
		arg=arg->CELLcdr;
	}
	return(res);
}

kerncell Veset()/*(eset 'expr1 ... 'exprn)*/
{
	int idx=ARGidx1;
	kerncell res=NIL;
	while(idx<argtop)
		res=mkset(argstk[idx++],res);
	return(remrep(res));
}

kerncell remrep(kerncell set)/*remove repetitions from set*/
{
	kerncell res;
	while(member(set->CELLcar,set->CELLcdr))
		set=set->CELLcdr;
	res=set;
	while(ISlist(set->CELLcdr)){
		if(member(set->CELLcdr->CELLcar,set->CELLcdr->CELLcdr))
			set->CELLcdr=set->CELLcdr->CELLcdr;
		else
			set=set->CELLcdr;
	}
	return(res);
}

kerncell Uiset()/*(iset expr (var1 dom1 ... varn domn) expr1 ... exprn)*/
{
	kerncell list=argstk[argtop];
	if(checkdoms(list->CELLcdr->CELLcar))
		error(isetsym,err_dom,0);
	return(iset(list->CELLcar,list->CELLcdr->CELLcar,
		list->CELLcdr->CELLcdr));
}

kerncell iset(kerncell gen,kerncell doms,kerncell body)/*implicit set construnction*/
{
	kernsym sym=CONVsym(doms->CELLcar);
	kerncell dom=eval(doms->CELLcdr->CELLcar);
	kerncell list;
	kerncell tmp,res=NIL;

	VARpush(sym,sym->flag,sym->bind);
	sym->flag=VARIABLE;
	doms=doms->CELLcdr->CELLcdr;
	if(!ISlist(dom) && dom!=NIL)
		error(isetsym,err_dom,0);
	while(ISlist(dom)){
		sym->bind=dom->CELLcar;
		if(ISlist(doms))
			res=unionaux(iset(gen,doms,body),res);
		else{
			list=body;
			while(ISlist(list)){
				tmp=eval(list->CELLcar);
				list=list->CELLcdr;
			}
			if(tmp!=NIL)
				res=mkset(eval(gen),res);
		}
		dom=dom->CELLcdr;
	}
	VARpop();
	return(ISlist(doms) ? res : remrep(res));
}

kerncell unionaux(kerncell set1,kerncell set2)/*union of set1 and set2*/
{
	while(ISlist(set1)){
		if(!member(set1->CELLcar,set2))
			set2=mkset(set1->CELLcar,set2);
		set1=set1->CELLcdr;
	}
	return(set2);
}

kerncell Vunion()/*(union 'set1 ... 'setn)*/
{
	int idx=ARGidx1;
	kerncell argi;
	kerncell res;

	if(idx==argtop)
		return(NIL);
	if(idx+1 == argtop)
		return(ARGnum1);
	res=ARGnum1;
	CHECKlist(unionsym,res);
	while(++idx<argtop){
		argi=argstk[idx];
		CHECKlist(unionsym,argi);
		while(ISlist(argi)){
			if(!member(argi->CELLcar,res))
				res=mkset(argi->CELLcar,res);
			argi=argi->CELLcdr;
		}
	}
	return(res);
}

kerncell Vintsec()/*(intsec 'set1 ... 'setn)*/
{
	int idx=ARGidx1;
	kerncell argi;
	kerncell tmp,res;

	if(idx==argtop)
		return(NIL);
	if(idx+1 == argtop)
		return(ARGnum1);
	tmp=ARGnum1;
	CHECKlist(intsecsym,tmp);
	while(++idx<argtop){
		argi=argstk[idx];
		CHECKlist(intsecsym,argi);
		res=NIL;
		while(ISlist(argi)){
			if(member(argi->CELLcar,tmp))
				res=mkset(argi->CELLcar,res);
			argi=argi->CELLcdr;
		}
		tmp=res;
	}
	return(res);
}

kerncell Ldiff()/*(diff 'set1 'set2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	kerncell res=NIL;
	CHECKlargs(diffsym,2);
	CHECKlist(diffsym,arg1);
	CHECKlist(diffsym,arg2);
	while(ISlist(arg1)){
		if(!member(arg1->CELLcar,arg2))
			res=mkset(arg1->CELLcar,res);
		arg1=arg1->CELLcdr;
	}
	return(res);
}

kerncell Lsubset()/*(subset 'set1 'set2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(subsetsym,2);
	CHECKlist(subsetsym,arg1);
	CHECKlist(subsetsym,arg2);
	while(ISlist(arg1)){
		if(!member(arg1->CELLcar,arg2))
			return(NIL);
		arg1=arg1->CELLcdr;
	}
	return(TTT);
}