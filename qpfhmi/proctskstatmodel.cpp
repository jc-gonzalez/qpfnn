/******************************************************************************
 * File:    proctskstatmodel.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.proctskstatmodel
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

#include "proctskstatmodel.h"

namespace QPF {

ProcTaskStatusModel::ProcTaskStatusModel()
{
    defineQuery("SELECT "
                "id as ID, "
                "t.task_data#>>'{State,StartedAt}' AS start, "
                "t.task_data#>>'{State,FinishedAt}' AS finish, "
                "t.task_data->>'{Info,TaskName' AS name, "
                "t.task_data->>'{Info,Agent}' AS agent, "
                "t.task_path AS proc, "
                "tt.status_desc AS status, "
                "t.task_data#>>'{State,Progress}' AS progress, "
                "t.task_data#>>'{State,ExitCode}' AS exit_code, "
                "t.task_data AS task_data "
                "FROM tasks_info t "
                "INNER JOIN task_status tt "
                "      ON t.task_data#>>'{State,TaskStatus}' = tt.task_status_id::text "
                "ORDER BY id;");

//    defineQuery("SELECT * FROM tasks_info t "
//                "ORDER BY id;");

    defineHeaders({"ID", "Started at", "Finished at",
                "Task Name", "Agent", "Proc.Element",
                "Status", "Progress", "Exit Code", "Task Info"});


    ColumnPalette statusPalette;
    statusPalette["SCHEDULED"] = FgBgColors(QColor(Qt::gray),  QColor(Qt::white));
    statusPalette["RUNNING"]   = FgBgColors(QColor(Qt::blue),  QColor(Qt::white));
    statusPalette["FINISHED"]  = FgBgColors(QColor(Qt::black), QColor(Qt::green));
    statusPalette["FAILED"]    = FgBgColors(QColor(Qt::white), QColor(Qt::red));

    TablePalette  tblPalette;
    tblPalette[6] = statusPalette;

    defineTablePalette(tblPalette);

//    setFullUpdate(true);

    refresh();
}

}
