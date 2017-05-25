/******************************************************************************
 * File:    productsmodel.cpp
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
 *   Provides object implementation for some declarations
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

#include "productsmodel.h"

namespace QPF {

ProductsModel::ProductsModel()
{
    /*
    defineQuery("SELECT  "
                "    concat(i.instrument, ':', p.signature) AS Signature,  "
                "    p.product_id as ID,  "
                "    p.product_type as Type,  "
                "    p.product_version as Version,  "
                "    p.product_size as Size,  "
                "    ps.status_desc as Status,  "
                "    c.creator_desc as Creator,  "
                "    o.obsmode_desc as ObsMode,  "
                "    p.start_time as Start,  "
                "    p.end_time as End,  "
                "    p.registration_time as RegTime,  "
                "    p.url as URL "
                "FROM products_info p  "
                "INNER JOIN instruments i ON p.instrument_id = i.instrument_id  "
                "INNER JOIN product_status ps ON p.product_status_id = ps.product_status_id  "
                "INNER JOIN creators c ON p.creator_id = c.creator_id  "
                "INNER JOIN observation_modes o ON p.obsmode_id = o.obsmode_id  "
                "ORDER BY concat(i.instrument, '.',  "
                "                p.signature, '.',  "
                "                right(concat('00000000000000000000', p.ID), 20)),"
                "                p.registration_time;");
    */
    defineQuery("SELECT  "
                "    concat(p.obs_id, ':', p.instrument_id) AS ObsIdInstr,  "
                "    p.product_id as ID,  "
                "    p.product_type as Type,  "
                "    p.product_version as Version,  "
                "    p.product_size as Size,  "
                "    p.product_status_id as Status,  "
                "    p.creator_id as Creator,  "
                "    p.obsmode_id as ObsMode,  "
                "    p.start_time as Start,  "
                "    p.end_time as End,  "
                "    p.registration_time as RegTime,  "
                "    p.url as URL "
                "FROM products_info p  "
                "ORDER BY concat(p.obs_id, '.', p.product_type, '.',  "
                "                right(concat('00000000000000000000', p.ID), 20)),"
                "                p.registration_time;");
    defineHeaders({//"Signature",
                   "Product Id", "Type", "Version",
                   "Size", "Status", "Creator", "Obs.Mode",
                   "Start", "End", "Reg.Time", "URL"});

    skipColumns(1);

    refresh();
}

}
