#ifndef _ISA_HH_
#define _ISA_HH_

#include <CDC8600.hh>

// 1. Branch Facility
#define LABEL(L)                                                                                                       \
    L : {                                                                                                              \
    }

#define jmp(L)			if (process(new  instructions::jmp   (0)))      goto L;

#define jmpa(L)			if (process(new  instructions::jmpa  (0)))      goto L;

#define jmprng(Xj,L)		if (process(new instructions::jmprng (Xj,  0))) goto L;

#define jmpnrng(Xj,L)		if (process(new instructions::jmpnrng(Xj,  0))) goto L;

#define jmpz(Xj,L)		if (process(new instructions::jmpz   (Xj,  0))) goto L;

#define jmpnz(Xj,L)		if (process(new instructions::jmpnz  (Xj,  0))) goto L;

#define jmpp(Xj,L)		if (process(new instructions::jmpp   (Xj,  0))) goto L;

#define jmpn(Xj,L)		if (process(new instructions::jmpn   (Xj,  0))) goto L;

#define bb(Xj,Xk,L)		if (process(new instructions::bb     (Xj, Xk))) goto L;

#define call(Xj,func)		if (process(new instructions::call   (Xj)))     func ();

#define calla(Xj,func)		if (process(new instructions::calla  (Xj)))     func ();

#define callp(Xj,Xk)		if (process(new instructions::callpi (Xj, Xk)));

#define jmpk(Xj,k)		if (process(new instructions::jmpk   (Xj, k)))  return;

#define rdjki(Xi, Xj, Xk)	instructions::rdjki(Xi, Xj, Xk);

#define sdjki(Xi, Xj, Xk)	instructions::sdjki(Xi, Xj, Xk);

#define rdKj(Xj, K)		instructions::rdKj(Xj, K);

#define xkj(Xj, k)		process(new instructions::xkj(Xj, k));

#define isjki(Xi, Xj, Xk)	process(new instructions::isjki(Xi, Xj, Xk));

#define idjkj(Xj, k)		process(new instructions::idjkj(Xj, k));

#define idzkj(Xj, Xk)		process(new instructions::idzkj(Xj, Xk));

#define isjkj(Xj, Xk)		process(new instructions::isjkj(Xj, Xk));

#define ipjkj(Xj, Xk)		process(new instructions::ipjkj(Xj, Xk));

#endif // _ISA_HH_
