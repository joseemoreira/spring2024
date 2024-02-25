class jmpnz : public FjK
{
    private:
	string	_label;
    
    public:
	jmpnz(u08 j, string L) : FjK(0x35, j, 0) { _label = L; }

	bool execute()
	{
	    if (0 != PROC.X(_j).u()) return true;
            else return false;
	}

	string mnemonic() const
	{
	    return "jmpnz";
	}

        bool ops()
	{
	    process(new operations::cmpz(_j));
	    process(new operations::jmpnz());
	    return false;
	}

	void fixit()
	{
	    assert(PROC.label2line.count(_label));
	    u32 targetline = PROC.label2line[_label];
	    assert(PROC.line2addr.count(targetline));
	    u32 targetaddr = PROC.line2addr[targetline];
	    assert(PROC.line2addr.count(_line));
	    u32 sourceaddr = PROC.line2addr[_line];
	    _K = ((targetaddr/8) - (sourceaddr/8)) & 0xfffff;
	}
};
