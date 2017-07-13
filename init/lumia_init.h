#include <string>
#include <map>

#define DPP_PARTITION  "/dev/block/mmcblk0p1"
#define DPP_MOUNTPOINT "/dpp"
#define DPP_FS         "vfat"
#define DPP_FLAGS      MS_RDONLY|MS_NOATIME|MS_NODEV|MS_NODIRATIME|MS_NOEXEC|MS_NOSUID
#define DPP_DATA       "shortname=lower,uid=1000,gid=1000,dmask=227,fmask=337,context=u:object_r:firmware_file:s0"
#define PRODUCT_DAT    "/dpp/Nokia/product.dat"

namespace dpp {

    typedef struct device {
        std::string device;
        std::string family;
        std::string description;
    } device;

    int         mount(void);
    dpp::device get_device(void);

    dpp::device unknown = {
        .device = "unknown",
        .family = "unknown",
        .description = "Unknown or not supported device",
    };

    map<std::string, dpp:device> devices = {
        {
            "RM-885",
            {
                .device =      "zeal",
                .family =      "zeal",
                .description = "Lumia 720 (RM-885)",
            }
        },
        {
            "RM-887",
            {
                .device =      "zeal_cmcc",
                .family =      "zeal",
                .description = "Lumia 720 China (RM-887)",
            }
        },
    };

}
