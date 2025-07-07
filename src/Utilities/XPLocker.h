/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#pragma once

class XPLocker
{
  public:
    static XPLocker& instance();

    // delete copy and move constructors and assign operators
    XPLocker(XPLocker const&)            = delete; // Copy construct
    XPLocker(XPLocker&&)                 = delete; // Move construct
    XPLocker& operator=(XPLocker const&) = delete; // Copy assign
    XPLocker& operator=(XPLocker&&)      = delete; // Move assign

    void entry(const char* file, const char* function, int line);
    void exit(const char* file, const char* function, int line);

  protected:
    XPLocker();
    ~XPLocker();
};
