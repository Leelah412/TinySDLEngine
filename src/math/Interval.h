#ifndef __INTERVAL_H__
#define __INTERVAL_H__

namespace tse{

// Intervals, where no interval overlaps with another
typedef struct DisjointInterval{
	unsigned int start;
	unsigned int end;

	DisjointInterval(): start{0}, end{0}{}
	DisjointInterval(unsigned int _start, unsigned int _end){
		set_interval(_start, _end);
	}

	void set_interval(unsigned int _start, unsigned int _end){
		// swap passed start and end to make interval correct
		if(_start > _end){
			start = _end;
			end = _start;
			return;
		}
		start = _start;
		end = _end;
	}

	bool is_disjoint(const DisjointInterval& interval) const{
		if((start <= interval.start) && (end >= interval.start)) return false;
		if((start <= interval.end) && (end >= interval.end)) return false;
		return true;
	}
	// Returns the length of the interval + 1 (inclusive interval)
	unsigned int length() const{ return end - start + 1; }
	// Start must not be bigger than end
	bool correct() const{ return !(start > end); }
	// Disjoint means end of interval with lower start position is lower than start of the other
	// Ensuring, that that's actually the case, is up to the implementation, but less operator assumes disjointness
	bool operator<(const DisjointInterval& interval) const{
		return start < interval.start;
	}
} DisjointInterval;

}

#endif // !__INTERVAL_H__
