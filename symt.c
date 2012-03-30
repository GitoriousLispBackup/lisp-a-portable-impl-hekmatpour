#include "kernel.h"
#include <string.h>

kernsym symtab [HASHTABSIZE]; /* symbol table */

void initsymtab()	/* initialize the symbol table */
{
	register int i;

	for(i=0; i < HASHTABSIZE; ++i)
		symtab[i] = NULL;
} /* initsymtab */

int hash(register char* name) /* the hash function */
{
	register int hashaddr = 0;

	while(*name)
		hashaddr += *name++;
	return (hashaddr % HASHTABSIZE);
} /* hash */

kernsym addsym(char* name) /* add a symbol to the symbol table */
{
	int hashaddr = hash(name);
	int len;
	register kernsym newsym, sym;

	newsym = CONVsym(klnew(sizeof(struct symbol)));
	newsym->flag = UNBOUND;
	newsym->name = klnew((len = strlen(name)) + 1);
	strcpy(newsym->name,name);
	*(newsym->name + len) = 0;
	if((sym = symtab[hashaddr]) == NULL || strcmp(name,sym->name) < 0)
	{
		/* insert in front of list */
		symtab[hashaddr] = newsym;
		newsym->link = sym;
	}
	else if(sym->link == NULL)
	{ /* append to the end of list */
		sym->link = newsym;
		newsym->link = NULL;
	}
	else
	{ /* insert in list */
		while(strcmp(name,sym->link->name) > 0  &&  (sym = sym->link)->link)
			;
		newsym->link = sym->link;
		sym->link = newsym;
	}
	newsym->prop = NIL;
	return(newsym);
} /* addsym */

kernsym findsym(char* name) /* find a symbol in the symbol table */
{
	register kernsym sym = symtab[hash(name)];
	int cmp;

	while(sym!=NULL && (cmp=strcmp(name,sym->name))>0)
		sym = sym->link;
	if(sym==NULL || cmp<0) /* not found */
		return NULL;
	return sym;
} /* findsym */

kernsym mksym(register char* name)
{
	kernsym sym = findsym(name);

	if(sym==NULL && ISunbound(sym=addsym(name)) && *name=='c') {
		while(*++name=='a' || *name=='d'); /* look for c..r form */
		if(*name=='r' && *++name==0) {
			sym->flag=LBINARY;
			sym->bind=CONVcell(Lcxxr); /* see evalcall in eval.c */
		}
	}
	return sym;
} /* mksym */

kernsym _mksym(char* name)
{
	_tempsym->name=name;
	return(_tempsym);
}

kernsym newsym(char* name,byte flag,kerncell bind)
{
	kernsym sym;
	sym=addsym(name);
	sym->flag=flag;
	sym->bind=bind;
	return(sym);
}