/*******************************************************************************
 *
 * socks5
 * A C++11 socks5 proxy server based on Libevent 
 *
 * Copyright 2018 Senlin Zhan. All rights reserved.
 *
 ******************************************************************************/

#include "address.hpp"
#include "cipher.hpp"
#include "server.hpp"

#include <gflags/gflags.h>
#include <glog/logging.h>

// Check whether the port is in range [1, 65535]
static bool isValidPort(const char *flagname, gflags::int32 value)
{
    return (value > 0 && value < 65536);
}

// Check whether the screct key is valid
static bool isValidSecretKey(const char *flagname, const std::string &value)
{
    return value.size() == Cryptor::KEY_SIZE;
}

// Listening address of the local server
DEFINE_string(host, "0.0.0.0", "Listening host");
DEFINE_int32(port, 5050, "Listening port");

// Listening address of the proxy server
DEFINE_string(remoteHost, "127.0.0.1", "Remote host");
DEFINE_int32(remotePort, 6060, "Remote port");

// Secret key
DEFINE_string(key, "12345678123456781234567812345678", "Secret key");

int main(int argc, char *argv[])
{
    if (!gflags::RegisterFlagValidator(&FLAGS_port, &isValidPort))
    {
        LOG(FATAL) << "Failed to register port validator";
    }

    if (!gflags::RegisterFlagValidator(&FLAGS_remotePort, &isValidPort))
    {
        LOG(FATAL) << "Failed to register remotePort validator";
    }
    
    if (!gflags::RegisterFlagValidator(&FLAGS_key, &isValidSecretKey))
    {
        LOG(FATAL) << "Failed to register key validator";
    }
    
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    auto port = static_cast<unsigned short>(FLAGS_port);
    auto remotePort = static_cast<unsigned short>(FLAGS_remotePort);

    // address of the local server
    auto address = Address::FromHostOrder(FLAGS_host, port);

    // address of the proxy server
    auto remoteAddress = Address::FromHostOrder(FLAGS_remoteHost, remotePort);

    LOG(WARNING) << "Local server options: "
                 << "Listening address = " << address << ", "
                 << "Proxy server address = " << remoteAddress << ", "
                 << "Secret key = " << FLAGS_key;
    
    Server server(address, remoteAddress, FLAGS_key);
    server.run();
    
    return 0;
}
