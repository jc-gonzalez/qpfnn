/******************************************************************************
 * File:    dirwatcher.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.DirWatcher
 *
 * Version:  1.2
 *
 * Date:    2016/06/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement DirWatcher class
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

#include "dwatcher.h"

#include <iostream>
#include <thread>

#include <cerrno>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
DirWatcher::DirWatcher(std::string pth)
{
    // Initialize file descriptor for inotify API
    fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Add watched directory to table
    if (! pth.empty()) { watch(pth); }

    // Start watch
    keepWatching = true;
    std::thread watcher(&DirWatcher::start, this);
    watcher.detach();
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
DirWatcher::~DirWatcher()
{
}

//----------------------------------------------------------------------
// Method: watch(pth)
//----------------------------------------------------------------------
void DirWatcher::watch(std::string pth)
{
    int wd = inotify_add_watch(fd, pth.c_str(),
                               IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);
    if (wd == -1) {
        std::cerr << "Cannot watch '" << pth << "%s'\n";
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    watchedDirs[wd] = pth;
}

//----------------------------------------------------------------------
// Method: start
//----------------------------------------------------------------------
void DirWatcher::start()
{
    static std::string lastTriggerEventFileName("");

    int poll_num;
    nfds_t nfds;
    struct pollfd fds[1];

    char buf[4096]
        __attribute__ ((aligned(__alignof__(struct inotify_event))));

    // Initialize polling variables
    nfds = 1;
    fds[0].fd     = fd;
    fds[0].events = POLLIN;

    // Polling loop
    while (keepWatching) {

        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR) continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if ((poll_num > 0) && (fds[0].revents & POLLIN)) {

            // In case of events, get them and store them into the queue
            const struct inotify_event * event;
            ssize_t len;
            char * ptr;

            // Loop while events can be read from inotify file desc.
            for (;;) {
                // Read events
                len = read(fd, buf, sizeof buf);
                if ((len == -1) && (errno != EAGAIN)) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }

                // If the nonblocking read() found no events to read, then it
                // returns -1 with errno set to EAGAIN. In that case, we exit
                // the loop
                if (len <= 0) break;

                // Loop over all events in the buffer
                for (ptr = buf; ptr < buf + len;
                     ptr += sizeof(struct inotify_event) + event->len) {

                    event = (const struct inotify_event *)(ptr);

                    // Store event in queue
                    DirWatchEvent dwe;
                    dwe.name = std::string(event->name);
                    if (dwe.name == lastTriggerEventFileName) { continue; }

                    dwe.path = watchedDirs[event->wd];
                    dwe.mask = event->mask;
                    dwe.isDir = (event->mask & IN_ISDIR);

                    lastTriggerEventFileName = dwe.name;

                    // Check file size
                    if (!dwe.isDir) {
                        struct stat dweStat;
                        std::string file(dwe.path + "/" + dwe.name);
                        if (stat(file.c_str(), &dweStat) != 0) {
                            perror("stat");
                            std::cerr << file << std::endl;
                            exit(EXIT_FAILURE);
                        } else {
                            dwe.size = dweStat.st_size;
                        }
                    } else {
                        dwe.size = -1;
                    }

//                    bool proceed = true;
//                    if (events.size() > 0) {
//                        proceed = events.back().path != dwe.path;
//                    }

//                    if (proceed) {
                        fprintf(stderr, "Storing event for %s\n", (dwe.path + "/" + dwe.name).c_str());
                        events.push(dwe);
//                    }
                }
            }

        }
    }

    // Not watching anymore
    close(fd);
}

//----------------------------------------------------------------------
// Method: stop
//----------------------------------------------------------------------
void DirWatcher::stop()
{
    keepWatching = false;
}

//----------------------------------------------------------------------
// Method: nextEvent
//----------------------------------------------------------------------
bool DirWatcher::nextEvent(DirWatchEvent & event)
{
    if (events.size() > 0) {
        event = events.front();
        events.pop();
        return true;
    }
    return false;
}

//}
