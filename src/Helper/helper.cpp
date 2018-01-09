

#include "helper.h"

MedianFilter::MedianFilter(uint64_t listlength, FilterValue initalValue) {
    _filterList.resize(listlength);
    for (int i = 0; i < _filterList.size(); ++i) {
        _filterList[i] = initalValue;
    }
}

MedianFilter::~MedianFilter() {

}

FilterValue MedianFilter::putNewValue(FilterValue value) {
    rotate(_filterList.begin(), _filterList.begin() + _filterList.size() - 1, _filterList.end());
    _filterList[0] = value;

    //Kopieren der Daten
    vector<FilterValue> tmpFilterList = _filterList;
    sort(tmpFilterList.begin(), tmpFilterList.end());
    return tmpFilterList[tmpFilterList.size() / 2];
}

MedianFilter::MedianFilter() {
    MedianFilter::MedianFilter(10, 400);
}
