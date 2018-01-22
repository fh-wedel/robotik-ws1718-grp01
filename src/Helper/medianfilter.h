

#ifndef PROJECT_HELPER_H
#define PROJECT_HELPER_H

#include <iostream>
#include "stdafx.h"
#include "protocol.h"

/**
 * Medianfilter.
 */
class MedianFilter {
private:
    vector<FilterValue> _filterList;

public:
    MedianFilter();
    ~MedianFilter();

    /**
     * Initialisiert den Medianfilter.
     * @param listlength Medianfilterlaenge
     * @param initalValue initial Wert
     */
    void initMedianFilter(uint64_t listlength, FilterValue initalValue);

    /**
     * Fuegt einen neuen Wert hinzu und gibt einen gefilterten Wert zurueck.
     * @param value neuer Wert
     * @return gefilterter Wert
     */
    FilterValue putNewValue(FilterValue value);
    FilterValue medianFromArray(vector<FilterValue> filterVals);
};

/**
 * Initialisiert den Medianfilter.
 * @param listlength Medianfilterlaenge
 * @param initalValue initial Wert
 */
void MedianFilter::initMedianFilter(uint64_t listlength, FilterValue initalValue) {
    _filterList.resize(listlength);
    for (unsigned int i = 0; i < _filterList.size(); ++i) {
        _filterList[i] = initalValue;
    }
}

MedianFilter::~MedianFilter() {

}

/**
 * Fuegt einen neuen Wert hinzu und gibt einen gefilterten Wert zurueck.
 * @param value neuer Wert
 * @return gefilterter Wert
 */
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

FilterValue MedianFilter::medianFromArray(vector<FilterValue> filterVals){
    if (filterVals.size() == 0) {
        cout << "MedianFilter not initialized" << endl;
        return 0; //lerder eher unpraktisch...
    }
    vector<FilterValue> tmpFilterList = filterVals;
    sort(tmpFilterList.begin(), tmpFilterList.end());
    return tmpFilterList[tmpFilterList.size() / 2];
}


MedianFilter::MedianFilter() {

}


#endif //PROJECT_HELPER_H
