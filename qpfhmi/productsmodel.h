/******************************************************************************
 * File:    productsmodel.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.productsmodel
 *
 * Version:  1.2
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declaration of several dataproductsmodel for QPF HMI
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/
#ifndef PRODUCTSMODEL_H
#define PRODUCTSMODEL_H

#include "dbtreemodel.h"

namespace QPF {

class ProductsModel : public DBTreeModel {

    Q_OBJECT

public:
    explicit ProductsModel();
};

}

#endif // PRODUCTSMODEL_H
