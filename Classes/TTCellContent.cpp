//
//  TTCellContent.cpp
//  animalplanet
//
//  Created by Marc Serena on 21/1/15.
//
//

#include "TTCellContent.h"

TTCellContent::TTCellContent(CONTENT_TYPES contentLayerType):
_contentLayerType(contentLayerType),
_contentIsDeath(false),
_contentIsBlocked(false),
_isUpdatingContent(false)
{

}

TTCellContent::~TTCellContent()
{
    
}

bool TTCellContent::isComboContent()
{
    return _contentLayerType==COMBO_VERTICAL_DYNAMIC_CONTENT ||
    _contentLayerType==COMBO_HORIZONTAL_DYNAMIC_CONTENT||
    _contentLayerType==COMBO_AREA_DYNAMIC_CONTENT;
}
