#include <CDC8600.hh>
#include <ISA.hh>
#include <blas/dcopy.hh>

namespace CDC8600
{
    namespace BLAS
    {
        void dcopy(u64 n, f64 *x, i64 incx, f64 *y, i64 incy)
        {
            Call(dcopy_cpp)(n, x, incx, y, incy);
        }

        void dcopy_cpp(u64 n, f64 *x, i64 incx, f64 *y, i64 incy)
        {
	    i64 ix = 0;
	    i64 iy = 0;
	    if (incx < 0) ix = (-n+1)*incx;
	    if (incy < 0) iy = (-n+1)*incy;
	    while (n != 0)
	    {
		y[iy] = x[ix];
		ix += incx;
		iy += incy;
		n--;
	    }
        }

	void dcopy_asm
	(
	    // u64 n,		[ X0 ]
	    // f64 *x,		[ X1 ]
	    // i64 incx,	[ X2 ]
	    // f64 *y,		[ X3 ]
	    // i64 incy		[ X4 ]
	)
	{
	    xkj(5, 0)		// X5 (ix) = 0
	    xkj(6, 0)		// X6 (iy) = 0
LABEL(loop) jmpz(0, end)	// if X0 (n) = 0 goto end
            rdjki(7, 1, 5)	// X7 (tmp) = MEM[X1 (x) + X5 (ix)]
	    sdjki(7, 3, 6)	// MEM[X3 (y) + X6 (iy)]=X7 (tmp)
            isjki(5, 5, 2)	// X5 (ix) = X5 (ix) + X2 (incx)
	    isjki(6, 6, 4)	// X6 (iy) = X6 (iy) + X4 (incy)
	    idjkj(0, 1)		// X0 (n) = X0 (n) - 1
            jmp(loop)
LABEL(end)  jmpk(15, 1)		// return to X15 (calling address) + 1
	}
    } // namespace BLAS
} // namespace CDC8600
