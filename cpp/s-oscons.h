/*
------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                  S Y S T E M . O S _ C O N S T A N T S                   --
--                                                                          --
--                                 S p e c                                  --
--                                                                          --
--          Copyright (C) 2000-2018, Free Software Foundation, Inc.         --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 3,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.                                     --
--                                                                          --
-- As a special exception under Section 7 of GPL version 3, you are granted --
-- additional permissions described in the GCC Runtime Library Exception,   --
-- version 3.1, as published by the Free Software Foundation.               --
--                                                                          --
-- You should have received a copy of the GNU General Public License and    --
-- a copy of the GCC Runtime Library Exception along with this program;     --
-- see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    --
-- <http://www.gnu.org/licenses/>.                                          --
--                                                                          --
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

pragma Style_Checks ("M32766");
--  Allow long lines

*/
/*
--  This package provides target dependent definitions of constant for use
--  by the GNAT runtime library. This package should not be directly with'd
--  by an application program.

--  This file is generated automatically, do not modify it by hand! Instead,
--  make changes to s-oscons-tmplt.c and rebuild the GNAT runtime library.
*/



/*
package System.OS_Constants is

   pragma Pure;
*/
/*

   ---------------------------------
   -- General platform parameters --
   ---------------------------------

   type OS_Type is (Windows, Other_OS);
*/
#define Target_OS                     Windows
/*
   pragma Warnings (Off, Target_OS);
   --  Suppress warnings on Target_OS since it is in general tested for
   --  equality with a constant value to implement conditional compilation,
   --  which normally generates a constant condition warning.

*/
#define Target_Name                   "x86_64-w64-mingw32"
#define SIZEOF_unsigned_int           4
/*

   -------------------
   -- System limits --
   -------------------

*/
#define IOV_MAX                       2147483647
#define NAME_MAX                      260
/*

   ---------------------
   -- File open modes --
   ---------------------

*/
#define O_RDWR                        2
#define O_NOCTTY                      -1
#define O_NDELAY                      -1
/*

   ----------------------
   -- Fcntl operations --
   ----------------------

*/
#define F_GETFL                       -1
#define F_SETFL                       -1
/*

   -----------------
   -- Fcntl flags --
   -----------------

*/
#define FNDELAY                       -1
/*

   ----------------------
   -- Ioctl operations --
   ----------------------

*/
#define IOCTL_Req_T int
#define FIONBIO                       -2147195266
#define FIONREAD                      1074030207
/*

   ------------------
   -- Errno values --
   ------------------

   --  The following constants are defined from <errno.h>

*/
#define EAGAIN                        11
#define ENOENT                        2
#define ENOMEM                        12
/*

   --  The following constants are defined from <winsock2.h> (WSA*)

*/
#define EACCES                        10013
#define EADDRINUSE                    10048
#define EADDRNOTAVAIL                 10049
#define EAFNOSUPPORT                  10047
#define EALREADY                      10037
#define EBADF                         10009
#define ECONNABORTED                  10053
#define ECONNREFUSED                  10061
#define ECONNRESET                    10054
#define EDESTADDRREQ                  10039
#define EFAULT                        10014
#define EHOSTDOWN                     10064
#define EHOSTUNREACH                  10065
#define EINPROGRESS                   10036
#define EINTR                         10004
#define EINVAL                        10022
#define EIO                           10101
#define EISCONN                       10056
#define ELOOP                         10062
#define EMFILE                        10024
#define EMSGSIZE                      10040
#define ENAMETOOLONG                  10063
#define ENETDOWN                      10050
#define ENETRESET                     10052
#define ENETUNREACH                   10051
#define ENOBUFS                       10055
#define ENOPROTOOPT                   10042
#define ENOTCONN                      10057
#define ENOTSOCK                      10038
#define EOPNOTSUPP                    10045
#define EPIPE                         32
#define EPFNOSUPPORT                  10046
#define EPROTONOSUPPORT               10043
#define EPROTOTYPE                    10041
#define ERANGE                        34
#define ESHUTDOWN                     10058
#define ESOCKTNOSUPPORT               10044
#define ETIMEDOUT                     10060
#define ETOOMANYREFS                  10059
#define EWOULDBLOCK                   10035
#define E2BIG                         7
#define EILSEQ                        42
/*

   ----------------------
   -- Terminal control --
   ----------------------

*/
#define DTR_CONTROL_ENABLE            1
#define RTS_CONTROL_ENABLE            1
/*

   -----------------------------
   -- Pseudo terminal library --
   -----------------------------

*/
#define PTY_Library                   ""
/*

   --------------
   -- Families --
   --------------

*/
#define AF_INET                       2
#define AF_INET6                      23
/*

   ------------------
   -- Socket modes --
   ------------------

*/
#define SOCK_STREAM                   1
#define SOCK_DGRAM                    2
/*

   -----------------
   -- Host errors --
   -----------------

*/
#define HOST_NOT_FOUND                11001
#define TRY_AGAIN                     11002
#define NO_DATA                       11004
#define NO_RECOVERY                   11003
/*

   --------------------
   -- Shutdown modes --
   --------------------

*/
#define SHUT_RD                       0
#define SHUT_WR                       1
#define SHUT_RDWR                     2
/*

   ---------------------
   -- Protocol levels --
   ---------------------

*/
#define SOL_SOCKET                    65535
#define IPPROTO_IP                    0
#define IPPROTO_UDP                   17
#define IPPROTO_TCP                   6
/*

   -------------------
   -- Request flags --
   -------------------

*/
#define MSG_OOB                       1
#define MSG_PEEK                      2
#define MSG_EOR                       -1
#define MSG_WAITALL                   8
#define MSG_NOSIGNAL                  -1
#define MSG_Forced_Flags              0
/*
   --  Flags set on all send(2) calls
*/
/*

   --------------------
   -- Socket options --
   --------------------

*/
#define TCP_NODELAY                   1
#define SO_REUSEADDR                  4
#define SO_REUSEPORT                  -1
#define SO_KEEPALIVE                  8
#define SO_LINGER                     128
#define SO_BROADCAST                  32
#define SO_SNDBUF                     4097
#define SO_RCVBUF                     4098
#define SO_SNDTIMEO                   4101
#define SO_RCVTIMEO                   4102
#define SO_ERROR                      4103
#define SO_BUSY_POLL                  -1
#define IP_MULTICAST_IF               9
#define IP_MULTICAST_TTL              10
#define IP_MULTICAST_LOOP             11
#define IP_ADD_MEMBERSHIP             12
#define IP_DROP_MEMBERSHIP            13
#define IP_PKTINFO                    19
/*

   ----------------------
   -- Type definitions --
   ----------------------

*/
/*
   --  Sizes (in bytes) of the components of struct timeval
*/
#define SIZEOF_tv_sec                 4
#define SIZEOF_tv_usec                4
/*

   --  Maximum allowed value for tv_sec
*/
#define MAX_tv_sec                    2 ** (SIZEOF_tv_sec * 8 - 1) - 1
/*

   --  Sizes of various data types
*/
#define SIZEOF_sockaddr_in            16
#define SIZEOF_sockaddr_in6           28
#define SIZEOF_fd_set                 8200
#define FD_SETSIZE                    1024
#define SIZEOF_struct_hostent         32
#define SIZEOF_struct_servent         32
/*

   --  Fields of struct msghdr
*/
#define Msg_Iovlen_T size_t
/*

   ----------------------------------------
   -- Properties of supported interfaces --
   ----------------------------------------

*/
#define Need_Netdb_Buffer             0
#define Need_Netdb_Lock               0
#define Has_Sockaddr_Len              0
#define Thread_Blocking_IO            True
/*
   --  Set False for contexts where socket i/o are process blocking

*/
#define Inet_Pton_Linkname            "__gnat_inet_pton"
/*

   ---------------------
   -- Threads support --
   ---------------------

   --  Clock identifier definitions

*/
#define CLOCK_REALTIME                0
#define CLOCK_MONOTONIC               1
#define CLOCK_THREAD_CPUTIME_ID       3
#define CLOCK_RT_Ada                  CLOCK_REALTIME
/*

   --------------------------------
   -- File and directory support --
   --------------------------------

*/
#define SIZEOF_struct_file_attributes 32
#define SIZEOF_struct_dirent_alloc    269
/*

   ------------------------------
   -- MinGW-specific constants --
   ------------------------------

   --  These constants may be used only within the MinGW version of
   --  GNAT.Sockets.Thin.
*/
#define WSASYSNOTREADY                10091
#define WSAVERNOTSUPPORTED            10092
#define WSANOTINITIALISED             10093
#define WSAEDISCON                    10101
/*

end System.OS_Constants;
*/
