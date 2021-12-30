#include "util/format.h"
using namespace util;
int util::getFormatStr (std::string_view view, int tot, int currPos) {
    if (view.empty()) {
        if (currPos + 1 >= tot) {
            return -1;
        }
        return currPos + 1;
    }
    int n;
    auto result = std::from_chars(view.begin(), view.end(), n);
    if (result.ec == std::errc::result_out_of_range || (n >= tot || -n > tot)) {
        return -1;
    } else if (n < 0) {
        return tot - n;
    } else {
        return n;
    }
}