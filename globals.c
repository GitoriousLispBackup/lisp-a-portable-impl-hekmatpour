#include "kernel.h"

char* err_args="incorrect number of arguments";
char* err_pars="bad parameter(s)";
char* err_evalstk="evaluation stack overflow";
char* err_varstk="variable stack overflow";
char* err_argstk="argument stack overflow";
char* err_catstk="catch stack overflow";
char* err_memory="memory space exhausted";
char* err_int="integer number expected";

char* err_real="real number expected";
char* err_num="number expected";
char* err_str="string expected";
char* err_chan1="channel expected";
char* err_chan2="bad channel";
char* err_sym1="symbol expected";
char* err_sym2="non-constant symbol expected";
char* err_pair="non-nil list expected";
char* err_list="list expected";
char* err_var="bad variable";
char* err_dom="bad domain designator";

kerncell catres;
kerncell golabel;
kerncell _tempstr;
kerncell read_and_eval,top_lev_call,top_lev_tags;
kerncell inchan,outchan,errchan;
iochan _inchan,_outchan,_errchan;
char strbuf[STRBUFSIZE+2];
struct variable varstk[VARSTKSIZE+1];
kerncell evalstk[EVALSTKSIZE+1];
kerncell argstk[ARGSTKSIZE+1];
int evaltop=-1;
int celltop=EVALSTKSIZE;
int vartop=-1;
int argtop=-1;
int _argtop=-1;
int(*org_interrupt)();

kernsym _bquotesym,_commasym,_atsym,_toptagsym,_errtagsym,_rettagsym,_gotagsym,_tempsym,
	_cxxrsym;
kernsym nil,ttt,eofsym,inchansym,outchansym,errchansym;
kernsym lamsym,vlamsym,ulamsym,mlamsym;
kernsym evalsym,callsym,applysym;
kernsym opensym,closesym,flushsym,readsym,printsym,princsym,tabsym,
	terprisym,prlensym,iobufsym,chanpsym,ppsym;
kernsym plussym,minussym,timessym,divsym,sumsym,prodsym,remsym,
	powsym,incsym,decsym,abssym,negsym,intsym,realsym,
	a_ltsym,a_gtsym,a_lesym,a_gesym,a_eqsym,a_nesym,
	numberpsym,intpsym,realpsym;
kernsym s_ltsym,s_gtsym,s_eqsym,strcmpsym,nthcharsym,substrsym,
	strlensym,strconcsym,nilstrpsym,stringpsym;
kernsym symnamesym,synonymsym,gensymsym,concatsym,bindingsym,
	symbolpsym,boundpsym;
kernsym carsym,cdrsym,nthelemsym,nthpairsym,rplacasym,rplacdsym,
	lastelemsym,lastpairsym,conssym,listsym,lengthsym,concsym,
	dconcsym,removesym,dremovesym,substsym,dsubstsym,reversesym,
	dreversesym,membersym,memqsym,equalsym,nequalsym,eqsym,neqsym,
	atompsym,listpsym,pairpsym,nullpsym;
kernsym convsetsym,dconvsetsym,convlistsym,dconvlistsym,
	esetsym,isetsym,unionsym,intsecsym,diffsym,subsetsym;
kernsym notsym, andsym,orsym,condsym,implysym,equivsym,
	allsym,existsym,onesym;
kernsym putpropsym,rempropsym,getsym,plistsym,setplistsym,
	assocsym,assqsym;
kernsym vectorsym,storesym,dimensionsym,vectorpsym;
kernsym catchsym,throwsym,caperrsym,errorsym,toplevelsym,
	resetsym,exitsym;
kernsym progsym,gosym,returnsym,dosym;
kernsym mapcarsym,mapasym,mapcdrsym,mapdsym;
kernsym voidsym,quotesym,kwotesym,defsym,funsym,argsym,letsym,
	setsym,setqsym,constsym,sssym,loadsym,shellsym;
