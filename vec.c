#include "kernel.h"

kerncell Lvector()/*(vector 'dim)*/
{
	kerncell arg=ARGnum1;
	kerncell vector,*vec;
	int dim;

	CHECKlargs(vectorsym,1);
	if(!ISint(arg) || (dim=arg->CELLinum)<=0)
		error(vectorsym,"bad dimension",arg);
	vec=CONVvector(klnew(sizeof(kerncell)*dim));
	vector=freshcell();
	vector->flag=VECTOROBJ;
	vector->CELLdim=arg;
	vector->CELLvec=vec;
	while(dim--)
		*vec++=NIL;
	return(vector);
}

kerncell Ustore()/*(store (vector idx) 'exr)*/
{
	kerncell list=argstk[argtop];
	kerncell slot=list->CELLcar;
	kerncell vector,index;

	if(!ISlist(slot) || !ISvector(vector=eval(slot->CELLcar))
		|| !ISint(index=eval(slot->CELLcdr->CELLcar)))
		error(storesym,"bad slot",slot);
	if(index->CELLinum<0 ||
		index->CELLinum >= vector->CELLdim->CELLinum)
		error(storesym,"index out of range",index);
	return(*(vector->CELLvec+index->CELLinum)=eval(list->CELLcdr->CELLcar));
}

kerncell Ldimension()/*(dimension 'vector)*/
{
	kerncell arg=ARGnum1;
//	kerncell* vec;

	CHECKlargs(dimensionsym,1);
	if(!ISvector(arg))
		error(dimensionsym,"non-vector argument",arg);
	return(arg->CELLdim);
}

kerncell Lvectorp()/*(vector? 'exr)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(vectorpsym,1);
	return(ISvector(arg)? TTT : NIL);
}
