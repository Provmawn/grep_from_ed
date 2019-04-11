#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "func.h"
#define	BLKSIZE	4096
#define	NBLK	2047
#define	FNSIZE	128
#define	LBSIZE	4096
#define	ESIZE	256
#define	GBSIZE	256
#define	NBRA	5
#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	14
#define	CCIRC	15
#define	STAR	01
#define	READ	0
#define	WRITE	1
int peekc, lastc, ninbuf = 0, io, oblock = -1, ichanged, nleft, nbra, tfile = -1, tline, iblock = -1;
char file[FNSIZE], linebuf[LBSIZE], expbuf[ESIZE+4], genbuf[LBSIZE], *nextip, *linebp, *braslist[NBRA], *braelist[NBRA], *globp, *loc1, *loc2, ibuff[BLKSIZE], obuff[BLKSIZE];
unsigned int *addr1, *addr2, *dot, *dol, *zero, nlall = 128;
long count;
#define MAX 200
char buf[MAX];
int bufp = 0;
int main(int argc, char *argv[]) {
	if (argc != 3) { exit(0); }
	zero = (unsigned *)malloc(nlall*sizeof(unsigned));
	init();
        snprintf(buf, MAX, "%s%c%c%s%c", argv[2], '\n', '/', argv[1], '\n');
        str_reverse(buf);
        bufp += strlen(buf);
        read_file();
        global(1);
        exit(0);
	return 0;
}
void str_reverse(char* s) {
    char* pend = s + strlen(s) - 1, *pstart = s;
    while (pend > s) {
        char temp = *s;
        *s++ = *pend;
        *pend-- = temp;
    }
}
void read_file(void) {
        int c = 'e';
        filename(c);
        init();
        addr2 = zero;
        io = open(file, 0);
        setwide();
        c = zero != dol;
        append(getfile, addr2);
        close(io);
        io = -1;
}
void commands(void) {
	unsigned int *a1 = 0;
	int c = '\n';
	char lastsep = '\n';
	for (;;) {
	for (addr1 = 0;;) {
		c = getchr();
		if (c!=',' && c!=';') { break; }
		if (a1==0) {
			a1 = zero+1;
			if (a1>dol) {a1--; }
		}
		addr1 = a1;
		if (c==';') { dot = a1; }
	}
	if (lastsep!='\n' && a1==0) { a1 = dol; }
	if ((addr2=a1)==0) {
		addr2 = dot;	
	}
	if (addr1==0) { addr1 = addr2; }
            switch(c) {
            case 'p':
                    newline();
                    print();
                    continue;
            case EOF:
                    return;
            }
	}
}
void print(void) {
	unsigned int *a1 = addr1;
	do {
	    _puts(_getline(*a1++));
	} while (a1 <= addr2);
	dot = addr2;
}
void setwide(void) {
        addr1 = zero + (dol>zero);
        addr2 = dol;
}
void newline(void) {
	int c;
	if ((c = getchr()) == '\n' || c == EOF) { return; }
}
void filename(int comm) {
	char *p1 = file, *p2 = file;
	int c;
	count = 0;
	while ((c = getchr()) == ' ');
	do {
		*p1++ = c;
	} while ((c = getchr()) != '\n');
	*p1++ = 0;
        while (*p1++ = *p2++);
}
int getchr(void) {
	if (lastc=peekc) {
		peekc = 0;
		return(lastc);
	}
	if (globp) {
		if ((lastc = *globp++) != 0)
			return(lastc);
		globp = 0;
		return(EOF);
	}
        return bufp > 0 ? buf[--bufp] : getchar();
	return(lastc);
}
int getfile(void) {
	int c;
	char *lp = linebuf, *fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0)
				if (lp>linebuf) {
					_puts("'\\n' appended");
					*genbuf = '\n';
				} else  { return(EOF); }
			fp = genbuf;
			while(fp < &genbuf[ninbuf]) {
				if (*fp++ & 0200) { break; }
			}
			fp = genbuf;
		}
		c = *fp++;
		if (c=='\0') { continue; }
		*lp++ = c;
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}
void putfile(void) {
	unsigned int *a1 = addr1;
	int n;
	char *fp = genbuf, *lp = _getline(*a1++);
	int nib = BLKSIZE;
	fp = genbuf;
	do {
		for (;;) {
			if (--nib < 0) {
				n = fp-genbuf;
				write(io, genbuf, n);
				nib = BLKSIZE-1;
				fp = genbuf;
			}
			count++;
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	} while (a1 <= addr2);
	n = fp-genbuf;
	write(io, genbuf, n);
}
int append(int (*f)(void), unsigned int *a) {
	unsigned int *a1, *a2, *rdot;
	int nline = 0, tl;
	dot = a;
	while ((*f)() == 0) {
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 > rdot) { *--a2 = *--a1; }
		*rdot = tl;
	}
	return(nline);
}
char* _getline(unsigned int tl) {
	char *bp = getblock(tl, READ), *lp = linebuf;
	int nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while (*lp++ = *bp++)
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), READ);
			nl = nleft;
		}
	return(linebuf);
}
int putline(void) {
	unsigned int tl = tline;
	char *bp = getblock(tl, WRITE), *lp = linebuf;
	int nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while (*bp = *lp++) {
		if (*bp++ == '\n') {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline += (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}
char* getblock(unsigned int atl, int iof) {
	int bno = (atl/(BLKSIZE/2)), off = (atl<<1) & (BLKSIZE-1) & ~03;
	nleft = BLKSIZE - off;
	if (bno==iblock) {
		ichanged |= iof;
		return(ibuff+off);
	}
	if (bno==oblock) { return(obuff+off); }
	if (iof==READ) {
		if (ichanged) { blkio(iblock, ibuff, write); }
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		return(ibuff+off);
	}
	if (oblock>=0) { blkio(oblock, obuff, write); }
	oblock = bno;
	return(obuff+off);
}
void blkio(int b, char *buf, int (*iofcn)(int, char*, int)) {
	lseek(tfile, (long)b*BLKSIZE, 0);
	(*iofcn)(tfile, buf, BLKSIZE);
}
void init(void) {
	int *markp;
	tline = 2;
	iblock = -1;
	oblock = -1;
	ichanged = 0;
	dot = dol = zero;
}
void global(int k) {
	int c = getchr();
	unsigned int *a1;
	char globuf[GBSIZE];
	char *gp = globuf;
	setwide();
	compile(c);
        c = getchr();
	*gp++ = 'p';
	*gp++ = '\n';
	*gp++ = 0;
	for (a1=zero; a1<=dol; a1++) {
		*a1 &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(a1)==k) { *a1 |= 01; }
	}
	for (a1=zero; a1<=dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands();
			a1 = zero;
		}
	}
}
void compile(int eof) {
	int c, cclcnt;
	char *ep = expbuf, *lastep, bracket[NBRA], *bracketp;
	bracketp = bracket;
	if ((c = getchr()) == '\n') {
		peekc = c;
		c = eof;
	}
	if (c == eof) { return; }
	nbra = 0;
	if (c=='^') {
		c = getchr();
		*ep++ = CCIRC;
	}
	peekc = c;
	lastep = 0;
	for (;;) {
		c = getchr();
		if (c == '\n') {
			peekc = c;
			c = eof;
		}
		if (c==eof) {
			*ep++ = CEOF;
			return;
		}
		if (c!='*') { lastep = ep; }
		switch (c) {
		case '\\':
			if ((c = getchr())=='(') {
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;
			}
			if (c == ')') {
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
			}
			if (c>='1' && c<'1'+NBRA) {
				*ep++ = CBACK;
				*ep++ = c-'1';
				continue;
			}
			*ep++ = CCHR;
			*ep++ = c;
			continue;
                case '.':
			*ep++ = CDOT;
			continue;
		case '*':
			if (lastep==0 || *lastep==CBRA || *lastep==CKET) { goto defchar; }
			*lastep |= STAR;
			continue;
		case '$':
			if ((peekc=getchr()) != eof && peekc!='\n') { goto defchar; }
			*ep++ = CDOL;
			continue;
		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c=getchr()) == '^') {
				c = getchr();
				ep[-2] = NCCL;
			}
			do {
				if (c=='-' && ep[-1]!=0) {
					if ((c=getchr())==']') {
						*ep++ = '-';
						cclcnt++;
						break;
					}
					while (ep[-1]<c) {
						*ep = ep[-1]+1;
						ep++;
						cclcnt++;
					}
				}
				*ep++ = c;
				cclcnt++;
			} while ((c = getchr()) != ']');
			lastep[1] = cclcnt;
			continue;
		defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
}
int execute(unsigned int *addr) {
	char *p1 = loc2, *p2 = expbuf;
	int c;
	for (c=0; c<NBRA; c++) {
		braslist[c] = 0;
		braelist[c] = 0;
	}
	if (addr == (unsigned *)0) {
		if (*p2==CCIRC) { return(0); }
	} else if (addr==zero) { return(0); }
	else { p1 = _getline(*addr); }
	if (*p2==CCIRC) {
		loc1 = p1;
		return(advance(p1, p2+1));
	}
	if (*p2==CCHR) {
		c = p2[1];
		do {
			if (*p1!=c) { continue; }
			if (advance(p1, p2)) {
				loc1 = p1;
				return(1);
			}
		} while (*p1++);
		return(0);
	}
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return(1);
		}
	} while (*p1++);
	return(0);
}
int advance(char *lp, char *ep) {
	char *curlp;
	int i;
	for (;;) switch (*ep++) {
	case CCHR:
		if (*ep++ == *lp++) { continue; }
		return(0);
	case CDOT:
		if (*lp++) {continue; }
		return(0);
	case CDOL:
		if (*lp==0) { continue; }
		return(0);
	case CEOF:
		loc2 = lp;
		return(1);
	case CCL:
		if (cclass(ep, *lp++, 1)) {
			ep += *ep;
			continue;
		}
		return(0);
	case NCCL:
		if (cclass(ep, *lp++, 0)) {
			ep += *ep;
			continue;
		}
		return(0);
	case CBRA:
		braslist[*ep++] = lp;
		continue;
	case CKET:
		braelist[*ep++] = lp;
		continue;
	case CBACK:
		if (backref(i, lp)) {
			lp += braelist[i] - braslist[i];
			continue;
		}
		return(0);
	case CBACK|STAR:
		curlp = lp;
		while (backref(i, lp)) { lp += braelist[i] - braslist[i]; }
		while (lp >= curlp) {
			if (advance(lp, ep))
				return(1);
			lp -= braelist[i] - braslist[i];
		}
		continue;
	case CDOT|STAR:
		curlp = lp;
		while (*lp++);
		goto star;
	case CCHR|STAR:
		curlp = lp;
		while (*lp++ == *ep);
		ep++;
		goto star;
	case CCL|STAR:
	case NCCL|STAR:
		curlp = lp;
		while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)));
		ep += *ep;
		goto star;
	star:
		do {
			lp--;
			if (advance(lp, ep)) { return(1); }
		} while (lp > curlp);
		return(0);
	}
}
int backref(int i, char *lp) {
	char *bp;
	bp = braslist[i];
	while (*bp++ == *lp++) {  if (bp >= braelist[i]) { return(1); } }
	return(0);
}
int cclass(char *set, int c, int af) {
	int n;
	if (c==0) { return(0); }
	n = *set++;
	while (--n) { if (*set++ == c) { return(af); } }
	return(!af);
}
void _puts(char *sp) {
	while (*sp) { putchr(*sp++); }
	putchr('\n');
}
char	line[70], *linp = line;
void putchr(int ac) { 
	char *lp = linp;
	int c = ac;
	*lp++ = c;
	if(c == '\n' || lp >= &line[64]) {
		linp = line;
		write(1, line, lp-line);
		return;
	}
	linp = lp;
}
