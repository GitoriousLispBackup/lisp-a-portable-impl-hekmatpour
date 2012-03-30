#include "kernel.h"

kerncell Lplus()/*(+ 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(plussym,2);
	if(ISint(arg1) || ISreal(arg1)){
		if(ISint(arg2))
			return(ISint(arg1) ? mkinum(arg1->CELLinum+arg2->CELLinum)
			:mkrnum(arg1->CELLrnum+arg2->CELLinum));
		if(ISreal(arg2))
			return(ISint(arg1) ? mkrnum(arg1->CELLinum+arg2->CELLrnum)
			:mkrnum(arg1->CELLrnum+arg2->CELLrnum));
		arg1=arg2;
	}
	error(plussym,err_num,arg1);
}

kerncell Lminus()/*(- 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(minussym,2);
	if(ISint(arg1) || ISreal(arg1)){
		if(ISint(arg2))
			return(ISint(arg1) ? mkinum(arg1->CELLinum-arg2->CELLinum)
			:mkrnum(arg1->CELLrnum-arg2->CELLinum));
		if(ISreal(arg2))
			return(ISint(arg1) ? mkrnum(arg1->CELLinum-arg2->CELLrnum)
			:mkrnum(arg1->CELLrnum-arg2->CELLrnum));
		arg1=arg2;
	}
	error(minussym,err_num,arg1);
}

kerncell Ltimes()/*(* 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(timessym,2);
	if(ISint(arg1) || ISreal(arg1)){
		if(ISint(arg2))
			return(ISint(arg1) ? mkinum(arg1->CELLinum*arg2->CELLinum)
			:mkrnum(arg1->CELLrnum*arg2->CELLinum));
		if(ISreal(arg2))
			return(ISint(arg1) ? mkrnum(arg1->CELLinum*arg2->CELLrnum)
			:mkrnum(arg1->CELLrnum*arg2->CELLrnum));
		arg1=arg2;
	}
	error(timessym,err_num,arg1);
}

kerncell Ldiv()/*(/ 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(divsym,2);
	if(ISint(arg1) || ISreal(arg1)){
		if(ISint(arg2))
			return(ISint(arg1) ? mkinum(arg1->CELLinum/arg2->CELLinum)
			:mkrnum(arg1->CELLrnum/arg2->CELLinum));
		if(ISreal(arg2))
			return(ISint(arg1) ? mkrnum(arg1->CELLinum/arg2->CELLrnum)
			:mkrnum(arg1->CELLrnum/arg2->CELLrnum));
		arg1=arg2;
	}
	error(divsym,err_num,arg1);
}

kerncell Lsum()/*(sum 'num1, 'num2, ..., 'numn)*/
{
	double sum=0;
	int has_real=0;
	int idx=ARGidx1;
	kerncell arg;

	while(idx<argtop){
		if(ISint(arg=argstk[idx++]))
			sum+=arg->CELLinum;
		else if(ISreal(arg)){
			has_real=1;
			sum+=arg->CELLrnum;
		}
		else
			error(sumsym,err_num,arg);
	}
	return(has_real ? mkrnum((real)sum):mkinum((int)sum));
}

kerncell Lprod()/*(prod 'num1, 'num2, ..., 'numn)*/
{
	double prod=1;
	int has_real=0;
	int idx=ARGidx1;
	kerncell arg;

	while(idx<argtop){
		if(ISint(arg=argstk[idx++]))
			prod*=arg->CELLinum;
		else if(ISreal(arg)){
			has_real=1;
			prod*=arg->CELLrnum;
		}
		else
			error(prodsym,err_num,arg);
	}
	return(has_real ? mkrnum((real)prod):mkinum((int)prod));
}

kerncell Lrem()/*(% 'inum1 'inum2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(remsym,2);
	return(mkinum(GETint(remsym,arg1) % GETint(remsym,arg2)));
}

kerncell Lpow()/*(^ 'inum1 'inum2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(powsym,2);
	return(mkrnum((real)pow((double)GETnum(powsym,arg1),
		(double)GETnum(powsym,arg2))));
}

kerncell Linc()/*(++ 'inum)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(incsym,1);
	return(mkinum(GETint(incsym,arg)+1));
}

kerncell Ldec()/*(-- 'inum)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(incsym,1);
	return(mkinum(GETint(incsym,arg)-1));
}

kerncell Labs()/*(abs 'inum)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(abssym,1);
	if(ISint(arg))
		return(arg->CELLinum>=0 ? arg:mkinum(-arg->CELLinum));
	if(ISreal(arg))
		return(arg->CELLrnum>=0 ? arg:mkrnum(-arg->CELLrnum));
	error(abssym,err_num,arg);
}

kerncell Lneg()/*(neg 'inum)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(negsym,1);
	if(ISint(arg))
		return(mkinum(-arg->CELLinum));
	if(ISreal(arg))
		return(mkrnum(-arg->CELLrnum));
	error(negsym,err_num,arg);
}

kerncell Lint()/*(int 'rnum)*/
{
	kerncell arg=ARGnum1;
	double floor();
	CHECKlargs(intsym,1);
	return(mkinum((int)floor(GETreal(intsym,arg)+0.0)));
}

kerncell Lreal()/*(real 'rnum)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(realsym,1);
	return(mkrnum((real)floor(GETint(realsym,arg)+0.0)));
}

kerncell La_lt()/*(< 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(a_ltsym,2);
	return(GETnum(a_ltsym,arg1)<GETnum(a_ltsym,arg2)?TTT : NIL);
}

kerncell La_gt()/*(> 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(a_gtsym,2);
	return(GETnum(a_gtsym,arg1)>GETnum(a_gtsym,arg2)?TTT : NIL);
}

kerncell La_le()/*(<= 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(a_lesym,2);
	return(GETnum(a_lesym,arg1)<=GETnum(a_lesym,arg2)?TTT : NIL);
}

kerncell La_ge()/*(>= 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(a_gesym,2);
	return(GETnum(a_gesym,arg1)>=GETnum(a_gesym,arg2)?TTT : NIL);
}

kerncell La_eq()/*(= 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(a_eqsym,2);
	return(GETnum(a_eqsym,arg1)==GETnum(a_eqsym,arg2)?TTT : NIL);
}

kerncell La_ne()/*(/= 'num1 'num2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(a_nesym,2);
	return(GETnum(a_nesym,arg1)!=GETnum(a_nesym,arg2)?TTT : NIL);
}

kerncell Lnumberp()/*(number? 'num1 'num2)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(numberpsym,1);
	return(ISint(arg) || ISreal(arg)?TTT:NIL);
}

kerncell Lintp()/*(int? 'exr)*/
{
	CHECKlargs(intpsym,1);
	return(ISint(ARGnum1)?TTT:NIL);
}

kerncell Lrealp()/*(real? 'expr)*/
{
	CHECKlargs(realpsym,1);
	return(ISreal(ARGnum1)?TTT:NIL);
}

/*#define FUNC(STROPNAME,STROPSYM,STROP)\
//	kerncell Ls_##STROPNAME()/*( 'str1 'str2)*/\
//{\
//	kerncell arg1=ARGnum1;\
//	kerncell arg2=ARGnum2;\
//\
//	CHECKlargs(STROPSYM,2);\
//	return(strcmp(GETstr(STROPSYM,arg1),\
//				(GETstr(STROPSYM,arg2)) STROP 0?TTT:NIL));\
//}\*/

/*#define FUNC(lt,s_ltsym,<)
//#define FUNC(STROPNAME,STROPSYM,STROP)*/
kerncell Ls_lt()
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(s_ltsym,2);
	return(strcmp(GETstr(s_ltsym,arg1),
				(GETstr(s_ltsym,arg2)) < 0?TTT:NIL));
}

/*#define FUNC(gt,s_gtsym,>)
#define FUNC(STROPNAME,STROPSYM,STROP)\*/
kerncell Ls_gt()/*( 'str1 'str2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(s_gtsym,2);
	return(strcmp(GETstr(s_gtsym,arg1),
				(GETstr(s_gtsym,arg2)) > 0?TTT:NIL));
}

/*#define FUNC(eq,s_eqsym,==)
#define FUNC(STROPNAME,STROPSYM,STROP)\*/
kerncell Ls_eq()/*( 'str1 'str2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	char* ptemp,*ptemp2;

	CHECKlargs(s_eqsym,2);
	ptemp=GETstr(s_eqsym,arg1);
	ptemp2=GETstr(s_eqsym,arg2);
	return(strcmp(ptemp,ptemp2) == 0 ? TTT:NIL);
#if 0

	return(strcmp(GETstr(s_eqsym,arg1),
				(GETstr(s_eqsym,arg2)) == 0?TTT:NIL));
#endif
}

kerncell Lstrcmp()/*(strcmp 'str1 'str2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;

	CHECKlargs(strcmpsym,2);
	return(mkinum(strcmp(GETstr(strcmpsym,arg1),
				(GETstr(strcmpsym,arg2)))));
}

kerncell Lnthchar()/*(nthchar 'str 'n)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	char* str;
	int n;
	CHECKlargs(nthcharsym,2);
	str=GETstr(nthcharsym,arg1);
	n=GETint(nthcharsym,arg2);
	while(n>0 && *str!=0){
		--n;
		++str;
	}
	return(mkinum(CONVint(*str)));
}

kerncell Lsubstr()/*(substr 'str 'i 'j)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	kerncell arg3=ARGnum3;
	char* str;
	int m,n;

	CHECKlargs(substrsym,3);
	str=GETstr(substrsym,arg1);
	m=GETint(substrsym,arg2);
	n=GETint(substrsym,arg3);
	while(m>0 && *str!=0){/*skip the first m chars*/
		--m;
		++str;
	}
	if((m=0)>n)/*negative range?*/
		strbuf[0]=0;
	else{/*copy the substring to strbuf*/
		while(m<n && *str)
			strbuf[m++]=*str++;
		strbuf[m]=0;
	}
	return(mkstr(strbuf));
}

kerncell Lstrlen()/*(strlen 'str)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(strlensym,1);
	return(mkinum(strlen(GETstr(strlensym,arg))));
}

kerncell Lstrconc()/*(strconc 'str1 'str2)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	char *str1, *str2;
	int m,n;

	CHECKlargs(strconcsym,2);
	m=strlen(str1=GETstr(strconcsym,arg1));
	n=strlen(str2=GETstr(strconcsym,arg2));
	if(m+n > STRBUFSIZE)
		error(strconcsym,"string overflow",0);
	strcpy(strbuf,str1);
	strcpy(strbuf+m,str2);
	strbuf[m+n]=0;
	return(mkstr(strbuf));
}

kerncell Lnilstrp()/*(nilstr? 'str)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(nilstrpsym,1);
	return(*(GETstr(nilstrpsym,arg))==0 ? TTT : NIL);
}

kerncell Lstringp()/*(string? 'str)*/
{
	CHECKlargs(stringpsym,1);
	return(ISstr(ARGnum1) ? TTT : NIL);
}
