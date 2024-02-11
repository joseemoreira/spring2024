#ifndef _OPERATIONS_HH_
#define _OPERATIONS_HH_

using namespace std;

namespace CDC8600
{
    namespace operations
    {
	extern u64 		count;		// operation count
	extern u64 		nextdispatch;	// next dispatch cycle
	extern vector<u64>	MEMready;	// ready cycles for memory locations
	extern vector<u64>	REGready;	// ready cycles for registers

	class operation
	{
	     private:
		 u64 _count;	// operation #
		 u64 _ready;    // inputs ready
                 u64 _issue;    // issue time
                 u64 _complete; // completion time (output ready)

	     public:
		 operation() { }

		 virtual    u64 ready() const = 0; 		// time inputs are ready
		 virtual    u64 target(u64 cycle) = 0;		// update ready time of output
		 virtual    u64 latency() const = 0; 		// operation latency
		 virtual    u64 throughput() const = 0; 	// operation inverse throughput
		 virtual string mnemonic() const = 0;		// operation mnemonic
		 virtual string dasm() const  = 0;		// operation disassembly

		 virtual   void dump(ostream &out)		// operation trace
		 {
		     ios	state(nullptr);
		     state.copyfmt(out);
		     out << setw(80) << setfill(' ');
		     out << " | " << setw( 9) << setfill(' ') << _count;
		     out << " | " << setw(24) << setfill(' ') << dasm();
		     out << " | " << setw( 9) << setfill(' ') << _ready;
		     out << " | " << setw( 9) << setfill(' ') << _issue;
		     out << " | " << setw( 9) << setfill(' ') << _complete;
		     out << endl;
		     out.copyfmt(state);
		 }

		 virtual   bool process(u64 dispatch)		// process this operation
		 {
		     _count = count;				// current instruction count
		     _ready = ready();				// check ready time for inputs
		     _issue = max(_ready, dispatch);		// account for operation dispatch
		     _complete = _issue + latency();		// time operation completes
		     target(_complete);				// update time output is ready
		     if (tracing) dump(cout);			// generate operation trace
		 }
	};

	bool process(operation *op);

	class xkj : public operation
	{
	    private:
	        u08 _j;
	        u08 _k;

	    public:
		xkj(u08 j, u08 k) : operation() { _j = j; _k = k; }
		u64 ready() const { return 0; }
		u64 target(u64 cycle) { REGready[_j] = cycle; }
		u64 latency() const { return 2; }
		u64 throughput() const { return 1; }
		string mnemonic() const { return "xkj"; }
		string dasm() const { return mnemonic() + "(" + to_string(_j) + ", " + to_string(_k) + ")"; }
	};

	class idzkj : public operation
	{
	    private:
		u08	_j;
		u08	_k;

	    public:
		idzkj(u08 j, u08 k) : operation() { _j = j; _k = k; }
		u64 ready() const { return REGready[_k]; }
		u64 target(u64 cycle) { REGready[_j] = cycle; }
		u64 latency() const { return 2; }
		u64 throughput() const { return 1; }
		string mnemonic() const { return "idzkj"; }
		string dasm() const { return mnemonic() + "(" + to_string(_j) + ", " + to_string(_k) + ")"; }
	};

	class isjkj : public operation
	{
	    private:
		u08	_j;
		u08	_k;

	    public:
		isjkj(u08 j, u08 k) : operation() { _j = j; _k = k; }
		u64 ready() const { return max(REGready[_k], REGready[_j]); }
		u64 target(u64 cycle) { REGready[_j] = cycle; }
		u64 latency() const { return 2; }
		u64 throughput() const { return 1; }
		string mnemonic() const { return "isjkj"; }
		string dasm() const { return mnemonic() + "(" + to_string(_j) + ", " + to_string(_k) + ")"; }
	};

	class ipjkj : public operation
	{
	    private:
		u08	_j;
		u08	_k;

	    public:
		ipjkj(u08 j, u08 k) : operation() { _j = j; _k = k; }
		u64 ready() const { return max(REGready[_k], REGready[_j]); }
		u64 target(u64 cycle) { REGready[_j] = cycle; }
		u64 latency() const { return 2; }
		u64 throughput() const { return 1; }
		string mnemonic() const { return "ipjkj"; }
		string dasm() const { return mnemonic() + "(" + to_string(_j) + ", " + to_string(_k) + ")"; }
	};

	class cmpz : public operation
	{
	    private:
	        u08 _j;

	    public:
		cmpz(u08 j) : operation() { _j = j; }
		u64 ready() const { return REGready[_j]; }
		u64 target(u64 cycle) { REGready[params::micro::CMPFLAGS] = cycle; }
		u64 latency() const { return 1; }
		u64 throughput() const { return 1; }
		string mnemonic() const { return "cmpz"; }
		string dasm() const { return mnemonic() + "(" + to_string(_j) + ")"; }
	};

	class jmpp : public operation
	{
	    private:

	    public:
		jmpp() : operation() { }
		u64 ready() const { return REGready[params::micro::CMPFLAGS]; }
		u64 target(u64 cycle) { nextdispatch = cycle; }
		u64 latency() const { return 1; }
		u64 throughput() const { return 1; }
		string mnemonic() const { return "jmpp"; }
		string dasm() const { return mnemonic() + "(" + ")"; }
	};
    } // namespace operations
} // namespace CDC8600

#endif // _OPERATIONS_HH_
