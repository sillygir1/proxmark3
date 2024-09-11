#pragma once

#include "../gui.h"
#include "../nfc_common.h"
#include <cmdhflist.h>
#include <comms.h>
#include <pm3_cmd.h>

void hf_trace_init(void *_view_manager, void *ctx);

void hf_trace_exit();
