#include<iostream>
#include<iomanip>
#include<CDC8600.hh>
#include<ISA.hh>

namespace CDC8600
{
    vector<word> 	MEM(params::MEM::N);
    uint32_t		FreeMEM;
    Processor		PROC;

    word& Processor::X
    (
        uint8_t	i
    )
    {
	return MEM[_XA*32 + i];
    }

    reg<20> Processor::P
    (
    )
    {
	return reg<20>(_XA*32+16, 0);
    }

    reg<12> Processor::FL
    (
    )
    {
	return reg<12>(_XA*32+16, 20);
    }

    reg<8> Processor::XA
    (
    )
    {
	return reg<8>(_XA*32+16, 32);
    }

    reg<12> Processor::RA
    (
    )
    {
	return reg<12>(_XA*32+16, 40);
    }

    reg<8> Processor::cond
    (
    )
    {
	return reg<8>(_XA*32+16, 52);
    }

    reg<4> Processor::mode
    (
    )
    {
	return reg<4>(_XA*32+16, 60);
    }

    template<int n> 
    u64	reg<n>::u()
    {
	u64 v = MEM[_loc].u();
	v = v >> _first;
	v = v & ((1UL << n) - 1);
	return v;
    }

    template<int n>
    reg<1> reg<n>::operator()
    (
        uint8_t bit
    )
    {
	assert(false);
    }

    template<int n>
    reg<n>& reg<n>::operator=
    (
        bool b
    )
    {
	assert(false);
    }

    template<int n>
    reg<n>& reg<n>::operator=
    (
        u64 u
    )
    {
	assert (u < (1UL << n));
	u64 v = MEM[_loc].u();
	u64 left = v >> (_first + n);
	u64 right = v & ((1UL << _first) - 1);
	MEM[_loc].u() = (left << (_first + n)) + (u << _first) + right;
	return *this;
    }

    namespace L1
    {
	cache::cache
	(
	) : _sets(params::L1::nsets)
	{
	}

	void cache::reset()
	{
	    for (u32 i=0; i < params::L1::nsets; i++) _sets[i].reset();
	}

	cacheset::cacheset
	(
	) : _valid(params::L1::nways), _tag(params::L1::nways), _used(params::L1::nways)
	{
	}

	void cacheset::reset()
	{
	    for (u32 i=0; i < params::L1::nways; i++) _valid[i] = false;
	}

	bool cache::loadhit
	(
	    u32 addr,
	    u64 cycle
	)
	{
	    u32 lineaddr = addr / params::L1::linesize;
	    cacheset &candidate = _sets[lineaddr % params::L1::nsets];
	    for (u32 i=0; i < params::L1::nways; i++)
		if (candidate.valid(i) && (candidate.tag(i) == lineaddr)) { candidate.used(i) = cycle; return true; } // hit

	    // must allocate on a load miss
	    for (u32 i=0; i < params::L1::nways; i++)
	    {
		if (!candidate.valid(i))
		{
		    // invalid entry, can use this one
		    candidate.validate(i);
		    candidate.tag(i) = lineaddr;
		    candidate.used(i) = cycle;
		    return false;
		}
	    }

	    // no invalid entry in this set, must find the LRU
	    u32 lruentry = 0;
	    u64 lrutime = UINT64_MAX;
	    for (u32 i=0; i < params::L1::nways; i++)
	    {
		if (candidate.used(i) < lrutime)
		{
		    lrutime = candidate.used(i);
		    lruentry = i;
		}
	    }
	    candidate.tag(lruentry) = lineaddr;
	    candidate.used(lruentry) = cycle;
	    return false;
	}
	
	bool cache::storehit
	(
	    u32 addr,
	    u64 cycle
	)
	{
	    u32 lineaddr = addr / params::L1::linesize;
	    cacheset &candidate = _sets[lineaddr % params::L1::nsets];
	    for (u32 i=0; i < params::L1::nways; i++)
		if (candidate.valid(i) && (candidate.tag(i) == lineaddr)) { candidate.used(i) = cycle; return true; } // hit
    	    // do not allocate on store miss
	    return false;
	}
    }

    void reset
    (
    )
    {
	for (u32 i = 0; i < params::MEM::N; i++) MEM[i].u() = 0;			// Zero the memory
	for (u32 i = 0; i < params::MEM::N; i++) operations::MEMready[i] = 0;		// Zero the ready time for all`memory locations
	PROC.REGready.resize(params::micro::nregs); for (u32 i = 0; i < params::micro::nregs; i++) PROC.REGready[i] = 0;	// Zero the ready time for all microarchitected registers
	FreeMEM = 4*8192;								// Heap starts in page 4
	PROC._XA = 4;									// User context for PROC[0] is in frame 4
	PROC.FL() = (u64)(29 * 8192 / 256);						// User data memory is 29 pages
	PROC.RA() = (u64)( 3 * 8192 / 256);						// User data memory begins in page 3
	PROC.instr_count = 0;								// Instruction count starts at 0
	PROC.instr_target = true;							// First instruction is target of a branch
	PROC.instr_forcealign = 0;							// Force instruction address to align to word boundary
	PROC.op_count = 0;								// Operation count starts at 0
	PROC.op_nextdispatch = 0;							// Start dispatching operations at cycle 0
	PROC.op_maxcycle = 0;								// Maximum completion time observed
	for (u32 i=0; i<PROC.trace.size(); i++) delete PROC.trace[i];			// Delete all previous instructions
	PROC.trace.clear();								// Clear the trace
	PROC.line2addr.clear();								// Clear line -> address map
	PROC.line2encoding.clear();							// Clear line -> encoding map
	PROC.line2len.clear();								// Clear line -> len map
	PROC.BRUs.resize(params::micro::nBRUs); for (u32 i=0; i < params::micro::nBRUs; i++) PROC.BRUs[i].clear();		// Cluear usage of BRUs
	PROC.FXUs.resize(params::micro::nFXUs); for (u32 i=0; i < params::micro::nFXUs; i++) PROC.FXUs[i].clear();		// Cluear usage of FXUs
	PROC.FPUs.resize(params::micro::nFPUs); for (u32 i=0; i < params::micro::nFPUs; i++) PROC.FPUs[i].clear();		// Cluear usage of FPUs
	PROC.LDUs.resize(params::micro::nLDUs); for (u32 i=0; i < params::micro::nLDUs; i++) PROC.LDUs[i].clear();		// Cluear usage of LDUs
	PROC.STUs.resize(params::micro::nSTUs); for (u32 i=0; i < params::micro::nSTUs; i++) PROC.STUs[i].clear();		// Cluear usage of STUs
	PROC.L1D.reset();								// Reset the L1 data cache (all entries invalid)
	PROC.labeling = false;
    }

    void *memalloc
    (
        u64	N
    )
    {
	void *addr = &(MEM[FreeMEM]);
	FreeMEM += N;
	return addr;
    }

    call0 Call(void (*f)())
    {
	return call0(f);
    }

    template class reg<12>;
    template class reg<8>;
    template class reg<4>;
    template class reg<1>;
    template class reg<20>;

    bool			tracing = false;		// Trace during the simulation?

    void addlabel
    (
        string label,
	u32    line
    )
    {
	if (PROC.label2line.count(label)) return;
	PROC.label2line[label] = line;
    }

    template<typename T> void label
    (
        T (*f)()
    )
    {
	PROC.line2addr.clear();
	PROC.line2encoding.clear();
	PROC.line2len.clear();
	PROC.label2line.clear();
	PROC.labeling = true;
	PROC.runningaddr = 0;
	f();
	PROC.labeling = false;
    }
    
    template void label(void (*f)());
    template void label(f64  (*f)());
    template void label(i64  (*f)());
    template void label(c128 (*f)());

    void labeladdr
    (
        instruction*	instr
    )
    {
	if (0 == PROC.runningaddr) PROC.runningaddr = instr->line() * 8;
	if (PROC.line2addr.count(instr->line())) return;

	if (PROC.runningaddr % instr->len())
	{
		cout << "Instruction at line # " << instr->line() 
		     << " has length " << instr->len() << " bytes, but has a byte offset of "
		     << (PROC.runningaddr % 8) << endl;
		assert(false);
	}
	PROC.line2addr[instr->line()] = PROC.runningaddr;
	PROC.line2encoding[instr->line()] = instr->encoding();
	PROC.line2len[instr->line()] = instr->len();
	PROC.runningaddr += instr->len();
    }

    void assignaddr
    /*
     * Assign a byte (not word!) address to an instruction.
     */
    (
        instruction*	instr,		// pointer to instruction
	bool		target		// is this a target of a branch?
    )
    {
	if (PROC.line2addr.count(instr->line()))				// line already in map?
	{
	    assert(instr->encoding() == PROC.line2encoding[instr->line()]);	// encoding match?
	    assert(instr->len()      == PROC.line2len[instr->line()]);		// instruction length match?
	}
	else if (PROC.instr_forcealign)						// this line has a label, so we must force a word alignment
	{
	    PROC.line2addr[instr->line()] = instr->line() * 8;			// this is a byte address
	    PROC.line2encoding[instr->line()] = instr->encoding();
	    PROC.line2len[instr->line()]      = instr->len();
	    PROC.instr_forcealign = 0;
	}
	else if (PROC.line2addr.count(instr->line() - 1)) 			// is the previous line in the map?
	{
	    PROC.line2addr[instr->line()]     = PROC.line2addr[instr->line() - 1] + PROC.line2len[instr->line() - 1];
	    PROC.line2encoding[instr->line()] = instr->encoding();
	    PROC.line2len[instr->line()]      = instr->len();
	}
	else									// new line
	{
	    PROC.line2addr[instr->line()] = instr->line() * 8;			// this is a byte address
	    PROC.line2encoding[instr->line()] = instr->encoding();
	    PROC.line2len[instr->line()]      = instr->len();
	}

	if (target)								// is this the target of a branch
	{
	    if (PROC.line2addr[instr->line()] % 8)				// is the target address word aligned?
	    {
		cout << "Instruction at line # " << instr->line() << " is the target of a branch but has a byte offset of " << (PROC.line2addr[instr->line()] % 8) << endl;
		assert(false);
	    }
	}
    }

    void dumpheader
    (
    )
    {
	cout << "  instr #";
	cout << " |    line #";
	cout << " |                   instruction ";
	cout << " |  address";
	cout << " | encoding ";
	cout << endl;

	cout << "----------";
	cout << "+-----------";
	cout << "+--------------------------------";
	cout << "+----------";
	cout << "+---------";
	cout << endl;
    }

    void dumpheaderop
    (
    )
    {
	cout << "  instr #";
	cout << " |    line #";
	cout << " |             instruction ";
	cout << " |  address";
	cout << " | encoding ";
	cout << " |     op # ";
	cout << " |               operation ";
	cout << " | dispatch ";
	cout << " |    ready ";
	cout << " |    issue ";
	cout << " | complete ";
	cout << endl;

	cout << "----------";
	cout << "+-----------";
	cout << "+--------------------------";
	cout << "+----------";
	cout << "+-----------";
	cout << "+-----------";
	cout << "+--------------------------";
	cout << "+-----------";
	cout << "+-----------";
	cout << "+-----------";
	cout << "+----------";
	cout << endl;
    }

    void dump
    (
        u32		i,
        instruction* 	instr
    )
    {
	cout << setw( 9) << i;
	cout << " | " << setw( 9) << instr->line();
	cout << " | " << setw(24) << instr->dasm();
	cout << " | " << setfill('0') << setw( 8) << hex << PROC.line2addr[instr->line()] << dec << setfill(' ');
	if (instr->len() == 4) cout << " | "     << setfill('0') << setw(8) << hex << instr->encoding() << dec << setfill(' ');
	if (instr->len() == 2) cout << " |     " << setfill('0') << setw(4) << hex << instr->encoding() << dec << setfill(' '); 
	cout << endl;
    }

    void dump
    (
        vector<instruction*>& T
    )
    {
	dumpheader();

	for (u32 i=0; i<T.size(); i++)
	{
	    dump(i, T[i]);
	}
    }

    bool process
    (
        instruction* 	instr,
	u32 		line
    )
    {
	if (PROC.labeling)
	{
	    instr->line() = line;
	    labeladdr(instr);
	    delete instr;
	    return false;
	}
	instr->line() = line;				// save instruction line number in source file
	assignaddr(instr, PROC.instr_target);		// assign an address to this instruction
	instr->fixit();					// fix displacement in branches
	PROC.instr_target = instr->execute();		// execute the instructions, remember if a branch is being taken
	PROC.trace.push_back(instr);			// save instruction to trace
	if (tracing)					// run-time tracing
	{
	    if (0 == PROC.instr_count) dumpheaderop();
	    dump(PROC.instr_count, instr);
	}
	instr->ops();					// process the internal operations of this instruction
	PROC.instr_count++;				// increment instruction counter
	return PROC.instr_target;			// return true if a branch is taken
    }

    namespace operations
    {
	vector<u64>		MEMready(params::MEM::N);	// ready cycle for memory locations

	bool process
	(
	    operation* op
	)
	{
	    op->process(PROC.op_nextdispatch);					// process this operation
	    PROC.op_count++;						// update operation count
	    PROC.op_maxcycle = max(PROC.op_maxcycle, op->complete());	// update maximum observed completion time
	}
    } // namespace operations
} // namespace 8600
