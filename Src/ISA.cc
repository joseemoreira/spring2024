#include<CDC8600.hh>
#include<assert.h>

namespace CDC8600
{
   namespace instructions
   {   
        bool jmpz
	(
	    uint8_t Xj
	)
	{
	    assert(Xj < 16);
	    if (0 == PROC.X(Xj).u()) return true;
	    else return false;
	}

	bool jmpp
	(
	    uint8_t Xj
	)
	{
	    assert(Xj < 16);
	    if (0 < PROC.X(Xj).i()) return true;
	    else return false;
	}

        void xkj
	(
	    uint8_t Xj, 
	    uint8_t k
	)
	{
	    assert(Xj < 16);
	    assert(k < 16);
	    PROC.X(Xj).u() = k;
	}

        void rdjki
	(
	    uint8_t Xi, 
	    uint8_t Xj, 
	    uint8_t Xk
	)
	{
	    assert(Xi < 16);
	    assert(Xj < 16);
	    assert(Xk < 16);
	    int64_t addr = (PROC.X(Xj).i() + PROC.X(Xk).i()) & 0xfffff;
	    assert(PROC.FL().u() == 1024);
	    if (addr < PROC.FL().u()*256 )
	    {
		// Good
		assert(PROC.RA().u() == 0);
		addr += PROC.RA().u()*256;
		assert(addr >= 0);			    // Address should be nonnegative
		assert(addr < params::MEM::N);              // Check against hardware limit
		PROC.X(Xi) = MEM[addr];                     // Read data
	    }
	    else
	    {
		// Bad
		PROC._XA = PROC.XA().u();
		PROC.cond()(2) = true;
		assert(false);
	    }
	}

        void sdjki
	(
	    uint8_t Xi, 
	    uint8_t Xj, 
	    uint8_t Xk
	)
	{
	    MEM[PROC.X(Xj).i() + PROC.X(Xk).i()] = PROC.X(Xi);
	}

        void isjki
	(
	    uint8_t Xi, 
	    uint8_t Xj, 
	    uint8_t Xk
	)
	{
	    PROC.X(Xi).i() = PROC.X(Xj).i() + PROC.X(Xk).i();
	}
	
        void idjkj
	(
	    uint8_t Xj, 
	    uint8_t k
	)
	{
	    PROC.X(Xj).i() = PROC.X(Xj).i() - k;
	}

	void idzkj
	(
	   uint8_t Xj,
	   uint8_t Xk
	)
	{
	    PROC.X(Xj).i() = -PROC.X(Xk).i();
	}

	void isjkj
	(
	    uint8_t Xj,
	    uint8_t k
	)
	{
	    PROC.X(Xj).i() = PROC.X(Xj).i() + k;
	}

	void ipjkj
	(
	    uint8_t Xj,
	    uint8_t Xk
	)
	{
	    PROC.X(Xj).i() = PROC.X(Xj).i() * PROC.X(Xk).i();
	}

	void rdKj
	(
	    uint8_t Xj,
	    uint32_t K
	)
	{
	    assert(Xj < 16);
	    assert(K < 1024*1024);

	    if (K < PROC.FL().u()*256)
	    {
		// Good
		uint32_t addr = PROC.RA().u()*256 + K;	// Architected address
		assert(addr < params::MEM::N);		// Check against hardware limit
		PROC.X(Xj) = MEM[addr];
	    }
	    else
	    {
		// Bad
		PROC.cond()(2) = true;
		PROC._XA = PROC.XA().u();
	    }
	}
   } // namespace instructions
} // namespace CDC8600
