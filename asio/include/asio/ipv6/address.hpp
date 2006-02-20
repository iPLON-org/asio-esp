//
// address.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2006 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IPV6_ADDRESS_HPP
#define ASIO_IPV6_ADDRESS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/push_options.hpp"

#include "asio/detail/push_options.hpp"
#include <cstring>
#include <string>
#include <stdexcept>
#include <boost/array.hpp>
#include <boost/throw_exception.hpp>
#include "asio/detail/pop_options.hpp"

#include "asio/error.hpp"
#include "asio/detail/socket_ops.hpp"
#include "asio/detail/socket_types.hpp"

namespace asio {
namespace ipv6 {

/// Implements IP version 6 style addresses.
/**
 * The asio::ipv6::address class provides the ability to use and
 * manipulate IP version 6 addresses.
 *
 * @par Thread Safety:
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
class address
{
public:
  /// The type used to represent an address as an array of bytes.
  typedef boost::array<unsigned char, 16> bytes_type;

  /// Default constructor.
  address()
  {
    in6_addr tmp_addr = IN6ADDR_ANY_INIT;
    addr_ = tmp_addr;
  }

  /// Construct an address from raw bytes.
  address(const bytes_type& bytes)
  {
    using namespace std; // For memcpy.
    memcpy(addr_.s6_addr, bytes.elems, 16);
  }

  /// Construct an address using an IP address string.
  address(const char* host)
  {
    if (asio::detail::socket_ops::inet_pton(AF_INET6, host, &addr_) <= 0)
    {
      asio::error e(asio::detail::socket_ops::get_error());
      boost::throw_exception(e);
    }
  }

  /// Construct an address using an IP address string.
  address(const std::string& host)
  {
    if (asio::detail::socket_ops::inet_pton(
          AF_INET6, host.c_str(), &addr_) <= 0)
    {
      asio::error e(asio::detail::socket_ops::get_error());
      boost::throw_exception(e);
    }
  }

  /// Copy constructor.
  address(const address& other)
    : addr_(other.addr_)
  {
  }

  /// Assign from another address.
  address& operator=(const address& other)
  {
    addr_ = other.addr_;
    return *this;
  }

  /// Assign from an IP address string.
  address& operator=(const char* addr)
  {
    address tmp(addr);
    addr_ = tmp.addr_;
    return *this;
  }

  /// Assign from an IP address string.
  address& operator=(const std::string& addr)
  {
    address tmp(addr);
    addr_ = tmp.addr_;
    return *this;
  }

  /// Get the address in bytes.
  bytes_type to_bytes() const
  {
    using namespace std; // For memcpy.
    bytes_type bytes;
    memcpy(bytes.elems, addr_.s6_addr, 16);
    return bytes;
  }

  /// Get the address as a string.
  std::string to_string() const
  {
    char addr_str[asio::detail::max_addr_v6_str_len];
    const char* addr =
      asio::detail::socket_ops::inet_ntop(AF_INET6, &addr_, addr_str,
          asio::detail::max_addr_v6_str_len);
    if (addr == 0)
    {
      asio::error e(asio::detail::socket_ops::get_error());
      boost::throw_exception(e);
    }
    return addr;
  }

  /// Determine whether the address is link local.
  bool is_link_local() const
  {
    return IN6_IS_ADDR_LINKLOCAL(&addr_) != 0;
  }

  /// Determine whether the address is site local.
  bool is_site_local() const
  {
    return IN6_IS_ADDR_SITELOCAL(&addr_) != 0;
  }

  /// Determine whether the address is a mapped IPv4 address.
  bool is_ipv4_mapped() const
  {
    return IN6_IS_ADDR_V4MAPPED(&addr_) != 0;
  }

  /// Determine whether the address is an IPv4-compatible address.
  bool is_ipv4_compatible() const
  {
    return IN6_IS_ADDR_V4COMPAT(&addr_) != 0;
  }

  /// Determine whether the address is a multicast address.
  bool is_multicast() const
  {
    return IN6_IS_ADDR_MULTICAST(&addr_) != 0;
  }

  /// Compare two addresses for equality.
  friend bool operator==(const address& a1, const address& a2)
  {
    using namespace std; // For memcmp.
    return memcmp(&a1.addr_, &a2.addr_, sizeof(in6_addr)) == 0;
  }

  /// Compare two addresses for inequality.
  friend bool operator!=(const address& a1, const address& a2)
  {
    using namespace std; // For memcmp.
    return memcmp(&a1.addr_, &a2.addr_, sizeof(in6_addr)) != 0;
  }

  /// Compare addresses for ordering.
  friend bool operator<(const address& a1, const address& a2)
  {
    using namespace std; // For memcmp.
    return memcmp(&a1.addr_, &a2.addr_, sizeof(in6_addr)) < 0;
  }

  /// Obtain an address object that represents any address.
  static address any()
  {
    return address();
  }

  /// Obtain an address object that represents the loopback address.
  static address loopback()
  {
    address tmp;
    in6_addr tmp_addr = IN6ADDR_LOOPBACK_INIT;
    tmp.addr_ = tmp_addr;
    return tmp;
  }

private:
  // The underlying IPv6 address.
  in6_addr addr_;
};

/// Output an address as a string.
/**
 * Used to output a human-readable string for a specified address.
 *
 * @param os The output stream to which the string will be written.
 *
 * @param addr The address to be written.
 *
 * @return The output stream.
 *
 * @relates tcp::endpoint
 */
template <typename Ostream>
Ostream& operator<<(Ostream& os, const address& addr)
{
  os << addr.to_string();
  return os;
}

} // namespace ipv6
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_IPV6_ADDRESS_HPP