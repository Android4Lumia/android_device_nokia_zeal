/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"

#include "dpp.h"

using namespace std;

void ds_properties();

int dpp::dpp_mount() {
    int ret = 0;

    if (access(DPP_MOUNTPOINT, F_OK) == -1) {
        ret = make_dir(DPP_MOUNTPOINT, 0400);
        if (ret) {
            if (mount("", "/", "", MS_REMOUNT, NULL) == 0) {
                ret = make_dir(DPP_MOUNTPOINT, 0400);
                mount("", "/", "", MS_REMOUNT|MS_RDONLY, NULL);
            }
        }
    }

    if (!ret) {
        if (mount(DPP_PARTITION, DPP_MOUNTPOINT, DPP_FS, DPP_FLAGS, DPP_DATA) == 0) {
            return 0;
        }
        else {
            ERROR("Mounting DPP failed\n");
            return -1;
        }
    }
    else {
        ERROR("Cannot create mountpoint for DPP\n");
        return -1;
    }
}

dpp::device dpp::get_device() {
    ifstream prod_dat;
    string rmcode;
    if (dpp::dpp_mount() == 0) {
        if (access(PRODUCT_DAT, R_OK) == 0) {
            prod_dat.open(PRODUCT_DAT);
            prod_dat >> rmcode;
            rmcode.erase(rmcode.find_last_not_of("\n\r")+1);
            parts = split(rmcode, ':');
            if (parts.size() == 2) {
                if (parts.at(0) == "TYPE") {
                    if (dpp::devices.count(parts.at(1)) == 1) {
                        return dpp::devices.at(parts.at(1));
                    }
                }
                else {
                    ERROR("This product.dat is strange, TYPE should be the first line\n");
                }
            }
            else {
                ERROR("Invalid product.bin?\n");
            }
            prod_dat.close();
        }
        else {
            ERROR("Cannot access device info\n");
        }
    }
    return dpp::unknown;
}

void ds_properties()
{
    property_set("persist.radio.multisim.config", "dsds");
    property_set("persist.radio.dont_use_dsd", "true");
}

void vendor_load_properties()
{
    dpp::device dev;

    dev = dpp::get_device();

    property_set("ro.product.device", dev.device);
    property_set("ro.product.name",   "lineage_" + dev.family);
    property_set("ro.product.model",  dev.description);
}
