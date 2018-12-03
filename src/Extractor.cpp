#include "include/Extractor.h"

std::vector<TableColumnInfo>& Extractor::getMetaData()
{
    return _metaData;
}

outdata_t& Extractor::getPreviewData()
{
    return _previewData;
}
