#include "kernel.h"
#include <malloc.h>
#include <math.h>
#include <string.h>

#define EOL '\n'
#define TAB '\t'
#define SPACE ' '
#define ESCAPE 033

#define LPARENTOK 1 /*(*/
#define RPARENTOK 2 /*(*/
#define LBRACKTOK 3 /*[*/
#define RBRACKTOK 4 /*]*/
#define LBRACETOK 5 /*{*/
#define RBRACETOK 6 /*}*/
#define QUOTETOK  7 /*'*/
#define BQUOTETOK 8 /*`*/
#define COMMATOK  9 /*,*/
#define ATTOK			10/*@*/
#define HASHTOK		11/*#*/
#define STRTOK		12/*string token*/
#define SYMTOK		13/*symbol token*/
#define EOLTOK		14/*end-of-line token*/
#define EOFTOK		15/*end-of-file token*/
#define INUMTOK		16/*integer number token*/
#define RNUMTOK		17/*real number token*/

#define NEXTtok(chan) ((chan)->tok=nexttok(chan))
#define ISdigit(ch) ((ch)>='0' && (ch)<='9')
#define DIGITvalue(ch) ((ch)-'0')

int bufprint_noargs(int,iochan,char*);
void tab(int column,iochan chan);
int isnum(char*);
void pp(kerncell expr,iochan chan,int lmar,int rmar);

int inumber=0;
real rnumber=0.0;

/*for use by pp:*/
int ppcols=MAXCOLS; /*maximum no. of columns on the screen*/
int ppgap=MAXCOLS; /*free gap for printing*/
int llimit=MAXCOLS-30;/*left limit*/
int rlimit=MAXCOLS+15;/*right limit*/

int printaux_nomax(int flag,kerncell expr,iochan chan);/*auxiliary*/

iochan openchan(FILE* file,int mode)/*open channel*/
{
	iochan chan;

	chan=CONVchan(klnew(sizeof(struct channel)));
	chan->buf=klnew(CHANBUFSIZE+2);
	chan->ch=EOL;
	chan->tok=EOLTOK;
	chan->pos=chan->len=0;
	chan->mode=mode;
	chan->file=file;
	return(chan);
}/*openchan*/

void closechan(iochan chan) /*close channel*/
{
	if(chan->mode==OUTCHAN && chan->len>0)
		bufprint(PRINT,chan,"\n");/*flush the buffer*/
	fclose(chan->file);
	free(chan->buf);
	free(chan);
}/*closechan*/

void initio() /*initialize I/O channels*/
{
	inchan=mkchan(_inchan=openchan(stdin,INCHAN));
	outchan=mkchan(_outchan=openchan(stdout,OUTCHAN));
	errchan=mkchan(_errchan=openchan(stderr,OUTCHAN));
}/*initio*/

char nextch(iochan chan)/*returns the next character from chan*/
{
	char ch;
	if(chan->pos<chan->len) /*more chars in buffer?*/
		return(chan->ch=chan->buf[chan->pos++]);
	while((ch=getc(chan->file))!=EOL && ch!=EOF)
		if(chan->len<CHANBUFSIZE) /*store it in the buffer*/
			chan->buf[chan->len++]=ch;
		else {
			chan->buf[chan->len]=0;
			while((ch=getc(chan->file))!=EOL && ch!=EOF)
				;/*skip till end of line*/
			error(readsym,"line too long",_mkstr(chan->buf));
		}
		if(chan->len==0) /*empty line?*/
			return(chan->ch=ch); /*ch is one of EOL,EOF*/
		chan->buf[chan->len++]=EOL;/*put a newline at the end*/
		chan->buf[chan->len]=0;/*null the end of string*/
		return(chan->ch=chan->buf[chan->pos++]);
}/*nextch*/

int nexttok(iochan chan)/*fetch and return the next token from chan*/
{
start:
	while(chan->ch==SPACE || chan->ch==TAB) /*skip blanks*/
		nextch(chan);
	switch(chan->ch){
	case '(': nextch(chan); return(LPARENTOK);
	case ')': nextch(chan); return(RPARENTOK);
	case '[': nextch(chan); return(LBRACKTOK);
	case ']': nextch(chan); return(RBRACKTOK);
	case '{': nextch(chan); return(LBRACETOK);
	case '}': nextch(chan); return(RBRACETOK);
	case '\'': nextch(chan); return(QUOTETOK);
	case '`': nextch(chan); return(BQUOTETOK);
	case ',': nextch(chan); return(COMMATOK);
	case '@': nextch(chan); return(ATTOK);
	case '#': nextch(chan); return(HASHTOK);
	case ';': chan->pos=chan->len=0; /*ignore comments*/
		nextch(chan);
		goto start;
	case '"':{
		int i=0;/*string is stored in strbuf*/
		while(nextch(chan)!='"' && chan->ch!=EOL && chan->ch!=EOF)
			strbuf[i++]=(chan->ch=='\\'?nextch(chan):chan->ch);
		strbuf[i]=0;
		if(chan->ch==EOL || chan->ch==EOF)
			error(readsym,"broken string",_mkstr(strbuf));
		nextch(chan);
		return(STRTOK);
					 }
	case '|':{
		int i=0;/*strange atom is stored in strbuf*/
		strbuf[i++]=chan->ch;
		while(nextch(chan)!='|' && chan->ch!=EOL && chan->ch!=EOF)
			strbuf[i++]=(chan->ch=='\\'?nextch(chan):chan->ch);
		strbuf[i++]='|';
		strbuf[i]=0;
		if(chan->ch==EOL || chan->ch==EOF)
			error(readsym,"broken atom",_mkstr(strbuf));
		nextch(chan);
		return(SYMTOK);
					 }
	case EOL: return(EOLTOK);/*end-of-line is reported*/
	case EOF: return(EOFTOK);/*end-of-file is reported*/
	case ESCAPE: nextch(chan); /*ignore escapes*/
		goto start;
		default:{
			int i=0;/*nums and syms are stored in strbuf*/
			strbuf[i++]=chan->ch;
			while(nextch(chan)!='(' && chan->ch!=')'
				&& chan->ch!='[' && chan->ch!=']'
				&& chan->ch!='{' && chan->ch!='}'
				&& chan->ch!=SPACE && chan->ch!=TAB
				&& chan->ch!=EOL && chan->ch!=EOF)
				strbuf[i++]=chan->ch;
			strbuf[i]=0;
			return(atomkind(strbuf));
						}
	}/*switch*/
}/*nexttok*/

int skipeoltok(iochan chan,int flag)/*skip eol token and return the next token*/
{
	if(flag)
		NEXTtok(chan);
	while(chan->tok==EOLTOK){/*skip eol's*/
		nextch(chan);
		NEXTtok(chan);
	}
	return(chan->tok);
}/*skipeoltok*/

int atomkind(char* name)/*work out whether name is a number or symbol*/
{
	int sign=1,frac=0,places=0;
	double pow();
	if(isnum(name)){
		rnumber=inumber=0;
		if(*name=='+' || *name=='-')/*signed number?*/
			sign=(*name++=='+'?1:-1);
		while(*name && *name!='.'){
			inumber=10*inumber + DIGITvalue(*name);
			++name;
		}
		if(*name=='.'){
			++name;
			while(*name && ISdigit(*name)){/*work out fraction*/
				frac=10*frac + DIGITvalue(*name);
				++name;
				++places;
			}
			rnumber=(float)(sign*(inumber+((double)frac)*pow(10.0,-(double)places)));
			return(RNUMTOK);/*real number*/
		}
		inumber*=sign;
		return(INUMTOK);/*integer number*/
	}
	return(SYMTOK);/*symbol*/
}/*atomkind*/

int isnum(char* name)/*is name a number string*/
{
	int decpoint=0;
	if(*name=='+' || *name=='-')
		++name;
	if(*name==0)/*empty name can't be number*/
		return(0);
	while(*name && (ISdigit(*name) || *name=='.')){
		if(*name=='.'){/*at most 1 decimal point allowed*/
			if(decpoint)
				return(0);
			decpoint=1;
		}
		++name;/*skip all digits and decimal point*/
	}
	return(*name==0);/*there must be nothing left*/
}/*isnum*/

kerncell readaux(iochan chan,int bq)/*read an s-expression from chan*/
	/*bq is non-zero when in a back-quote s-expression*/
{
	int save_celltop=celltop;/*save the top of cell stack*/
	kerncell obj;
	if(chan==_inchan && _outchan->len>0){
		fprintf(_outchan->file,"%s",_outchan->buf);/*flush output*/
		_outchan->len=0;
	}
	obj=readaux1(chan,bq);
	celltop=save_celltop;/*restore top of cell stack*/
	return(CELLpush(obj));
}/*readaux*/

kerncell readaux1(iochan chan,int bq)/*read an s-expression: for internal use ONLY*/
	/*bq is non-zero when in a back-quote s-expression*/
{
	kerncell obj;
start:
	skipeoltok(chan,0);
	switch(chan->tok){
	case SYMTOK:
		obj=CONVcell(mksym(strbuf));
		NEXTtok(chan);
		break;
	case INUMTOK:
		obj=mkinum(inumber);
		NEXTtok(chan);
		break;
	case RNUMTOK:
		obj=mkrnum(rnumber);
		NEXTtok(chan);
		break;
	case STRTOK:
		obj=mkstr(strbuf);
		NEXTtok(chan);
		break;
	case LPARENTOK:
	case LBRACKTOK:{
		/*NOTE: ) matches ( only, and ] matches [ only*/
		int right=(chan->tok==LPARENTOK?RPARENTOK:RBRACKTOK);
		kerncell list;
		if(skipeoltok(chan,1)==right){
			NEXTtok(chan);
			return(NIL);/*()*/
		}
		obj=list=mkcell(readaux1(chan,bq),nil);/*(* ...)*/
		while(skipeoltok(chan,0),
			chan->tok!=RPARENTOK && chan->tok!=RBRACKTOK
			&& chan->tok!=EOFTOK){
				list->CELLcdr=mkcell(readaux1(chan,bq),nil);
				list=list->CELLcdr;
		}
		if(chan->tok==EOFTOK)
			error(readsym,"unexpected end of file",0);
		if(chan->tok!=right){
			if(chan->tok==RPARENTOK)
				error(readsym,"[ ... ) is not allowed",0);
			else
				error(readsym,"( ... ] is not allowed",0);
		}
		NEXTtok(chan);
		break;
								 }
	case LBRACETOK:{
		kerncell set;
		if(skipeoltok(chan,1)==RBRACETOK){
			NEXTtok(chan);
			return(NIL);/*{}*/
		}
		obj=set=mkset(readaux1(chan,bq),nil);/* (* ...) */
		while(skipeoltok(chan,0),
			chan->tok!=RBRACETOK && chan->tok!=EOFTOK){
				set->CELLcdr=mkset(readaux1(chan,bq),nil);
				set=set->CELLcdr;
		}
		if(chan->tok==EOFTOK)
			error(readsym,"unexpected end of file",0);
		obj=remrep(obj);
		NEXTtok(chan);
		break;
								 }
	case QUOTETOK:
		NEXTtok(chan);
		obj=mkcell(quotesym,mkcell(readaux1(chan,bq),nil));
		break;
	case BQUOTETOK:
		NEXTtok(chan);
		obj=transform(readaux1(chan,1));
		break;
	case COMMATOK:
		NEXTtok(chan);
		if(!bq)
			error(readsym,"',' outside a back-quoted s-expression",0);
		obj=mkcell(_commasym,readaux1(chan,bq));
		break;
	case ATTOK:
		NEXTtok(chan);
		if(!bq)
			error(readsym,"'@' outside a back-quoted s-expression",0);
		obj=mkcell(_atsym,readaux1(chan,bq));
		break;
	case HASHTOK:
		NEXTtok(chan);
		obj=eval(readaux1(chan,bq));
		break;
	case EOLTOK:
		chan->ch=SPACE;
		NEXTtok(chan);
		goto start;
	case EOFTOK:
		return(CONVcell(eofsym));
	case RPARENTOK:
		NEXTtok(chan);
		error(readsym,"unexpected ')'",0);
	case RBRACKTOK:
		NEXTtok(chan);
		error(readsym,"unexpected ']'",0);
	case RBRACETOK:
		NEXTtok(chan);
		error(readsym,"unexpected '}'",0);
	default:
		NEXTtok(chan);
		return(NIL);
								 }/*switch*/
	return(obj);
}/*readaux1*/

int hasmacro(kerncell expr)/*returns non-zero when expr contains ',' or '@'*/
{
	if(!ISlist(expr))
		return(0);
	if(expr->CELLcar==CONVcell(_commasym) ||
		expr->CELLcar==CONVcell(_atsym))
		return(1);
	while(ISlist(expr)){
		if(hasmacro(expr->CELLcar))
			return(1);
		expr=expr->CELLcdr;
	}
	return(0);
}/*hasmacro*/

/*
`x 'x
``x ''x
`(x,y) (cons 'x (cons y nil))
`(x @y z) (cons 'x (conc y '(z)))
*/
kerncell transform(kerncell list)/*transform back-quoted s-expressions*/
{
	kerncell obj;
	if(list==NIL)
		return(NIL);
	if(!hasmacro(list))
		return(mkcell(quotesym,mkcell(list,nil)));
	if(!ISlist(obj=list->CELLcar)){
		if(obj==CONVcell(_commasym) || obj==CONVcell(_atsym))
			return(eval(transform(list->CELLcdr)));
		return(mkcell(conssym,
			mkcell(mkcell(quotesym,mkcell(obj,nil)),
			mkcell(transform(list->CELLcdr),nil))));
	}
	if(obj->CELLcar==CONVcell(_commasym))
		return(mkcell(conssym,
			mkcell(eval(transform(obj->CELLcdr)),
			mkcell(transform(list->CELLcdr),nil))));
	if(obj->CELLcar==CONVcell(_atsym))
		return(mkcell(concsym,
			mkcell(eval(transform(obj->CELLcdr)),
			mkcell(transform(list->CELLcdr),nil))));
	return(mkcell(conssym,
		mkcell(transform(obj),
		mkcell(transform(list->CELLcdr),nil))));
}/*transform*/

int printaux_nomax(int flag,kerncell expr,iochan chan)/*auxiliary*/
{
	int dontcare=0;
	return printaux(flag,expr,chan,dontcare);
}

int printaux(int flag,kerncell expr,iochan chan,int max)/*auxiliary*/
	/*max specifies an upper bound when flag is LENGTH*/
{
	if(ISsym(expr))/*is expr a symbol?*/
		return(bufprint((flag==PRINC && *CONVsym(expr)->name=='|'?
				STRIP:flag),
				chan,"%s",CONVsym(expr)->name));
	switch(expr->flag){
	case INTOBJ:
		return(bufprint(flag,chan,"%1d",expr->CELLinum));
	case REALOBJ:
		return(bufprint(flag,chan,"%f",expr->CELLrnum));
	case STROBJ:
		return(bufprint(flag,chan,
			(flag=PRINC? "%s" : "\"%s\""),
			expr->CELLstr));
	case CHANOBJ:
		return(bufprint(flag,chan,"<channel:%1d>",expr->CELLchan));
	case VECTOROBJ:
		return(bufprint(flag,chan,"vector[%1d]",expr->CELLdim->CELLinum));
	case LISTOBJ:
		if(expr->CELLcar==CONVcell(quotesym)){
			bufprint(flag,chan,"'");
			return(1+printaux(flag,expr->CELLcdr->CELLcar,chan,max));
		}
	case SETOBJ:{/*handles lists and sets*/
		int size;
		int oflag=expr->flag;
		size=bufprint(flag,chan,(oflag==LISTOBJ ? "(" : "{"));
		do{
			if(flag==LENGTH && size>max)
				return(size);
			size+=printaux(flag,expr->CELLcar,chan,max);
			if((expr=expr->CELLcdr) != NIL){
				if(expr->flag!=oflag){
					if(flag==LENGTH && size>max)
						return(size);
					size+=bufprint(flag,chan," . ");
					size+=printaux(flag,expr,chan,max);
					break;
				}
				else
					size+=bufprint(flag,chan," ");
			}
		}while(expr!=NIL);
		size+=bufprint(flag,chan,(oflag==LISTOBJ?")":"}"));
		return(size);
							}
	default:
		return(bufprint(flag,chan,"<@:%1d>",expr->CELLcar));
	}/*switch*/
}/*printaux*/

int bufprint_noargs(int flag,iochan chan,char* format)/*buffered print*/
{
	word dontcare;
	return bufprint(flag,chan,format,dontcare);
}

int bufprint(int flag,iochan chan,char* format,word arg)/*buffered print*/
{
	static char outputbuf[CHANBUFSIZE+2];
	char*outbuf=outputbuf;
	sprintf(outbuf,format,arg);
	if(flag==LENGTH)
		return(strlen(outputbuf));
	else if(flag==STRIP){ /*strip |symbol| to symbol*/
		++outbuf;
		*(outbuf+strlen(outbuf)-1)=0;
	}
	if(chan->len>0)
		--(chan->len);/*get rid of the last null char*/
	do{
		*(chan->buf+chan->len++)=*outbuf;
		if(*outbuf==EOL || chan->len>CHANBUFSIZE){
			*(chan->buf+chan->len)=0;
			fprintf(chan->file,"%s",chan->buf);
			chan->len=0;
			if(!*(outbuf+1))
				break;
		}
	}while(*outbuf++);
	return(strlen(outputbuf));
}/*bufprint*/

/*
=> (setq f (open "test" "w")) ;;open file test for writing
<channel:135236>
*/
kerncell Lopen()/*(open 'name 'mode)*/
{
	kerncell arg1=ARGnum1;
	kerncell arg2=ARGnum2;
	CHECKlargs(opensym,2);
	return(openaux(GETstr(opensym,arg1),GETstr(opensym,arg2)));
}/*Lopen*/

kerncell openaux(char*name,char*mode)/*open a channel*/
{
	FILE *file,*fopen();
	if((file=fopen(name,mode))==NULL)
		error(opensym,"can't open file",_mkstr(name));
	return(mkchan(openchan(file,
		(*mode=='r'?
		(*++mode!=0 ? INOUTCHAN:INCHAN)
		:OUTCHAN))));
}/*openaux*/

/*
=> (close f)
t
*/
kerncell Lclose()/*(close 'chan)*/
{
	kerncell arg=ARGnum1;
	CHECKlargs(closesym,1);
	closechan(GETchan(closesym,arg));
	arg->flag=VOID;/*arg is no more a channel*/
	return(TTT);
}/*Lclose*/

kerncell Vflush()/*(flush ['chan])*/
{
	kerncell arg;
	iochan chan;
	CHECKvargs2(flushsym,1);
	chan=(ARGidx1==argtop?
		_outchan
		:(arg=ARGnum1,GETchan(flushsym,arg)));
	if(chan->len==0)
		return(TTT);
	if(chan->mode==OUTCHAN || chan->mode==INOUTCHAN)
		bufprint_noargs(PRINT,chan,"\n");
	else
		chan->len=0;
	return(TTT);
}/*Vflush*/

kerncell Vread() /*(read ['chan])*/
{
	kerncell arg;
	CHECKvargs2(readsym,1);
	if(argtop==ARGidx1)
		return(readaux(_inchan,0));
	else{
		if(!ISchan(arg=ARGnum1) || arg->CELLchan->mode==OUTCHAN)
			error(readsym,err_chan2,arg);
		return(readaux(arg->CELLchan,0));
	}
}/*Vread*/

kerncell Vprint()/*(print 'expr [;chan])*/
{
	kerncell arg2;
	CHECKvargs(printsym,1,2);
	if(argtop-ARGidx1==1)
		printaux_nomax(PRINT,ARGnum1,_outchan);
	else{
		if(!ISchan(arg2=ARGnum2) || arg2->CELLchan->mode==INCHAN)
			error(printsym,err_chan2,arg2);
		printaux_nomax(PRINT,ARGnum1,arg2->CELLchan);
	}
	return(TTT);
}/*Vprint*/

kerncell Vprinc()/*(princ 'expr ['chan])*/
{
	kerncell arg2;
	if(argtop-ARGidx1 == 1)
		printaux_nomax(PRINC,ARGnum1,_outchan);
	else{
		if(!ISchan(arg2=ARGnum2) || arg2->CELLchan->mode == INCHAN)
			error(princsym,err_chan2,arg2);
		printaux_nomax(PRINC,ARGnum1,arg2->CELLchan);
	}
	return(TTT);
}/*Vprinc*/

kerncell Vtab()/*(tab 'column ['chan])*/
{
	kerncell arg1=ARGnum1, arg2;
	iochan chan;
	CHECKvargs(tabsym,1,2);
	if(argtop-ARGidx1 == 1)
		chan=_outchan;
	else{
		if(!ISchan(arg2=ARGnum2) || arg2->CELLchan->mode==INCHAN)
			error(tabsym,err_chan2,arg2);
		chan=arg2->CELLchan;
	}
	arg1=ARGnum1;
	tab(GETint(tabsym,arg1),chan);
	return(TTT);
}/*Vtab*/

void tab(int column,iochan chan)/*tab*/
{
	if(column>CHANBUFSIZE)
		column=CHANBUFSIZE;
	if(chan->len>column)
		bufprint_noargs(PRINT,chan,"\n");
	if(column<0)
		return;
	while(chan->len<column)
		*(chan->buf+chan->len++) = SPACE;
	*(chan->buf+chan->len)=0;
}/*tab*/

kerncell Vterpri()/*(terpri ['chan])*/
{
	kerncell arg;
	CHECKvargs2(terprisym,1);
	if(argtop==ARGidx1)
		bufprint_noargs(PRINT,_outchan,"\n");
	else{
		if(!ISchan(arg=ARGnum1) || arg->CELLchan->mode==INCHAN)
			error(terprisym,err_chan2,arg);
		bufprint_noargs(PRINT,arg->CELLchan,"\n");
	}
	return(TTT);
}/*Vterpri*/

kerncell Vprlen()
{
	kerncell arg2;
	int max;
	CHECKvargs(prlensym,1,2);
	max=(argtop-ARGidx1==1?
		MAXCOLS
		: (arg2=ARGnum2,GETint(prlensym,arg2)));
	return(nil);
}/*Vprlen*/

kerncell Viobuf()
{
	kerncell arg;
	CHECKvargs2(iobufsym,1);
	return(mkinum(ARGidx1==argtop?
		_outchan->len
		:(arg=ARGnum1,GETchan(iobufsym,arg)->len)));
}

kerncell Lchanp()
{
	CHECKlargs(chanpsym,1);
	return(ISchan(ARGnum1) ? TTT:NIL);
}

kerncell Vpp()
{
	kerncell arg1=ARGnum1;
	kerncell arg2;
	CHECKvargs(ppsym,1,1);
	ppgap=MAXCOLS;
	pp((ISfun(arg1)?CONVsym(arg1)->bind : arg1),
		(argtop-ARGidx1 == 1 ?
		_outchan
		:(!ISchan(arg2=ARGnum2) || arg2->CELLchan->mode==INCHAN?
		CONVchan(error(ppsym,err_chan2,arg2))
		:arg2->CELLchan)),
		0,0);
	return(TTT);
}

void pp(kerncell expr,iochan chan,int lmar,int rmar)
{
	int flag=expr->flag;
	int lmar1;

	if(lmar>llimit && printaux(LENGTH,expr,chan,rlimit)>rlimit){
		bufprint_noargs(PRINT,chan,"\n; <<=== continuned left ===<<");
		pp(expr,chan,4,0);
		bufprint_noargs(PRINT,chan,"\n; >>=== continued right ===>>\n");
		return;
	}
	tab(lmar,chan);
	if(!ISlist(expr)){
		printaux_nomax(PRINT,expr,chan);
		return;
	}
	bufprint_noargs(PRINT,chan,(flag==LISTOBJ?"(":"{"));
	if(printlen(expr,chan,rmar)<ppgap)
		do{
			pp(expr->CELLcar,chan,chan->len,rmar);
			if((expr=expr->CELLcdr)!=NIL){
				bufprint_noargs(PRINT,chan," . ");/*dotted pair*/
				pp(expr,chan,chan->len,rmar);
				break;
			}
			else
				bufprint_noargs(PRINT,chan," ");
		}while(expr!=NIL);
	else{
		if(!ISlist(expr->CELLcar) && ISlist(expr->CELLcdr->CELLcdr)){
			pp(expr->CELLcar,chan,chan->len,rmar);
			bufprint_noargs(PRINT,chan," ");
			expr=expr->CELLcdr;
		}
		lmar1=chan->len;
		do{
			pp(expr->CELLcar,chan,lmar1,
				(expr->CELLcdr==NIL? rmar+1 : rmar));
			if((expr=expr->CELLcdr)!=NIL){
				if(expr->flag!=flag){
					bufprint_noargs(PRINT,chan," . ");/*dotted pair*/
					pp(expr,chan,lmar1,rmar);
					break;
				}
				else
					bufprint_noargs(PRINT,chan," ");
			}
		}while(expr!=NIL);
	}
	bufprint_noargs(PRINT,chan,(flag==LISTOBJ ? ")" : "}"));
}/*pp*/

int printlen(kerncell expr,iochan chan,int rmar)/*length of expr*/
{
	int len;

	ppgap=ppcols-chan->len;
	len=printaux(LENGTH,expr,chan,ppgap);
	return(rmar+(len>ppgap ? ppgap : len));
}
