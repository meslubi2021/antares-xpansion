/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once

#include <cstdint>
namespace Memory
{
/*!
** \brief Get the amount of total physical memory
**
** When information about the memory usage can not be retrieved, a default
** value will be used. (see defaultAvailable)
** \return A value in bytes
*/
uint64_t Total();

/*!
** \brief Get the amount of available physical memory
**
** When information about the memory usage can not be retrieved, a default
** value will be used. (see defaultAvailable)
** \return A value in bytes
*/
uint64_t Available();

/*!
** \brief Information about the current memory usage
**
** When information about the current memory usage could not be
** retrieved, some default values are used (see defaultTotal and defaultAvailable).
** This is the case for example when the implementation is missing for the
** current operating system. This method ensures that the values are always valid.
**
** Performance tip: If you're interrested in a single value, you should use
**   the corresponding global routine instead, which will be at least as
**   fast as this this class, sometimes better.
*/
class Usage final
{
 public:
  //! \name Constructor
  //@{
  /*!
    ** \brief Default constructor
  */
  Usage(){
    update();
  }
  /*!
    ** \brief Copy constructor
    **
    ** The values will remain untouched.
  */
  Usage(const Usage& copy) = default;
  //@}

  /*!
    ** \brief Update information about the current memory usage
    **
    ** This routine is automatically called by the constructor.
  */
  bool update();

 public:
  //! The amount of the available physical memory (in bytes)
  uint64_t available = 0;
  //! The amount of the total physical memory (in bytes)
  uint64_t total = 0;

}; // class Usage

} // namespace Memory
