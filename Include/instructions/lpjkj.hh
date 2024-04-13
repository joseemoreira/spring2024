class lpjkj : public Fjk
{
    public:
	lpjkj(u08 j, u08 k) : Fjk(0x01, j, k) {}
	lpjkj() : Fjk(0x01, 0, 0) {}

	bool execute()
	{
		stringstream ss;
		ss << setfill('0') << setw(16) << hex << PROC[me()].X(_j).u() <<
			" & " << setw(16) << PROC[me()].X(_k).u();
	    PROC[me()].X(_j).u() = PROC[me()].X(_j).u() & PROC[me()].X(_k).u();
    	ss << " = " << setw(16) << PROC[me()].X(_j).u() << dec << setfill(' ');
    	_trace = ss.str();
		return false;
	}

	bool ops()
	{
	    operations::process<operations::lpjkj>(_j, _j, 0, _k);
	    return false;
	}


	bool match(u08 F)
	{
		if (0x01 == F) return true;
		return false;
	}

	void decode(u32 code)
        {
	    assert(code < 65536);       // 16-bit instruction
	    assert(match(code >> 8));   // we are in the right instruction
	    _j = (code >> 4) & 0xf;     // extract j
	    _k = code  & 0xf;           // extract k
	}

	string mnemonic() const
	{
	    return "lpjkj";
	}

	vector<operations::operation*> crack()
	{
		vector<operations::operation*>	ops;
		ops.push_back(new operations::lpjkj(_j, _j, 0, _k));
		return ops;
	}
};