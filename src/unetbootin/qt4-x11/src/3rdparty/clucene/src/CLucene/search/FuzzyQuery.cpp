/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "FuzzyQuery.h"

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)

	/**
     * Constructor for enumeration of all terms from specified <code>reader</code> which share a prefix of
     * length <code>prefixLength</code> with <code>term</code> and which have a fuzzy similarity &gt;
     * <code>minSimilarity</code>. 
     * 
     * @param reader Delivers terms.
     * @param term Pattern term.
     * @param minSimilarity Minimum required similarity for terms from the reader. Default value is 0.5f.
     * @param prefixLength Length of required common prefix. Default value is 0.
     * @throws IOException
     */
	 FuzzyTermEnum::FuzzyTermEnum(const IndexReader* reader, Term* term, qreal minSimilarity, size_t prefixLength): 
        distance(0),
        _endEnum(false),
		prefix(LUCENE_BLANK_STRING),
		prefixLength(0),
		minimumSimilarity(minSimilarity)
	{
	//Func - Constructor
	//Pre  - reader contains a valid reference to an IndexReader
	//       term != NULL
	//Post - The instance has been created

		CND_PRECONDITION(term != NULL,"term is NULL");
		
		scale_factor = 1.0f / (1.0f - minimumSimilarity);
		searchTerm = _CL_POINTER(term);
		
		text = STRDUP_TtoT(term->text());
		textLen = term->textLength();
		
		
		//Initialize e to NULL
		e          = NULL;
		eWidth     = 0;
		eHeight    = 0;
		
		if(prefixLength > 0 && prefixLength < textLen){
			this->prefixLength = prefixLength;
		
			prefix = _CL_NEWARRAY(TCHAR,prefixLength+1);
			_tcsncpy(prefix,text,prefixLength);
			prefix[prefixLength]='\0';
		
			textLen = prefixLength;
			text[textLen]='\0';
		}
		
		
		//Set the enumeration 
		Term* trm = _CLNEW Term(term, prefix);
		setEnum(reader->terms(trm));
		_CLDECDELETE(trm);
  }

  FuzzyTermEnum::~FuzzyTermEnum(){
  //Func - Destructor
  //Pre  - true
  //Post - FuzzyTermEnum has been destroyed

	  //Close the enumeration
	  close();
  }

  bool FuzzyTermEnum::endEnum() {
  //Func - Returns the fact if the current term in the enumeration has reached the end
  //Pre  - true
  //Post - The boolean value of endEnum has been returned

      return _endEnum;
  }

  void FuzzyTermEnum::close(){
  //Func - Close the enumeration
  //Pre  - true
  //Post - The enumeration has been closed

      FilteredTermEnum::close();
	  
      //Finalize the searchTerm
      _CLDECDELETE(searchTerm);
	  //Destroy e
      _CLDELETE_ARRAY(e);

	  _CLDELETE_CARRAY(text);

	  if ( prefix != LUCENE_BLANK_STRING )
		  _CLDELETE_CARRAY(prefix);
  }

  bool FuzzyTermEnum::termCompare(Term* term) {
  //Func - Compares term with the searchTerm using the Levenshtein distance.
  //Pre  - term is NULL or term points to a Term
  //Post - if pre(term) is NULL then false is returned otherwise
  //       if the distance of the current term in the enumeration is bigger than the FUZZY_THRESHOLD
  //       then true is returned 
	  
	  if (term == NULL){
		  return false;  //Note that endEnum is not set to true!
	  }

	  const TCHAR* termText = term->text();
	  size_t termTextLen = term->textLength();

		  //Check if the field name of searchTerm of term match
		  //(we can use == because fields are interned)
      if ( searchTerm->field() == term->field() && 
		  	(prefixLength==0 || _tcsncmp(termText,prefix,prefixLength)==0 )) {

			const TCHAR* target = termText+prefixLength;
			size_t targetLen = termTextLen-prefixLength;

		    //Calculate the Levenshtein distance
			int32_t dist = editDistance(text, target, textLen, targetLen);
			distance = 1 - ((qreal)dist / (qreal)min(textLen, targetLen));
			return (distance > minimumSimilarity);
      }
		_endEnum = true;
		return false;
  }

  qreal FuzzyTermEnum::difference() {
  //Func - Returns the difference between the distance and the fuzzy threshold
  //       multiplied by the scale factor
  //Pre  - true
  //Post - The difference is returned

     return (qreal)((distance - minimumSimilarity) * scale_factor );
  }
  
  
	/** Finds and returns the smallest of three integers 
		precondition: Must define int32_t __t for temporary storage and result
	*/
	#define min3(a, b, c) __t = (a < b) ? a : b; __t = (__t < c) ? __t : c;

  int32_t FuzzyTermEnum::editDistance(const TCHAR* s, const TCHAR* t, const int32_t n, const int32_t m) {
  //Func - Calculates the Levenshtein distance also known as edit distance is a measure of similiarity
  //       between two strings where the distance is measured as the number of character
  //       deletions, insertions or substitutions required to transform one string to
  //       the other string.
  //Pre  - s != NULL and contains the source string
  //       t != NULL and contains the target string
  //       n >= 0 and contains the length of the source string
  //       m >= 0 and containts the length of th target string
  //Post - The distance has been returned

      CND_PRECONDITION(s != NULL, "s is NULL");
      CND_PRECONDITION(t != NULL, "t is NULL");
	  CND_PRECONDITION(n >= 0," n is a negative number");
	  CND_PRECONDITION(n >= 0," n is a negative number");

      int32_t i;     // iterates through s
      int32_t j;     // iterates through t
      TCHAR s_i; // ith character of s

      if (n == 0) 
          return m;
      if (m == 0) 
          return n;

	//Check if the array must be reallocated because it is too small or does not exist
    if (e == NULL || eWidth <= n || eHeight <= m) {
        //Delete e if possible
        _CLDELETE_ARRAY(e);
        //resize e
		eWidth  = max(eWidth, n+1);
        eHeight = max(eHeight, m+1);
        e = _CL_NEWARRAY(int32_t,eWidth*eHeight);
    }
    
    CND_CONDITION(e != NULL,"e is NULL");

    // init matrix e
	for (i = 0; i <= n; i++){
        e[i + (0*eWidth)] = i;
    }
	for (j = 0; j <= m; j++){
        e[0 + (j*eWidth)] = j;
    }

	int32_t __t; //temporary variable for min3

    // start computing edit distance
    for (i = 1; i <= n; i++) {
        s_i = s[i - 1];
        for (j = 1; j <= m; j++) {
			if (s_i != t[j-1]){
				min3(e[i + (j*eWidth) - 1], e[i + ((j-1)*eWidth)], e[i + ((j-1)*eWidth)-1]);
                e[i + (j*eWidth)] = __t+1;
			}else{
				min3(e[i + (j*eWidth) -1]+1, e[i + ((j-1)*eWidth)]+1, e[i + ((j-1)*eWidth)-1]);
                e[i + (j*eWidth)] = __t;
			}
        }
    }

    // we got the result!
    return e[n + ((m)*eWidth)];
  }


  /**
   * Create a new FuzzyQuery that will match terms with a similarity 
   * of at least <code>minimumSimilarity</code> to <code>term</code>.
   * If a <code>prefixLength</code> &gt; 0 is specified, a common prefix
   * of that length is also required.
   * 
   * @param term the term to search for
   * @param minimumSimilarity a value between 0 and 1 to set the required similarity
   *  between the query term and the matching terms. For example, for a
   *  <code>minimumSimilarity</code> of <code>0.5</code> a term of the same length
   *  as the query term is considered similar to the query term if the edit distance
   *  between both terms is less than <code>length(term)*0.5</code>
   * @param prefixLength length of common (non-fuzzy) prefix
   * @throws IllegalArgumentException if minimumSimilarity is &gt; 1 or &lt; 0
   * or if prefixLength &lt; 0 or &gt; <code>term.text().length()</code>.
   */
  FuzzyQuery::FuzzyQuery(Term* term, qreal minimumSimilarity, size_t prefixLength):
	MultiTermQuery(term)
  {
  //Func - Constructor
  //Pre  - term != NULL
  //Post - The instance has been created

	  CND_PRECONDITION(term != NULL,"term is NULL");

	    if (minimumSimilarity > 1.0f)
		  _CLTHROWA(CL_ERR_IllegalArgument,"minimumSimilarity > 1");
        else if (minimumSimilarity < 0.0f)
		  _CLTHROWA(CL_ERR_IllegalArgument,"minimumSimilarity < 0");
    
	    this->minimumSimilarity = minimumSimilarity;
    
		if(prefixLength >= term->textLength())
			_CLTHROWA(CL_ERR_IllegalArgument,"prefixLength >= term.textLength()");
		this->prefixLength = prefixLength;

    }
  
  
    qreal FuzzyQuery::defaultMinSimilarity = 0.5f;

    FuzzyQuery::~FuzzyQuery(){
    //Func - Destructor
	//Pre  - true
	//Post - Instance has been destroyed
    }

    TCHAR* FuzzyQuery::toString(const TCHAR* field) const{
	//Func - Returns the query string
	//Pre  - field != NULL
	//Post - The query string has been returned

        CND_PRECONDITION(field != NULL,"field is NULL");

        StringBuffer buffer;
        const TCHAR* b = MultiTermQuery::toString(field);
    
        buffer.append ( b );
       _CLDELETE_CARRAY(b);
        buffer.append( _T("~") );

		buffer.appendFloat(minimumSimilarity,1);

        return buffer.toString();
    }

  const TCHAR* FuzzyQuery::getQueryName() const{
  //Func - Returns the name of the query
  //Pre  - true
  //post - The string FuzzyQuery has been returned

     return getClassName();
  }
  const TCHAR* FuzzyQuery::getClassName(){
  //Func - Returns the name of the query
  //Pre  - true
  //post - The string FuzzyQuery has been returned

     return _T("FuzzyQuery");
  }


  /**
   * Returns the minimum similarity that is required for this query to match.
   * @return float value between 0.0 and 1.0
   */
  qreal FuzzyQuery::getMinSimilarity() const {
    return minimumSimilarity;
  }

  FuzzyQuery::FuzzyQuery(const FuzzyQuery& clone):
		MultiTermQuery(clone)
	{
	  this->minimumSimilarity = clone.getMinSimilarity();
	  this->prefixLength = clone.getPrefixLength();
    
		//if(prefixLength < 0)
		//	_CLTHROWA(CL_ERR_IllegalArgument,"prefixLength < 0");
		//else 
		if(prefixLength >= clone.getTerm()->textLength())
			_CLTHROWA(CL_ERR_IllegalArgument,"prefixLength >= term.textLength()");

	}

  Query* FuzzyQuery::clone() const{
		return _CLNEW FuzzyQuery(*this);
	}
	size_t FuzzyQuery::hashCode() const{
		//todo: we should give the query a seeding value... but
		//need to do it for all hascode functions
		size_t val = Similarity::floatToByte(getBoost()) ^ getTerm()->hashCode();
		val ^= Similarity::floatToByte(this->getMinSimilarity());
		val ^= this->getPrefixLength();
		return val;
	}
	bool FuzzyQuery::equals(Query* other) const{
		if (!(other->instanceOf(FuzzyQuery::getClassName())))
			return false;

		FuzzyQuery* fq = (FuzzyQuery*)other;
		return (this->getBoost() == fq->getBoost())
			&& this->getMinSimilarity() == fq->getMinSimilarity()
			&& this->getPrefixLength() == fq->getPrefixLength()
			&& getTerm()->equals(fq->getTerm());
	}
    
  /**
   * Returns the prefix length, i.e. the number of characters at the start
   * of a term that must be identical (not fuzzy) to the query term if the query
   * is to match that term. 
   */
  size_t FuzzyQuery::getPrefixLength() const {
    return prefixLength;
  }

  FilteredTermEnum* FuzzyQuery::getEnum(IndexReader* reader){
	  Term* term = getTerm(false);
	  FuzzyTermEnum* ret = _CLNEW FuzzyTermEnum(reader, term, minimumSimilarity, prefixLength);
	  return ret;
  }

CL_NS_END
