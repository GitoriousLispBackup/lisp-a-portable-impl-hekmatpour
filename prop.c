#include "kernel.h"

kerncell Lputprop()/*(putprop 'sym 'value 'property)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	kerncell arg3=ARGnum3;
	kerncell plist;

	CHECKlargs(putpropsym,3);
	CHECKsym1(putpropsym,arg1);
	plist=CONVsym(arg1)->prop;
	while(ISlist(plist)){
		if(equal(plist->CELLcar,arg3))
			return(plist->CELLcdr->CELLcar=arg2);
		plist=plist->CELLcdr->CELLcdr;
	}
	CONVsym(arg1)->prop=mkcell(arg3,mkcell(arg2,CONVsym(arg1)->prop));
	return(arg2);
}

kerncell Lremprop()/*(remprop 'sym 'property)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	kerncell plist;
	kerncell res;

	CHECKlargs(rempropsym,2);
	CHECKsym1(rempropsym,arg1);
	plist=CONVsym(arg1)->prop;
	if(equal(plist->CELLcar,arg2)){
		CONVsym(arg1)->prop=plist->CELLcdr->CELLcdr;
		return(plist);
	}
	else{
		plist=plist->CELLcdr;
		while(ISlist(plist->CELLcdr)){
			if(equal(plist->CELLcdr->CELLcar,arg2)){
				res=plist->CELLcdr;
				plist->CELLcdr=plist->CELLcdr->CELLcdr->CELLcdr;
				return(res);
			}
			plist=plist->CELLcdr->CELLcdr;
		}
	}
	return(NIL);
}

kerncell Lget()/*(get 'sym 'property)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(getsym,2);
	CHECKsym1(getsym,arg1);
	arg1=CONVsym(arg1)->prop;
	while(ISlist(arg1)){
		if(equal(arg1->CELLcar,arg2))
			return(arg1->CELLcdr->CELLcar);
		arg1=arg1->CELLcdr->CELLcdr;
	}
	return(NIL);
}

kerncell Lplist()/*(plist 'sym)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(plistsym,1);
	CHECKsym1(plistsym,arg);
	return(CONVsym(arg)->prop);
}

kerncell Lsetplist()/*(setplist 'sym 'plist)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(setplistsym,2);
	CHECKsym1(setplistsym,arg1);
	CHECKlist(setplistsym,arg2);
	return(CONVsym(arg1)->prop=arg2);
}

kerncell Lassoc()/*(assoc 'key 'alist)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(assocsym,2);
	CHECKlist(assocsym,arg2);
	while(ISlist(arg2)){
		if(!ISlist(arg2->CELLcar))
			error(assocsym,"bad alist element",arg2->CELLcar);
		if(equal(arg1,arg2->CELLcar->CELLcar))
			return(arg2->CELLcar);
		arg2=arg2->CELLcdr;
	}
	return(NIL);
}

kerncell Lassq()/*(assq 'key 'alist)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(assqsym,2);
	CHECKlist(assqsym,arg2);
	while(ISlist(arg2)){
		if(!ISlist(arg2->CELLcar))
			error(assqsym,"bad alist element",arg2->CELLcar);
		if(arg1 == arg2->CELLcar->CELLcar)
			return(arg2->CELLcar);
		arg2=arg2->CELLcdr;
	}
	return(NIL);
}