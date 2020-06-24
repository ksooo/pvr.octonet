/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

//Include platform specific datatypes, header files, defines and constants:
#if defined TARGET_WINDOWS
#define WIN32_LEAN_AND_MEAN // Enable LEAN_AND_MEAN support
#pragma warning(disable : 4005) // Disable "warning C4005: '_WINSOCKAPI_' : macro redefinition"
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma warning(default : 4005)
#include <windows.h>

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifndef socklen_t
typedef int socklen_t;
#endif
#ifndef ipaddr_t
typedef unsigned long ipaddr_t;
#endif
#ifndef port_t
typedef unsigned short port_t;
#endif
#ifndef sa_family_t
#define sa_family_t ADDRESS_FAMILY
#endif
#elif defined TARGET_LINUX || defined TARGET_DARWIN || defined TARGET_FREEBSD
#ifdef SOCKADDR_IN
#undef SOCKADDR_IN
#endif
#include <arpa/inet.h> /* for inet_pton */
#include <errno.h>
#include <fcntl.h>
#include <netdb.h> /* for gethostbyname */
#include <netinet/in.h> /* for htons */
#include <sys/socket.h> /* for socket,connect */
#include <sys/types.h> /* for socket,connect */
#include <sys/un.h> /* for Unix socket */
#include <unistd.h> /* for read, write, close */

typedef int SOCKET;
typedef sockaddr SOCKADDR;
typedef sockaddr_in SOCKADDR_IN;
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
#define SOCKET_ERROR (-1)

#define closesocket(sd) ::close(sd)
#else
#error Platform specific socket support is not yet available on this platform!
#endif

#include <string>
#include <vector>

namespace OCTO
{

#define MAXCONNECTIONS 1 ///< Maximum number of pending connections before "Connection refused"
#define MAXRECV 1500 ///< Maximum packet size

enum SocketFamily
{
  af_unspec = AF_UNSPEC,
  af_inet = AF_INET,
  af_inet6 = AF_INET6
};

enum SocketDomain
{
#if defined TARGET_LINUX || defined TARGET_DARWIN || defined TARGET_FREEBSD
  pf_unix = PF_UNIX,
  pf_local = PF_LOCAL,
#endif
  pf_inet = PF_INET
};

enum SocketType
{
  sock_stream = SOCK_STREAM,
  sock_dgram = SOCK_DGRAM
};

enum SocketProtocol
{
  tcp = IPPROTO_TCP,
  udp = IPPROTO_UDP
};

class Socket
{
public:
  /*!
   * An unconnected socket may be created directly on the local
   * machine. The socket type (SOCK_STREAM, SOCK_DGRAM) and
   * protocol may also be specified.
   * If the socket cannot be created, an exception is thrown.
   *
   * \param family Socket family (IPv4 or IPv6)
   * \param domain The domain parameter specifies a communications domain within which communication will take place;
   * this selects the protocol family which should be used.
   * \param type base type and protocol family of the socket.
   * \param protocol specific protocol to apply.
   */
  Socket(const enum SocketFamily family,
         const enum SocketDomain domain,
         const enum SocketType type,
         const enum SocketProtocol protocol = tcp);
  Socket(void);
  virtual ~Socket();

  //Socket settings

  /*!
   * Socket setFamily
   * \param family    Can be af_inet or af_inet6. Default: af_inet
   */
  void setFamily(const enum SocketFamily family) { m_family = family; };

  /*!
   * Socket setDomain
   * \param domain    Can be pf_unix, pf_local, pf_inet or pf_inet6. Default: pf_inet
   */
  void setDomain(const enum SocketDomain domain) { m_domain = domain; };

  /*!
   * Socket setType
   * \param type    Can be sock_stream or sock_dgram. Default: sock_stream.
   */
  void setType(const enum SocketType type) { m_type = type; };

  /*!
   * Socket setProtocol
   * \param protocol    Can be tcp or udp. Default: tcp.
   */
  void setProtocol(const enum SocketProtocol protocol) { m_protocol = protocol; };

  /*!
   * Socket setPort
   * \param port    port number for socket communication
   */
  void setPort(const unsigned short port) { m_sockaddr.sin_port = htons(port); };

  bool setHostname(const std::string& host);

  // Server initialization

  /*!
   * Socket create
   * Create a new socket
   * \return     True if succesful
   */
  bool create();

  /*!
   * Socket close
   * Close the socket
   * \return     True if succesful
   */
  bool close();

  /*!
   * Socket bind
   */
  bool bind(const unsigned short port);
  bool listen() const;
  bool accept(Socket& socket) const;

  // Client initialization
  bool connect(const std::string& host, const unsigned short port);

  bool reconnect();

  // Data Transmission

  /*!
   * Socket send function
   *
   * \param data    Reference to a std::string with the data to transmit
   * \return    Number of bytes send or -1 in case of an error
   */
  int send(const std::string& data);

  /*!
   * Socket send function
   *
   * \param data    Pointer to a character array of size 'size' with the data to transmit
   * \param size    Length of the data to transmit
   * \return    Number of bytes send or -1 in case of an error
   */
  int send(const char* data, const unsigned int size);

  /*!
   * Socket sendto function
   *
   * \param data    Reference to a std::string with the data to transmit
   * \param size    Length of the data to transmit
   * \param sendcompletebuffer    If 'true': do not return until the complete buffer is transmitted
   * \return    Number of bytes send or -1 in case of an error
   */
  int sendto(const char* data, unsigned int size, bool sendcompletebuffer = false);
  // Data Receive

  /*!
   * Socket receive function
   *
   * \param data    Reference to a std::string for storage of the received data.
   * \param minpacketsize    The minimum number of bytes that should be received before returning from this function
   * \return    Number of bytes received or SOCKET_ERROR
   */
  int receive(std::string& data, unsigned int minpacketsize) const;

  /*!
   * Socket receive function
   *
   * \param data    Reference to a std::string for storage of the received data.
   * \return    Number of bytes received or SOCKET_ERROR
   */
  int receive(std::string& data) const;

  /*!
   * Socket receive function
   *
   * \param data    Pointer to a character array of size buffersize. Used to store the received data.
   * \param buffersize    Size of the 'data' buffer
   * \param minpacketsize    Specifies the minimum number of bytes that need to be received before returning
   * \return    Number of bytes received or SOCKET_ERROR
   */
  int receive(char* data, const unsigned int buffersize, const unsigned int minpacketsize) const;

  /*!
   * Socket recvfrom function
   *
   * \param data    Pointer to a character array of size buffersize. Used to store the received data.
   * \param buffersize    Size of the 'data' buffer
   * \param from        Optional: pointer to a sockaddr struct that will get the address from which the data is received
   * \param fromlen    Optional, only required if 'from' is given: length of from struct
   * \return    Number of bytes received or SOCKET_ERROR
   */
  int recvfrom(char* data,
               const int buffersize,
               struct sockaddr* from = nullptr,
               socklen_t* fromlen = nullptr) const;

  bool set_non_blocking(const bool);

  bool ReadLine(std::string& line);

  bool is_valid() const;

private:
  SOCKET m_sd; ///< Socket Descriptor
  SOCKADDR_IN m_sockaddr; ///< Socket Address
  //struct addrinfo* m_addrinfo;         ///< Socket address info
  std::string m_hostname; ///< Hostname
  unsigned short m_port; ///< Port number

  enum SocketFamily m_family; ///< Socket Address Family
  enum SocketProtocol m_protocol; ///< Socket Protocol
  enum SocketType m_type; ///< Socket Type
  enum SocketDomain m_domain; ///< Socket domain

#ifdef TARGET_WINDOWS
  WSADATA m_wsaData; ///< Windows Socket data
  static int
      win_usage_count; ///< Internal Windows usage counter used to prevent a global WSACleanup when more than one Socket object is used
#endif

  void errormessage(int errornum, const char* functionname = nullptr) const;
  int getLastError(void) const;
  bool osInit();
  void osCleanup();
};

} //namespace OCTO
