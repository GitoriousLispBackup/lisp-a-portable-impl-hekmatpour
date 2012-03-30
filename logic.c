#include "kernel.h"

kerncell Ucond()/*(cond --cluase1-- ... --clausen--)*/
{
	kerncell list=argstk[argtop];
	kerncell clause;
	kerncell obj;

	while(ISlist(list)){
		clause=list->CELLcar;
		CHECKlist(condsym,clause);
		if((obj=eval(clause->CELLcar))!=NIL){
			while(ISlist(clause=clause->CELLcdr))
				obj=eval(clause->CELLcar);
			return(obj);
		}
		list=list->CELLcdr;
	}
	return(NIL);
}

kerncell Lnot()/*(not 'expr)*/
{
	CHECKlargs(notsym,1);
	return(ARGnum1==NIL ? TTT : NIL);
}

kerncell Uand()/*(and expr1 ... exprn)*/
{
	kerncell list=argstk[argtop];
	kerncell obj=NIL;

	while(ISlist(list)){
		if((obj=eval(list->CELLcar))==NIL)
			return(NIL);
		list=list->CELLcdr;
	}
	return(obj);
}

kerncell Uor()/*(or expr1 ... exprn)*/
{
	kerncell list=argstk[argtop];
	kerncell obj;
	while(ISlist(list)){
		if((obj=eval(list->CELLcar))!=NIL)
			return(obj);
		list=list->CELLcdr;
	}
	return(NIL);
}

kerncell Limply()/*(==> 'expr1 'expr2)*/
{
	CHECKlargs(implysym,2);
	return(ARGnum1==NIL || ARGnum2!=NIL ? TTT:  NIL);
}

kerncell Lequiv()/*(<=> 'expr1 'expr2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(equivsym,2);
	return(arg1==arg2 ||
		arg1!=NIL && arg2!=NIL ? TTT : NIL);
}

kerncell Uall()/*(all(var1 dom1 ... varn domn) expr1 ... exprn)*/
{
	kerncell list=argstk[argtop];
	if(list==NIL || checkdoms(list->CELLcar))
		error(allsym,err_dom,0);
	return(all(list->CELLcar,list->CELLcdr));
}

int checkdoms(kerncell doms)/*check doamin designators*/
{
	if(!ISlist(doms))
		return(1);
	do{
		if(!ISsym(doms->CELLcar) || ISconst(doms->CELLcar) ||
			!ISlist(doms=doms->CELLcdr))
			return(1);
	}while(ISlist(doms=doms->CELLcdr));
	return(0);
}

kerncell all(kerncell doms,kerncell body)/*universal quantifier*/
{
	kernsym sym=CONVsym(doms->CELLcar);
	kerncell dom=eval(doms->CELLcdr->CELLcar);
	kerncell list;
	kerncell res=TTT;
	VARpush(sym,sym->flag,sym->bind);
	sym->flag=VARIABLE;
	doms=doms->CELLcdr->CELLcdr;
	if(!ISlist(dom) && dom!=NIL)
		error(allsym,err_dom,0);
	while(ISlist(dom)){
		sym->bind=dom->CELLcar;
		if(ISlist(doms))
			res=all(doms,body);
		else{
			list=body;
			while(ISlist(list)){
				res=eval(list->CELLcar);
				list=list->CELLcdr;
			}
		}
		if(res==NIL)
			break;
		dom=dom->CELLcdr;
	}
	VARpop();
	return(res);
}

kerncell Uexist()/*(exit (var1 dom1 ... varn domn) expr1 ... exprn)*/
{
	kerncell list=argstk[argtop];

	if(list==NIL || checkdoms(list->CELLcar))
		error(existsym,err_dom,0);
	return(exist(list->CELLcar,list->CELLcdr));
}

kerncell exist(kerncell doms,kerncell body)/*existential quantifier*/
{
	kernsym sym=CONVsym(doms->CELLcar);
	kerncell dom=eval(doms->CELLcdr->CELLcar);
	kerncell list;
	kerncell res=NIL;

	VARpush(sym,sym->flag,sym->bind);
	sym->flag=VARIABLE;
	doms=doms->CELLcdr->CELLcdr;
	if(!ISlist(dom) && dom!=NIL)
		error(exitsym,err_dom,0);
	while(ISlist(dom)){
		sym->bind=dom->CELLcar;
		if(ISlist(doms))
			res=exist(doms,body);
		else{
			list=body;
			while(ISlist(list)){
				res=eval(list->CELLcar);
				list=list->CELLcdr;
			}
		}
		if(res!=NIL)
			break;
		dom=dom->CELLcdr;
	}
	VARpop();
	return(res);
}

kerncell Uone()/*(one (var1 dom1 deflt) expr1 ... exprn)*/
{
	kerncell list=argstk[argtop];
	kerncell dom;
	kerncell body,deflt,res;
	kernsym sym;

	if(list==NIL || !ISlist(dom=list->CELLcar) ||
		!ISsym(sym=CONVsym(dom->CELLcar)) || ISconst(sym))
		error(onesym,err_dom,0);
	deflt=eval(dom->CELLcdr->CELLcdr->CELLcar);
	VARpush(sym,sym->flag,sym->bind);
	sym->flag=VARIABLE;
	dom=eval(dom->CELLcdr->CELLcar);
	if(!ISlist(dom) && dom!=NIL)
		error(onesym,err_dom,0);
	body=list->CELLcdr;
	while(ISlist(dom)){
		sym->bind=dom->CELLcar;
		list=body;
		while(ISlist(list)){
			res=eval(list->CELLcar);
			list=list->CELLcdr;
		}
		if(res!=NIL)
			break;
		dom=dom->CELLcdr;
	}
	VARpop();
	return(ISlist(dom)?dom->CELLcar:deflt);
}