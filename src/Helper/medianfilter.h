

#ifndef PROJECT_HELPER_H
#define PROJECT_HELPER_H

#include <iostream>
#include "stdafx.h"
#include "protocol.h"


class MedianFilter {
private:
    vector<FilterValue> _filterList;

public:
    MedianFilter();
    ~MedianFilter();
    void initMedianFilter(uint64_t listlength, FilterValue initalValue);
    FilterValue putNewValue(FilterValue value);
};

void MedianFilter::initMedianFilter(uint64_t listlength, FilterValue initalValue) {
    _filterList.resize(listlength);
    for (unsigned int i = 0; i < _filterList.size(); ++i) {
        _filterList[i] = initalValue;
    }
}

MedianFilter::~MedianFilter() {

}

FilterValue MedianFilter::putNewValue(FilterValue value) {
    if (_filterList.size() == 0) {
        cout << "MedianFilter not initialized" << endl;
        return 0; //lerder eher unpraktisch...
    }
    rotate(_filterList.begin(), _filterList.begin() + _filterList.size() - 1, _filterList.end());
    _filterList[0] = value;

    //Kopieren der Daten
    vector<FilterValue> tmpFilterList = _filterList;
    sort(tmpFilterList.begin(), tmpFilterList.end());
    return tmpFilterList[tmpFilterList.size() / 2];
}

MedianFilter::MedianFilter() {

}


#endif //PROJECT_HELPER_H
