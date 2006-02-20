//
// tcp.hpp
// ~~~~~~~
//
// Copyright (c) 2003-2006 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IPV6_TCP_HPP
#define ASIO_IPV6_TCP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/push_options.hpp"

#include "asio/detail/push_options.hpp"
#include <boost/throw_exception.hpp>
#include <boost/detail/workaround.hpp>
#include <cstring>
#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
# include <iostream>
#endif // BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
#include <memory>
#include "asio/detail/pop_options.hpp"

#include "asio/basic_socket_acceptor.hpp"
#include "asio/basic_stream_socket.hpp"
#include "asio/error.hpp"
#include "asio/socket_acceptor_service.hpp"
#include "asio/stream_socket_service.hpp"
#include "asio/ipv6/address.hpp"
#include "asio/detail/socket_ops.hpp"
#include "asio/detail/socket_option.hpp"
#include "asio/detail/socket_types.hpp"

namespace asio {
namespace ipv6 {

/// Encapsulates the flags needed for TCP.
/**
 * The asio::ipv6::tcp class contains flags necessary for TCP sockets.
 *
 * @par Thread Safety:
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Safe.
 *
 * @par Concepts:
 * Protocol.
 */
class tcp
{
public:
  class endpoint;

  /// Obtain an identifier for the type of the protocol.
  int type() const
  {
    return SOCK_STREAM;
  }

  /// Obtain an identifier for the protocol.
  int protocol() const
  {
    return IPPROTO_TCP;
  }

  /// Obtain an identifier for the protocol family.
  int family() const
  {
    return PF_INET6;
  }

  /// Template typedefs for acceptor and socket types.
  template <typename Allocator>
  struct types
  {
    /// The service type for IPv4 TCP sockets.
    typedef stream_socket_service<tcp, Allocator> socket_service;

    /// The IPv4 TCP socket type.
    typedef basic_stream_socket<socket_service> socket;

    /// The service type for IPv4 TCP acceptors.
    typedef socket_acceptor_service<tcp, Allocator> acceptor_service;

    /// The IPv4 TCP acceptor type.
    typedef basic_socket_acceptor<acceptor_service> acceptor;
  };

  /// The IPv4 TCP socket type.
  typedef types<std::allocator<void> >::socket socket;

  /// The IPv4 TCP acceptor type.
  typedef types<std::allocator<void> >::acceptor acceptor;

  /// Socket option for disabling the Nagle algorithm.
  /**
   * Implements the IPPROTO_TCP/TCP_NODELAY socket option.
   *
   * @par Examples:
   * Setting the option:
   * @code
   * asio::stream_socket socket(io_service); 
   * ...
   * asio::ipv6::tcp::no_delay option(true);
   * socket.set_option(option);
   * @endcode
   *
   * @par
   * Getting the current option value:
   * @code
   * asio::stream_socket socket(io_service); 
   * ...
   * asio::ipv6::tcp::no_delay option;
   * socket.get_option(option);
   * bool is_set = option.get();
   * @endcode
   *
   * @par Concepts:
   * Socket_Option, Boolean_Socket_Option.
   */
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined no_delay;
#else
  typedef asio::detail::socket_option::boolean<
    IPPROTO_TCP, TCP_NODELAY> no_delay;
#endif
};

/// Describes an endpoint for a TCP socket.
/**
 * The asio::ipv6::tcp::endpoint class describes an endpoint that may be
 * associated with a particular socket.
 *
 * @par Thread Safety:
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 *
 * @par Concepts:
 * Endpoint.
 */
class tcp::endpoint
{
public:
  /// The protocol type associated with the endpoint.
  typedef tcp protocol_type;

  /// The type of the endpoint structure. This type is dependent on the
  /// underlying implementation of the socket layer.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined data_type;
#else
  typedef asio::detail::socket_addr_type data_type;
#endif

  /// The type for the size of the endpoint structure. This type is dependent on
  /// the underlying implementation of the socket layer.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined size_type;
#else
  typedef asio::detail::socket_addr_len_type size_type;
#endif

  /// Default constructor.
  endpoint()
  {
    addr_.sin6_family = AF_INET6;
    addr_.sin6_port = 0;
    addr_.sin6_flowinfo = 0;
    in6_addr tmp_addr = IN6ADDR_LOOPBACK_INIT;
    addr_.sin6_addr = tmp_addr;
    addr_.sin6_scope_id = 0;
  }

  /// Construct an endpoint using a port number, specified in the host's byte
  /// order. The IP address will be the any address (i.e. in6addr_any). This
  /// constructor would typically be used for accepting new connections.
  endpoint(unsigned short port_num)
  {
    addr_.sin6_family = AF_INET6;
    addr_.sin6_port =
      asio::detail::socket_ops::host_to_network_short(port_num);
    addr_.sin6_flowinfo = 0;
    in6_addr tmp_addr = IN6ADDR_LOOPBACK_INIT;
    addr_.sin6_addr = tmp_addr;
    addr_.sin6_scope_id = 0;
  }

  /// Construct an endpoint using a port number and an IP address. This
  /// constructor may be used for accepting connections on a specific interface
  /// or for making a connection to a remote endpoint.
  endpoint(unsigned short port_num, const asio::ipv6::address& addr)
  {
    using namespace std; // For memcpy.
    addr_.sin6_family = AF_INET6;
    addr_.sin6_port =
      asio::detail::socket_ops::host_to_network_short(port_num);
    addr_.sin6_flowinfo = 0;
    asio::ipv6::address::bytes_type bytes = addr.to_bytes();
    memcpy(addr_.sin6_addr.s6_addr, bytes.elems, 16);
    addr_.sin6_scope_id = 0;
  }

  /// Copy constructor.
  endpoint(const endpoint& other)
    : addr_(other.addr_)
  {
  }

  /// Assign from another endpoint.
  endpoint& operator=(const endpoint& other)
  {
    addr_ = other.addr_;
    return *this;
  }

  /// The protocol associated with the endpoint.
  protocol_type protocol() const
  {
    return protocol_type();
  }

  /// Get the underlying endpoint in the native type.
  data_type* data()
  {
    return reinterpret_cast<data_type*>(&addr_);
  }

  /// Get the underlying endpoint in the native type.
  const data_type* data() const
  {
    return reinterpret_cast<const data_type*>(&addr_);
  }

  /// Get the underlying size of the endpoint in the native type.
  size_type size() const
  {
    return sizeof(addr_);
  }

  /// Set the underlying size of the endpoint in the native type.
  void size(size_type size)
  {
    if (size != sizeof(addr_))
    {
      asio::error e(asio::error::invalid_argument);
      boost::throw_exception(e);
    }
  }

  /// Get the port associated with the endpoint. The port number is always in
  /// the host's byte order.
  unsigned short port() const
  {
    return asio::detail::socket_ops::network_to_host_short(
        addr_.sin6_port);
  }

  /// Set the port associated with the endpoint. The port number is always in
  /// the host's byte order.
  void port(unsigned short port_num)
  {
    addr_.sin6_port =
      asio::detail::socket_ops::host_to_network_short(port_num);
  }

  /// Get the IP address associated with the endpoint.
  asio::ipv6::address address() const
  {
    using namespace std; // For memcpy.
    asio::ipv6::address::bytes_type bytes;
    memcpy(bytes.elems, addr_.sin6_addr.s6_addr, 16);
    return asio::ipv6::address(bytes);
  }

  /// Set the IP address associated with the endpoint.
  void address(const asio::ipv6::address& addr)
  {
    using namespace std; // For memcpy.
    asio::ipv6::address::bytes_type bytes = addr.to_bytes();
    memcpy(addr_.sin6_addr.s6_addr, bytes.elems, 16);
  }

  /// Compare two endpoints for equality.
  friend bool operator==(const endpoint& e1, const endpoint& e2)
  {
    return e1.address() == e2.address() && e1.port() == e2.port();
  }

  /// Compare two endpoints for inequality.
  friend bool operator!=(const endpoint& e1, const endpoint& e2)
  {
    return e1.address() != e2.address() || e1.port() != e2.port();
  }

  /// Compare endpoints for ordering.
  friend bool operator<(const endpoint& e1, const endpoint& e2)
  {
    if (e1.address() < e2.address())
      return true;
    if (e1.address() != e2.address())
      return false;
    return e1.port() < e2.port();
  }

private:
  // The underlying IPv4 socket address.
  asio::detail::inet_addr_v6_type addr_;
};

/// Output an endpoint as a string.
/**
 * Used to output a human-readable string for a specified endpoint.
 *
 * @param os The output stream to which the string will be written.
 *
 * @param endpoint The endpoint to be written.
 *
 * @return The output stream.
 *
 * @relates tcp::endpoint
 */
#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
std::ostream& operator<<(std::ostream& os, const tcp::endpoint& endpoint)
{
  os << '[' << endpoint.address().to_string() << ']' << ':' << endpoint.port();
  return os;
}
#else // BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
template <typename Ostream>
Ostream& operator<<(Ostream& os, const tcp::endpoint& endpoint)
{
  os << '[' << endpoint.address().to_string() << ']' << ':' << endpoint.port();
  return os;
}
#endif // BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))

} // namespace ipv6
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_IPV6_TCP_HPP