DEFINES += _BUILD_FOR_QT_ LUCENE_DISABLE_MEMTRACKING
win32:DEFINES += _CRT_SECURE_NO_DEPRECATE _MT

CLUCENEDIR = ../../../../src/3rdparty/clucene/src/CLucene

INCLUDEPATH += . .. \
               $$CLUCENEDIR \
               $$CLUCENEDIR/../ \
               $$CLUCENEDIR/analysis \
               $$CLUCENEDIR/analysis/standard \
               $$CLUCENEDIR/config \
               $$CLUCENEDIR/debug \
               $$CLUCENEDIR/document \
               $$CLUCENEDIR/index \
               $$CLUCENEDIR/queryParser \
               $$CLUCENEDIR/search \
               $$CLUCENEDIR/store \
               $$CLUCENEDIR/util


SOURCES += $$CLUCENEDIR/StdHeader.cpp \
           $$CLUCENEDIR/analysis/AnalysisHeader.cpp \
           $$CLUCENEDIR/analysis/Analyzers.cpp \
           $$CLUCENEDIR/config/gunichartables.cpp \
           $$CLUCENEDIR/config/repl_lltot.cpp \
           $$CLUCENEDIR/config/repl_tcscasecmp.cpp \
           $$CLUCENEDIR/config/repl_tcslwr.cpp \
           $$CLUCENEDIR/config/repl_tcstod.cpp \
           $$CLUCENEDIR/config/repl_tcstoll.cpp \
           $$CLUCENEDIR/config/repl_tprintf.cpp \
           $$CLUCENEDIR/config/threads.cpp \
           $$CLUCENEDIR/config/utf8.cpp \
           $$CLUCENEDIR/debug/condition.cpp \
           $$CLUCENEDIR/debug/error.cpp \
           $$CLUCENEDIR/debug/memtracking.cpp \
           $$CLUCENEDIR/document/DateField.cpp \
           $$CLUCENEDIR/document/Document.cpp \
           $$CLUCENEDIR/document/Field.cpp \
           $$CLUCENEDIR/index/CompoundFile.cpp \
           $$CLUCENEDIR/index/DocumentWriter.cpp \
           $$CLUCENEDIR/index/FieldInfos.cpp \
           $$CLUCENEDIR/index/FieldsReader.cpp \
           $$CLUCENEDIR/index/FieldsWriter.cpp \
           $$CLUCENEDIR/index/IndexModifier.cpp \
           $$CLUCENEDIR/index/IndexReader.cpp \
           $$CLUCENEDIR/index/IndexWriter.cpp \
           $$CLUCENEDIR/index/MultiReader.cpp \
           $$CLUCENEDIR/index/SegmentInfos.cpp \
           $$CLUCENEDIR/index/SegmentMergeInfo.cpp \
           $$CLUCENEDIR/index/SegmentMergeQueue.cpp \
           $$CLUCENEDIR/index/SegmentMerger.cpp \
           $$CLUCENEDIR/index/SegmentReader.cpp \
           $$CLUCENEDIR/index/SegmentTermDocs.cpp \
           $$CLUCENEDIR/index/SegmentTermEnum.cpp \
           $$CLUCENEDIR/index/SegmentTermPositions.cpp \
           $$CLUCENEDIR/index/SegmentTermVector.cpp \
           $$CLUCENEDIR/index/Term.cpp \
           $$CLUCENEDIR/index/TermInfo.cpp \
           $$CLUCENEDIR/index/TermInfosReader.cpp \
           $$CLUCENEDIR/index/TermInfosWriter.cpp \
           $$CLUCENEDIR/index/TermVectorReader.cpp \
           $$CLUCENEDIR/index/TermVectorWriter.cpp \
           $$CLUCENEDIR/queryParser/Lexer.cpp \
           $$CLUCENEDIR/queryParser/MultiFieldQueryParser.cpp \
           $$CLUCENEDIR/queryParser/QueryParser.cpp \
           $$CLUCENEDIR/queryParser/QueryParserBase.cpp \
           $$CLUCENEDIR/queryParser/QueryToken.cpp \
           $$CLUCENEDIR/queryParser/TokenList.cpp \
           $$CLUCENEDIR/search/BooleanQuery.cpp \
           $$CLUCENEDIR/search/BooleanScorer.cpp \
           $$CLUCENEDIR/search/CachingWrapperFilter.cpp \
           $$CLUCENEDIR/search/ChainedFilter.cpp \
           $$CLUCENEDIR/search/ConjunctionScorer.cpp \
           $$CLUCENEDIR/search/DateFilter.cpp \
           $$CLUCENEDIR/search/ExactPhraseScorer.cpp \
           $$CLUCENEDIR/search/Explanation.cpp \
           $$CLUCENEDIR/search/FieldCache.cpp \
           $$CLUCENEDIR/search/FieldCacheImpl.cpp \
           $$CLUCENEDIR/search/FieldDocSortedHitQueue.cpp \
           $$CLUCENEDIR/search/FieldSortedHitQueue.cpp \
           $$CLUCENEDIR/search/FilteredTermEnum.cpp \
           $$CLUCENEDIR/search/FuzzyQuery.cpp \
           $$CLUCENEDIR/search/HitQueue.cpp \
           $$CLUCENEDIR/search/Hits.cpp \
           $$CLUCENEDIR/search/IndexSearcher.cpp \
           $$CLUCENEDIR/search/MultiSearcher.cpp \
           $$CLUCENEDIR/search/MultiTermQuery.cpp \
           $$CLUCENEDIR/search/PhrasePositions.cpp \
           $$CLUCENEDIR/search/PhraseQuery.cpp \
           $$CLUCENEDIR/search/PhraseScorer.cpp \
           $$CLUCENEDIR/search/PrefixQuery.cpp \
           $$CLUCENEDIR/search/QueryFilter.cpp \
           $$CLUCENEDIR/search/RangeFilter.cpp \
           $$CLUCENEDIR/search/RangeQuery.cpp \
           $$CLUCENEDIR/search/SearchHeader.cpp \
           $$CLUCENEDIR/search/Similarity.cpp \
           $$CLUCENEDIR/search/SloppyPhraseScorer.cpp \
           $$CLUCENEDIR/search/Sort.cpp \
           $$CLUCENEDIR/search/TermQuery.cpp \
           $$CLUCENEDIR/search/TermScorer.cpp \
           $$CLUCENEDIR/search/WildcardQuery.cpp \
           $$CLUCENEDIR/search/WildcardTermEnum.cpp \
           $$CLUCENEDIR/store/FSDirectory.cpp \
           $$CLUCENEDIR/store/IndexInput.cpp \
           $$CLUCENEDIR/store/IndexOutput.cpp \
           $$CLUCENEDIR/store/Lock.cpp \
           $$CLUCENEDIR/store/MMapInput.cpp \
           $$CLUCENEDIR/store/RAMDirectory.cpp \
           $$CLUCENEDIR/store/TransactionalRAMDirectory.cpp \
           $$CLUCENEDIR/util/BitSet.cpp \
           $$CLUCENEDIR/util/Equators.cpp \
           $$CLUCENEDIR/util/FastCharStream.cpp \
           $$CLUCENEDIR/util/fileinputstream.cpp \
           $$CLUCENEDIR/util/Misc.cpp \
           $$CLUCENEDIR/util/Reader.cpp \
           $$CLUCENEDIR/util/StringBuffer.cpp \
           $$CLUCENEDIR/util/StringIntern.cpp \
           $$CLUCENEDIR/util/ThreadLocal.cpp \
           $$CLUCENEDIR/analysis/standard/StandardAnalyzer.cpp \
           $$CLUCENEDIR/analysis/standard/StandardFilter.cpp \
           $$CLUCENEDIR/analysis/standard/StandardTokenizer.cpp


#Header files
HEADERS += qclucene_global_p.h \
           qclucene-config_p.h \
           qanalyzer_p.h \
           qtokenizer_p.h \
           qtoken_p.h \
           qtokenstream_p.h \
           qdocument_p.h \
           qfield_p.h \
           qindexreader_p.h \
           qindexwriter_p.h \
           qterm_p.h \
           qqueryparser_p.h \
           qfilter_p.h \
           qhits_p.h \
           qsearchable_p.h \
           qsort_p.h \
           qquery_p.h \
           qreader_p.h


#Source files
SOURCES += qanalyzer.cpp \
           qtokenizer.cpp \
           qtoken.cpp \
           qtokenstream.cpp \
           qdocument.cpp \
           qfield.cpp \
           qindexreader.cpp \
           qindexwriter.cpp \
           qterm.cpp \
           qqueryparser.cpp \
           qfilter.cpp \
           qhits.cpp \
           qsearchable.cpp \
           qsort.cpp \
           qquery.cpp \
           qreader.cpp
