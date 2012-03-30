#include "kernel.h"

#ifdef UNIX
#include <signal.h>
_interrupt()
{
	error(0,"interrupted - to exit type (exit)",0);
}
#endif/*UNIX*/

void initialize(void)
{
	initcelltab();
	initsymtab();
	initio();
	catres=mkcell(nil,nil);/*catch result pair*/
	/*internals:*/
	_bquotesym=CONVsym(&_bquotesym);
	_commasym=CONVsym(&_commasym);
	_atsym=CONVsym(&_atsym);
	INTERNALsym(_toptagsym); _toptagsym->bind=CONVcell(_toptagsym);
	INTERNALsym(_errtagsym); _errtagsym->bind=CONVcell(_errtagsym);
	INTERNALsym(_rettagsym); _rettagsym->bind=CONVcell(_rettagsym);
	INTERNALsym(_gotagsym); _gotagsym->bind=CONVcell(_gotagsym);
	INTERNALsym(_tempsym); _tempsym->bind=CONVcell(_tempsym);
	INTERNALsym(_cxxrsym); _cxxrsym->name="c..r";
	_tempstr=mkstr("");
	/*constants*/
	nil=newsym("nil",CONSTANT,0);nil->bind=nil->prop=NIL;
	ttt=newsym("t",CONSTANT,0);ttt->bind=TTT;
	eofsym=newsym("eof",CONSTANT,0);eofsym->bind=CONVcell(eofsym);

	inchansym=newsym("inchan",CONSTANT,inchan);
	outchansym=newsym("outchan",CONSTANT,outchan);
	errchansym=newsym("errchan",CONSTANT,errchan);

	lamsym=newsym("lam",UNBOUND,nil);
	vlamsym=newsym("vlam",UNBOUND,nil);
	ulamsym=newsym("ulam",UNBOUND,nil);
	mlamsym=newsym("mlam",UNBOUND,nil);

	evalsym=newsym("eval",LBINARY,Leval);
	callsym=newsym("call",VBINARY,Vcall);
	applysym=newsym("apply",LBINARY,Lapply);

	opensym=newsym("open",LBINARY,Lopen);
	closesym=newsym("close",LBINARY,Lclose);
	flushsym=newsym("flush",VBINARY,Vflush);
	readsym=newsym("read",VBINARY,Vread);
	printsym=newsym("print",VBINARY,Vprint);
	princsym=newsym("princ",VBINARY,Vprinc);
	tabsym=newsym("tab",VBINARY,Vtab);
	terprisym=newsym("terpri",VBINARY,Vterpri);
	prlensym=newsym("prlen",VBINARY,Vprlen);
	iobufsym=newsym("iobuf",VBINARY,Viobuf);
	chanpsym=newsym("chan?",LBINARY,Lchanp);
	ppsym=newsym("pp",VBINARY,Vpp);

	plussym=newsym("+",LBINARY,Lplus);
	minussym=newsym("-",LBINARY,Lminus);
	timessym=newsym("*",LBINARY,Ltimes);
	divsym=newsym("/",LBINARY,Ldiv);
	sumsym=newsym("sum",LBINARY,Lsum);
	prodsym=newsym("prod",LBINARY,Lprod);
	remsym=newsym("%",LBINARY,Lrem);
	powsym=newsym("^",LBINARY,Lpow);
	incsym=newsym("++",LBINARY,Linc);
	decsym=newsym("--",LBINARY,Ldec);
	abssym=newsym("abs",LBINARY,Labs);
	negsym=newsym("neg",LBINARY,Lneg);
	intsym=newsym("int",LBINARY,Lint);
	realsym=newsym("real",LBINARY,Lreal);
	a_ltsym=newsym("<",LBINARY,La_lt);
	a_gtsym=newsym(">",LBINARY,La_gt);
	a_lesym=newsym("<=",LBINARY,La_le);
	a_gesym=newsym(">=",LBINARY,La_ge);
	a_eqsym=newsym("=",LBINARY,La_eq);
	a_nesym=newsym("/=",LBINARY,La_ne);

	numberpsym=newsym("number?",LBINARY,Lnumberp);
	intpsym=newsym("int?",LBINARY,Lintp);
	realpsym=newsym("real?",LBINARY,Lrealp);

	s_ltsym=newsym("<<",LBINARY,Ls_lt);
	s_gtsym=newsym(">>",LBINARY,Ls_gt);
	s_eqsym=newsym("==",LBINARY,Ls_eq);
	strcmpsym=newsym("strcmp",LBINARY,Lstrcmp);
	nthcharsym=newsym("nthchar",LBINARY,Lnthchar);
	substrsym=newsym("substr",LBINARY,Lsubstr);
	strlensym=newsym("strlen",LBINARY,Lstrlen);
	strconcsym=newsym("strconc",LBINARY,Lstrconc);
	nilstrpsym=newsym("nilstr?",LBINARY,Lnilstrp);
	stringpsym=newsym("string?",LBINARY,Lstringp);

	symnamesym=newsym("symname",LBINARY,Lsymname);
	synonymsym=newsym("synonym",UBINARY,Usynonym);
	gensymsym=newsym("gensym",LBINARY,Lgensym);
	concatsym=newsym("concat",VBINARY,Vconcat);
	bindingsym=newsym("binding",LBINARY,Lbinding);
	symbolpsym=newsym("symbol?",LBINARY,Lsymbolp);
	boundpsym=newsym("bound?",LBINARY,Lboundp);

	carsym=newsym("car",LBINARY,Lcar);
	cdrsym=newsym("cdr",LBINARY,Lcdr);
	nthelemsym=newsym("nthelem",LBINARY,Lnthelem);
	nthpairsym=newsym("nthpair",LBINARY,Lnthpair);
	rplacasym=newsym("rplaca",LBINARY,Lrplaca);
	rplacdsym=newsym("rplacd",LBINARY,Lrplacd);
	lastelemsym=newsym("lastelem",LBINARY,Llastelem);
	lastpairsym=newsym("lastpair",LBINARY,Llastpair);
	conssym=newsym("cons",LBINARY,Lcons);
	listsym=newsym("list",VBINARY,Vlist);
	lengthsym=newsym("length",LBINARY,Llength);
	concsym=newsym("conc",VBINARY,Vconc);
	dconcsym=newsym("*conc",VBINARY,Vdconc);
	removesym=newsym("remove",LBINARY,Lremove);
	dremovesym=newsym("*remove",LBINARY,Ldremove);
	substsym=newsym("subst",LBINARY,Lsubst);
	dsubstsym=newsym("*subst",LBINARY,Ldsubst);
	reversesym=newsym("reverse",LBINARY,Lreverse);
	dreversesym=newsym("*reverse",LBINARY,Ldreverse);
	membersym=newsym("member",LBINARY,Lmember);
	memqsym=newsym("memq",LBINARY,Lmemq);
	equalsym=newsym("equal",LBINARY,Lequal);
	nequalsym=newsym("nequal",LBINARY,Lnequal);
	eqsym=newsym("eq",LBINARY,Leq);
	neqsym=newsym("neq",LBINARY,Lneq);
	atompsym=newsym("atom?",LBINARY,Latomp);
	listpsym=newsym("list?",LBINARY,Llistp);
	pairpsym=newsym("pair?",LBINARY,Lpairp);
	nullpsym=newsym("null?",LBINARY,Lnullp);

	convsetsym=newsym("convset",LBINARY,Lconvset);
	dconvsetsym=newsym("*convset",LBINARY,Ldconvset);
	convlistsym=newsym("convlist",LBINARY,Lconvlist);
	dconvlistsym=newsym("*convlist",LBINARY,Ldconvlist);
	esetsym=newsym("eset",VBINARY,Veset);
	isetsym=newsym("iset",UBINARY,Uiset);
	unionsym=newsym("union",VBINARY,Vunion);
	intsecsym=newsym("intsec",VBINARY,Vintsec);
	diffsym=newsym("diff",LBINARY,Ldiff);
	subsetsym=newsym("subset",LBINARY,Lsubset);

	notsym=newsym("not",LBINARY,Lnot);
	andsym=newsym("and",UBINARY,Uand);
	orsym=newsym("or",UBINARY,Uor);
	condsym=newsym("cond",UBINARY,Ucond);
	implysym=newsym("==>",LBINARY,Limply);
	equivsym=newsym("<=>",LBINARY,Lequiv);
	allsym=newsym("all",UBINARY,Uall);
	existsym=newsym("exist",UBINARY,Uexist);
	onesym=newsym("one",UBINARY,Uone);

	putpropsym=newsym("putprop",LBINARY,Lputprop);
	rempropsym=newsym("remprop",LBINARY,Lremprop);
	getsym=newsym("get",LBINARY,Lget);
	plistsym=newsym("plist",LBINARY,Lplist);
	setplistsym=newsym("setplist",LBINARY,Lsetplist);
	assocsym=newsym("assoc",LBINARY,Lassoc);
	assqsym=newsym("assq",LBINARY,Lassq);
	
	vectorsym=newsym("vector",LBINARY,Lvector);
	storesym=newsym("store",UBINARY,Ustore);
	dimensionsym=newsym("dimension",LBINARY,Ldimension);
	vectorpsym=newsym("vector?",LBINARY,Lvectorp);

	catchsym=newsym("catch",UBINARY,Ucatch);
	throwsym=newsym("throw",VBINARY,Vthrow);
	caperrsym=newsym("caperr",UBINARY,Ucaperr);
	errorsym=newsym("error",VBINARY,Verror);
	toplevelsym=newsym("toplevel",LBINARY,Ltoplevel);
	resetsym=newsym("reset",LBINARY,Lreset);
	exitsym=newsym("exit",VBINARY,Vexit);
	
	progsym=newsym("prog",UBINARY,Uprog);
	gosym=newsym("go",UBINARY,Ugo);
	returnsym=newsym("return",VBINARY,Vreturn);
	dosym=newsym("do",UBINARY,Udo);
	
	mapcarsym=newsym("mapcar",VBINARY,Vmapcar);
	mapasym=newsym("mapa",VBINARY,Vmapa);
	mapcdrsym=newsym("mapcdr",VBINARY,Vmapcdr);
	mapdsym=newsym("mapd",VBINARY,Vmapd);

	voidsym=newsym("void",UBINARY,Uvoid);
	quotesym=newsym("quote",UBINARY,Uquote);
	kwotesym=newsym("kwote",LBINARY,Lkwote);
	defsym=newsym("def",UBINARY,Udef);
	funsym=newsym("fun",MBINARY,Mfun);
	argsym=newsym("arg",LBINARY,Larg);
	letsym=newsym("let",MBINARY,Mlet);
	setsym=newsym("set",LBINARY,Lset);
	setqsym=newsym("setq",UBINARY,Usetq);
	constsym=newsym("const",UBINARY,Uconst);
	sssym=newsym("ss",VBINARY,Vss);
	loadsym=newsym("load",VBINARY,Vload);
#ifdef UNIX
	shellsym=newsym("!",UBINARY,Ushell);
#endif/*UNIX*/
	/*globals*/
	read_and_eval=mkcell(evalsym,mkcell(mkcell(readsym,nil),nil));
	top_lev_call=mkcell(toplevelsym,nil);
	top_lev_tags=mkcell(_toptagsym,mkcell(_errtagsym,nil));
#ifdef UNIX
	org_interrupt=signal(SIGINT,SIG_IGN);
	signal(SIGINT,_interrupt);
#endif/*UNIX*/
}
