#include<CDC8600.hh>
#include<assert.h>

namespace CDC8600
{
   namespace instructions
   {   
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
