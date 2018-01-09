

#ifndef PROJECT_HELPER_H
#define PROJECT_HELPER_H

#include "../../protocol.h"

class MedianFilter {
private:
    vector<FilterValue> _filterList;

public:
    MedianFilter();
    MedianFilter(uint64_t listlength, FilterValue initalValue);
    ~MedianFilter();
    FilterValue putNewValue(FilterValue value);
};


#endif //PROJECT_HELPER_H
