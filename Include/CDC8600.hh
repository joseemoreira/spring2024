#ifndef _CDC8600_HH_
#define _CDC8600_HH_

#include<stdlib.h>
#include<stdint.h>
#include<assert.h>
#include<vector>
#include<iostream>
#include<complex>

using namespace std;

namespace CDC8600
{
    typedef uint64_t		u64;
    typedef uint32_t		u32;
    typedef uint8_t		u08;
    typedef int64_t		i64;
    typedef int32_t		i32;
    typedef double		f64;
    typedef complex<double>	c128;

    namespace params
    {

	namespace MEM
	{
	    const uint32_t	N = 262144;	// Memory size = 256Ki words
	} // namespace Memory

    } // namespace params

    class word		// 64 bits, interpreted as signed, unsigned, or float
    {
	private:

	    union
	    {
		u64	u;
		i64	i;
		f64	f;
	    } _data;

	public:

	    word& operator=(i64 x)
	    {
		_data.i = x;
		return *this;
	    }

	    u64& u() { return _data.u; } 
	    u64  u() const { return _data.u; }
	    i64& i() { return _data.i; }
    };

    template<int n> class reg
    {
	private:
	    u32	_loc;	// location of memory word containing this register
	    u08 _first;	// first bit in word for this register
	public:

	    reg(u32 loc, u08 first) : _loc(loc), _first(first) { assert(n <= 20); assert(_first + n <= 64); }
	    u64 u();
	    reg<1> operator()(uint8_t);
	    reg<n>& operator=(bool);
	    reg<n>& operator=(u64);
    };

    class Processor
    {
	private:

	public:

	    uint8_t	_XA;		// The address of the current exchange packet	
	    word&	X(uint8_t i);	// Xi register in current exchange packet
	    reg<4>	mode();		// mode field of current XPW
	    reg<8>	cond();		// cond field of current XPW
	    reg<12>	RA();		// RA field of current XPW
	    reg<8>	XA();		// XA field of current XPW
	    reg<12>	FL();		// FL field of current XPW
	    reg<20>	P();		// P field of current XPW
    };

    extern vector<word>	MEM;
    extern uint32_t	FreeMEM;
    extern Processor	PROC;

    void reset();

    void *memalloc(u64);

    class call0
    {
      private:
        void (*_f)();

      public:
        call0(void (*f)())
        {
            _f = f;
        }

        void operator()(u64 arg1, f64 *arg2, i64 arg3, f64 *arg4, i64 arg5)
        {
	    PROC.X(0).u() = arg1;
	    PROC.X(1).u() = (word*)arg2 - &(MEM[PROC.RA().u()*256]);
	    PROC.X(2).i() = arg3;
	    PROC.X(3).u() = (word*)arg4 - &(MEM[PROC.RA().u()*256]);
	    PROC.X(4).i() = arg5;

	    _f();
	}

        void operator()(u64 arg1, c128 *arg2, i64 arg3, c128 *arg4, i64 arg5)
        {
	    PROC.X(0).u() = arg1;
	    PROC.X(1).u() = (word*)arg2 - &(MEM[PROC.RA().u()*256]);
	    PROC.X(2).i() = arg3;
	    PROC.X(3).u() = (word*)arg4 - &(MEM[PROC.RA().u()*256]);
	    PROC.X(4).i() = arg5;

	    _f();
	}
    };

    template <typename T1, typename T2, typename T3, typename T4, typename T5> class call5
    {
      private:
        void (*_f)(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5);

      public:
        call5(void (*f)(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5))
        {
            _f = f;
        }
        void operator()(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
        {
	    _f(arg1, arg2, arg3, arg4, arg5);
	}
    };

    call0 Call(void (*f)());

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    call5<T1, T2, T3, T4, T5> Call(void (*f)(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5))
    {
        return call5<T1, T2, T3, T4, T5>(f);
    }

    class instruction					// Generic instruction class
    {
	protected:
	    u32	_line;					// line number of instruction in source file
	    u32 _addr;					// byte (not word) address of instruction in memory
	public:
	    virtual bool execute() = 0;			// every instruction must have a method "execute" that implements its semantics and returns "true" if branch is taken
	    virtual u08 len() const = 0;		// length of instruction in bytes (2 or 4)
	    virtual string mnemonic() const = 0;	// mnemonic for the instruction
	    virtual string dasm() const = 0;		// disassembly for the instruction
	    virtual u32 encoding() const = 0;		// instruction encoding
	    u32& line() { return _line; }
	    u32& addr() { return _addr; }
    };

    class Fijk : public instruction	// Instructions of 4/4/4/4 format
    {
	protected:
	    u08 _F;	// 4-bit function code
	    u08 _i;	// 4-bit i field
	    u08 _j;	// 4-bit j field
	    u08 _k;	// 4-bit k field
	public:
	    Fijk(u08 F, u08 i, u08 j, u08 k)
	    {
		assert(F < 16);
		assert(i < 16);
		assert(j < 16);
		assert(k < 16);
		_F = F;
		_i = i;
		_j = j;
		_k = k;
	    }
	    u08 len() const { return 2; }
	    string dasm() const { return mnemonic() + "(" + to_string(_i) + ", " + to_string(_j) + ", " + to_string(_k) + ")"; }
	    u32 encoding() const { return (_F << 12) + (_i << 8) + (_j << 4) + (_k << 0); }
    };

    class FijK : public instruction	// Instructions of 4/4/4/20 format
    {
	protected:
	    u08	_F;	// 4-bit function code
	    u08 _i;	// 4-bit i field
	    u08 _j;	// 4-bit j field
	    u32 _K;	// 20-bit K field
	public:
	    FijK(u08 F, u08 i, u08 j, u32 K)
	    {
		assert(F < 16);
		assert(i < 16);
		assert(j < 16);
		assert(K < (1 << 20));
		_F = F;
		_i = i;
		_j = j;
		_K = K;
	    }
	    u08 len() const { return 4; }
	    string dasm() const { return mnemonic() + "(" + to_string(_i) + ", " + to_string(_j) + ", " + to_string(_K) + ")"; }
	    u32 encoding() const { return (_F << 28) + (_i << 24) + (_j << 20) + (_K << 0); }
    };

    class Fjn : public instruction	// Instructions of 6/4/6 format
    {
	protected:
	    u08 _F;	// 6-bit function code
	    u08 _j;	// 4-bit j field
	    u08 _n;	// 6-bit n field
	public:
	    Fjn(u08 F, u08 j, u08 n)
	    {
		assert(F < 64);
		assert(j < 16);
		assert(n < 64);
		_F = F;
		_j = j;
		_n = n;
	    }
	    u08 len() const { return 2; }
	    string dasm() const { return mnemonic() + "(" + to_string(_j) + ", " + to_string(_n) + ")"; }
    };

    class Fjk : public instruction	// Instructions of 8/4/4 format
    {
	protected:
	    u08	_F;	// 8-bit Function code
	    u08	_j;	// 4-bit j field
	    u08 _k;	// 4-bit k field
	public:
	    Fjk(u08 F, u08 j, u08 k)
	    {
		assert(F < 256);
		assert(j < 16);
		assert(k < 16);
		_F = F;
		_j = j;
		_k = k;
	    }
	    u08 len() const { return 2; }
	    string dasm() const { return mnemonic() + "(" + to_string(_j) + ", " + to_string(_k) + ")"; }
	    u32 encoding() const { return (_F << 8) + (_j << 4) + (_k << 0); }
    };

    class FjK : public instruction	// Instructions of 8/4/20 fomrat
    {
	protected:
	    u08 _F;	// 8-bit Function code
	    u08 _j;	// 4-bit j field
	    u32 _K;	// 20-bit K field
	public:
	    FjK(u08 F, u08 j, u32 K)
	    {
		assert(F < 256);
		assert(j < 16);
		assert(K < (1 << 20));
		_F = F;
		_j = j;
		_K = K;
	    }
	    u08 len() const { return 4; }
	    string dasm() const { return mnemonic() + "(" + to_string(_j) + ", " + to_string(_K) + ")"; }
	    u32 encoding() const { return (_F << 24) + (_j << 20) + (_K << 0); }
    };

    namespace instructions
    {
#include<jmp.hh>				// Jump to P+K                                                  (p86)
#include<jmpz.hh>				// Jump to P + K if (Xj) equal to 0                             (p94)
#include<jmpp.hh>				// Jump to P + K if (Xj) positive                               (p98)
#include<jmpk.hh>				// Subroutine exit, computed jump to (Xj) + k                   (p110)
#include<xkj.hh>				// Transmit k to Xj                                             (p55)
#include<compk.hh>				// Copy complement of (Xk) to Xj 				(p41)
#include<isjki.hh>				// Integer sum of (Xj) plus (Xk) to Xi				(p122)
#include<ipjkj.hh>				// Integer product of (Xj) times (Xk) to Xj 			(p52)
#include<idjkj.hh>				// Integer difference of (Xj) minus k to Xj 			(p58)
#include<isjkj.hh>				// Integer sum of (Xj) plus k to Xj 				(p57)
#include<idzkj.hh>				// Integer difference of zero minus (Xk) to Xj 			(p62)
#include<rdKj.hh>				// Read data at address K to Xj					(p74)
#include<rdjki.hh>				// Read data at address (Xj) + (Xk) to (Xi)			(p133)
#include<sdjki.hh>				// Store data at address (Xj) + (Xk) from Xi			(p135)
    } // namespace instructions

    namespace instructions
    {
	extern u32  count;	// Current instruction count
	extern bool target;	// Is the current instruction the target of a branch?
    };

    extern vector<instruction*>	trace;

    extern bool process(instruction*, u32);

    extern void dump(vector<instruction*>&);

} // namespace CDC8600

#endif // _CDC8600_HH_
