/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_util_Equators_
#define _lucene_util_Equators_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

//#ifdef QT_LINUXBASE
// we are going to use qreal now, we basically maintain our own clucene anyway
//// LSB doesn't define float_t - see http://bugs.linuxbase.org/show_bug.cgi?id=2374
//typedef float float_t;
//#endif

CL_NS_DEF(util)

///////////////////////////////////////////////////////////////////////////////
// Equators
///////////////////////////////////////////////////////////////////////////////

class Equals{
public:
	class Int32:public CL_NS_STD(binary_function)<const int32_t*,const int32_t*,bool>
	{
	public:
		bool operator()( const int32_t val1, const int32_t val2 ) const;
	};
	
	class Char:public CL_NS_STD(binary_function)<const char*,const char*,bool>
	{
	public:
		bool operator()( const char* val1, const char* val2 ) const;
	};
#ifdef _UCS2
	class WChar: public CL_NS_STD(binary_function)<const wchar_t*,const wchar_t*,bool>
	{
	public:
		bool operator()( const wchar_t* val1, const wchar_t* val2 ) const;
	};
	class TChar: public WChar{
	};
#else
	class TChar: public Char{
	};
#endif

    template<typename _cl>
	class Void:public CL_NS_STD(binary_function)<const void*,const void*,bool>
	{
	public:
		bool operator()( _cl* val1, _cl* val2 ) const{
			return val1==val2;
		}
	};

    class Qstring : public CL_NS_STD(binary_function)<const QString&, const QString&, bool>
    {
    public:
        bool operator() (const QString& val1, const QString& val2) const;
    };
};


///////////////////////////////////////////////////////////////////////////////
// Comparors
///////////////////////////////////////////////////////////////////////////////

class Comparable : LUCENE_BASE
{
public:
   virtual ~Comparable(){
   }
   
	virtual int32_t compareTo(void* o) = 0;
};

/** @internal */
class Compare{
public:
	class _base
	{	// traits class for hash containers
	public:
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		_base()
		{
		}
	};

	class Int32:public _base, public Comparable{
		int32_t value;
	public:
		int32_t getValue() const;
		Int32(int32_t val);
		Int32();
		int32_t compareTo(void* o);
		bool operator()( int32_t t1, int32_t t2 ) const;
		size_t operator()( int32_t t ) const;
	};

	
	class Float:public Comparable{
		qreal value;
	public:
		qreal getValue() const;
		Float(qreal val);
		int32_t compareTo(void* o);
	};


	class Char: public _base //<char*>
	{
	public:
		bool operator()( const char* val1, const char* val2 ) const;
		size_t operator()( const char* val1) const;
	};

#ifdef _UCS2
	class WChar: public _base //<wchar_t*>
	{
	public:
		bool operator()( const wchar_t* val1, const wchar_t* val2 ) const;
		size_t operator()( const wchar_t* val1) const;
	};
#endif

	class TChar: public _base, public Comparable{
	  const TCHAR* s;
	public:
		const TCHAR* getValue() const;

		TChar();
		TChar(const TCHAR* str);
		int32_t compareTo(void* o);
		bool operator()( const TCHAR* val1, const TCHAR* val2 ) const;
		size_t operator()( const TCHAR* val1) const;
	};


	template<typename _cl>
	class Void:public _base //<const void*,const void*,bool>
	{
	public:
		int32_t compareTo(_cl* o){
			if ( this == o )
				return o;
			else
				return this > o ? 1 : -1;
		}
		bool operator()( _cl* t1, _cl* t2 ) const{
			return t1 > t2 ? true : false;
		}
		size_t operator()( _cl* t ) const{
			return (size_t)t;
		}
	};

	class Qstring : public _base
	{
	public:
		bool operator() (const QString& val1, const QString& val2) const;
		size_t operator() (const QString& val1) const;
	};
};

///////////////////////////////////////////////////////////////////////////////
// allocators
///////////////////////////////////////////////////////////////////////////////

class Deletor
{
public:

	template<typename _kt>
	class Array{
	public:
		static void doDelete(_kt* arr){
			_CLDELETE_LARRAY(arr);
		}
	};
	class tcArray{
	public:
		static void doDelete(const TCHAR* arr){
			_CLDELETE_CARRAY(arr);
		}
	};
	class acArray{
	public:
		static void doDelete(const char* arr){
			_CLDELETE_CaARRAY(arr);
		}
	};

	class Unintern{
	public:
		static void doDelete(TCHAR* arr);
	};
	template<typename _kt>
	class Object{
	public:
		static void doDelete(_kt* obj){
			_CLLDELETE(obj);
		}
	};
	template<typename _kt>
	class Void{
	public:
		static void doDelete(_kt* obj){
			_CLVDELETE(obj);
		}
	};
	class Dummy{
	public:
		static void doDelete(const void* nothing)
        {
			// TODO: remove all occurances where it hits this point
			// CND_WARNING(false, "Deletor::Dummy::doDelete run, set deleteKey
            // or deleteValue to false");
		}
	};
	class DummyInt32{
	public:
		static void doDelete(const int32_t nothing){
		}
	};
	class DummyFloat{
	public:
		static void doDelete(const qreal nothing){
		}
	};
	template <typename _type>
	class ConstNullVal{
	public:
		static void doDelete(const _type nothing)
        {
			// TODO: remove all occurances where it hits this point
			// CND_WARNING(false, "Deletor::Dummy::doDelete run, set deleteKey
            // or deleteValue to false");
		}
	};
	
	template <typename _type>
	class NullVal{
	public:
		static void doDelete(_type nothing)
        {
			// TODO: remove all occurances where it hits this point
			// CND_WARNING(false, "Deletor::Dummy::doDelete run, set deleteKey
            // or deleteValue to false");
		}
	};
	class DummyQString {
	public:
		static void doDelete(const QString& nothing) {
		}
	};
};
////////////////////////////////////////////////////////////////////////////////

CL_NS_END
#endif
