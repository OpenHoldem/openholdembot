#pragma once

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/client_simple.hpp>

#include <algorithm>
#include <string>
#include <process.h>

extern xmlrpc_c::serverAbyss* xServer;

void xServerThread(void* dlg);

