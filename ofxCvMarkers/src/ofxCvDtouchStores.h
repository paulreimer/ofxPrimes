#pragma once

#include "ofxCvDtouchStores.h"
#include "dtouch/sequencefilestore.h"

template <bool b>
class BoolStore : public SequenceStore {
public:
	BoolStore(){}
	
	virtual void addSequence( int * seq, bool white ){ return; }
	virtual bool contains(const int * seq, const bool white){ return b; }
};

typedef BoolStore<true> YesStore;
typedef BoolStore<false> NoStore;

template <int nonnulls>
class NonNullStore : public SequenceStore {
public:
	NonNullStore(){}
	
	virtual void addSequence( int * seq, bool white ){ return; }
	virtual bool contains(const int * seq, const bool white){
		for (int i=0; i<=nonnulls; i++)
			if (*(seq+i) == 0) return false;
		
		return true;
	}
};

typedef BoolStore<true> YesStore;
typedef BoolStore<false> NoStore;

/*
 template <int i, int j>
 class LUTStore : public SequenceStore {
 public:
 LUTStore(){}
 
 void addSequence(int* seq, bool white);
 bool contains(const int* seq, const bool white);
 
 private:
 //	std::map<std::pair<std::vector<int>, bool> >, bool> seqs;
 };
 
 class MapStore : public SequenceStore {
 public:
 MapStore(){}
 
 virtual void addSequence(int* seq, bool white);
 virtual bool contains(const int* seq, const bool white);
 
 private:
 //	std::map<std::pair<std::vector<int>, bool> >, bool> seqs;
 };
 */


