#include "kernel.h"

kerncell Vmapa()
{
	kerncell fun=ARGnum1;
	int arg1=ARGidx1+1;
	int argn=argtop;
	int i;
	kerncell res=argstk[arg1];

	CHECKvargs1(mapasym,2);
	for(i=arg1;i<argn;++i)
		CHECKlist(mapasym,argstk[i]);
	ARGpush(fun);
	for(;;){
		if(argstk[arg1]==NIL)
			break;
		for(i=arg1;i<argn;++i){
			ARGpush(argstk[i]->CELLcar);
			argstk[i]=argstk[i]->CELLcdr;
		}
		ARGpush(CONVcell(argn+1));
		Vcall();
		argtop=argn+1;
	}
	argtop=argn;
	return(res);
}

kerncell Vmapcar()
{
	kerncell fun=ARGnum1;
	kerncell res=NIL;
	int arg1=ARGidx1+1;
	int argn=argtop;
	int i;

	CHECKvargs1(mapcarsym,2);
	for(i=arg1;i<argn;++i)
		CHECKlist(mapcarsym,argstk[i]);
	ARGpush(fun);
	for(;;){
		if(argstk[arg1]==NIL)
			break;
		for(i=arg1;i<argn;++i){
			ARGpush(argstk[i]->CELLcar);
			argstk[i]=argstk[i]->CELLcdr;
		}
		ARGpush(CONVcell(argn+1));
		res=mkcell(Vcall(),res);
		argtop=argn+1;
	}
	argtop=argn;
	return(dreverse(res));
}

kerncell Vmapd()
{
	kerncell fun=ARGnum1;
	int arg1=ARGidx1+1;
	int argn=argtop;
	int i;
	kerncell res=argstk[arg1];

	CHECKvargs1(mapdsym,2);
	for(i=arg1;i<argn;++i)
		CHECKlist(mapdsym,argstk[i]);
	ARGpush(fun);
	for(;;){
		if(argstk[arg1]==NIL)
			break;
		for(i=arg1;i<argn;++i){
			ARGpush(argstk[i]);
			argstk[i]=argstk[i]->CELLcdr;
		}
		ARGpush(CONVcell(argn+1));
		Vcall();
		argtop=argn+1;
	}
	argtop=argn;
	return(res);
}

kerncell Vmapcdr()
{
	kerncell fun=ARGnum1;
	kerncell res=NIL;
	int arg1=ARGidx1+1;
	int argn=argtop;
	int i;

	CHECKvargs1(mapcdrsym,2);
	for(i=arg1;i<argn;++i)
		CHECKlist(mapcdrsym,argstk[i]);
	ARGpush(fun);
	for(;;){
		if(argstk[arg1]==NIL)
			break;
		for(i=arg1;i<argn;++i){
			ARGpush(argstk[i]);
			argstk[i]=argstk[i]->CELLcdr;
		}
		ARGpush(CONVcell(argn+1));
		res=mkcell(Vcall(),res);
		argtop=argn+1;
	}
	argtop=argn;
	return(dreverse(res));
}
