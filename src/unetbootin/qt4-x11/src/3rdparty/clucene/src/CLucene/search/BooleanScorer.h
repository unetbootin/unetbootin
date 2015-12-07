/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#ifndef _lucene_search_BooleanScorer_
#define _lucene_search_BooleanScorer_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Scorer.h"

CL_NS_DEF(search)

class BooleanScorer : public Scorer {
public:
    class Bucket : LUCENE_BASE {
    public:
        int32_t	doc;				  // tells if bucket is valid
        qreal	score;				  // incremental score
        int32_t	bits;				  // used for bool constraints
        int32_t	coord;				  // count of terms in score
        Bucket*	next;				  // next valid bucket

        Bucket();
        ~Bucket();
    };

    class SubScorer: LUCENE_BASE {
    public:
        bool done;
        Scorer* scorer;
        bool required;
        bool prohibited;
        HitCollector* collector;
        SubScorer* next;
        SubScorer(Scorer* scr, const bool r, const bool p, HitCollector* c, SubScorer* nxt);
        ~SubScorer();
    };

    class BucketTable:LUCENE_BASE {		
    private:
        BooleanScorer* scorer;
    public:
        Bucket* buckets;
        Bucket* first;			  // head of valid list

        BucketTable(BooleanScorer* scr);
        int32_t size() const;
        HitCollector* newCollector(const int32_t mask);
        void clear();
        ~BucketTable();

    };

    class Collector: public HitCollector {
    private:
        BucketTable* bucketTable;
        int32_t mask;
    public:
        Collector(const int32_t mask, BucketTable* bucketTable);

        void collect(const int32_t doc, const qreal score);
    };

    SubScorer* scorers;
    BucketTable* bucketTable;

    int32_t maxCoord;
    int32_t nextMask;

    int32_t end;
    Bucket* current;

public:
    LUCENE_STATIC_CONSTANT(int32_t,BucketTable_SIZE=1024);
    int32_t requiredMask;
    int32_t prohibitedMask;
    qreal* coordFactors;

    BooleanScorer(Similarity* similarity);
    ~BooleanScorer();
    void add(Scorer* scorer, const bool required, const bool prohibited);
    int32_t doc() const { return current->doc; }
    bool next();
    qreal score();
    bool skipTo(int32_t target);
    void explain(int32_t doc, Explanation* ret);
    TCHAR* toString();
    void computeCoordFactors();
};

CL_NS_END
#endif
