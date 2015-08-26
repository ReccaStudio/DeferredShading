#pragma once

template<typename RefType>
class TRefCountPtr
{
	typedef RefType* RefTypePtr;
public:

protected:
	RefType* ReferenceV;
};

class RefCountObj
{
public:
	RefCountObj() : m_NumRefs(0)
	{

	}
	virtual ~RefCountObj() { }

	unsigned int AddRef() const
	{
		return unsigned int(++m_NumRefs);
	}

	unsigned int Release() const
	{
		unsigned int refs = unsigned int(--m_NumRefs);
		if (0 == refs)
		{
			delete this;
		}
		return refs;
	}

	unsigned int GetRefCount() const
	{
		return unsigned int(m_NumRefs);
	}

private:
	mutable int m_NumRefs;
};