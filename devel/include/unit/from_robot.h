/* Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Willow Garage, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Auto-generated by gensrv_cpp from file /home/intelligentrobotics/rokon_clone/October/src/unit/srv/from_robot.srv
 *
 */


#ifndef UNIT_MESSAGE_FROM_ROBOT_H
#define UNIT_MESSAGE_FROM_ROBOT_H

#include <ros/service_traits.h>


#include <unit/from_robotRequest.h>
#include <unit/from_robotResponse.h>


namespace unit
{

struct from_robot
{

typedef from_robotRequest Request;
typedef from_robotResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct from_robot
} // namespace unit


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::unit::from_robot > {
  static const char* value()
  {
    return "a20adcbdcf70fc177b6fbf8293f1e1b0";
  }

  static const char* value(const ::unit::from_robot&) { return value(); }
};

template<>
struct DataType< ::unit::from_robot > {
  static const char* value()
  {
    return "unit/from_robot";
  }

  static const char* value(const ::unit::from_robot&) { return value(); }
};


// service_traits::MD5Sum< ::unit::from_robotRequest> should match 
// service_traits::MD5Sum< ::unit::from_robot > 
template<>
struct MD5Sum< ::unit::from_robotRequest>
{
  static const char* value()
  {
    return MD5Sum< ::unit::from_robot >::value();
  }
  static const char* value(const ::unit::from_robotRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::unit::from_robotRequest> should match 
// service_traits::DataType< ::unit::from_robot > 
template<>
struct DataType< ::unit::from_robotRequest>
{
  static const char* value()
  {
    return DataType< ::unit::from_robot >::value();
  }
  static const char* value(const ::unit::from_robotRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::unit::from_robotResponse> should match 
// service_traits::MD5Sum< ::unit::from_robot > 
template<>
struct MD5Sum< ::unit::from_robotResponse>
{
  static const char* value()
  {
    return MD5Sum< ::unit::from_robot >::value();
  }
  static const char* value(const ::unit::from_robotResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::unit::from_robotResponse> should match 
// service_traits::DataType< ::unit::from_robot > 
template<>
struct DataType< ::unit::from_robotResponse>
{
  static const char* value()
  {
    return DataType< ::unit::from_robot >::value();
  }
  static const char* value(const ::unit::from_robotResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // UNIT_MESSAGE_FROM_ROBOT_H
