/*
 * Copyright - hotline1337
 *
 * Licensed under the GNU General Public License, Version 3.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JOBJECT_HPP
#define JOBJECT_HPP

#pragma region JOBJECT_INCLUDES
#include <libloaderapi.h>
#pragma endregion JOBJECT_INCLUDES

#if defined(_MSC_VER)
#define JOBJECT_FORCEINLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
#define JOBJECT_FORCEINLINE __attribute__((always_inline)) inline
#else
#define JOBJECT_FORCEINLINE inline
#endif

namespace jobject
{
    namespace detail
    {
        namespace typedefs
        {
            typedef struct _UNICODE_STRING {
                USHORT Length;
                USHORT MaximumLength;
                PWSTR  Buffer;
            } UNICODE_STRING;
            typedef UNICODE_STRING* PUNICODE_STRING;

            typedef struct _OBJECT_ATTRIBUTES {
                ULONG Length;
                HANDLE RootDirectory;
                PUNICODE_STRING ObjectName;
                ULONG Attributes;
                PVOID SecurityDescriptor;
                PVOID SecurityQualityOfService;
            } OBJECT_ATTRIBUTES;

            using NtCreateJobObject_t = long(__stdcall*)(HANDLE* job, ACCESS_MASK desired_access, OBJECT_ATTRIBUTES* obbject_attributes);
            using NtAssignProcessToJobObject_t = long(__stdcall*)(HANDLE job, HANDLE process);
            using NtSetInformationJobObject_t = long(__stdcall*)(HANDLE job, JOBOBJECTINFOCLASS info_class, void* info, ULONG info_size);
        }

        template <typename T>
        JOBJECT_FORCEINLINE const T get(const char* func)
        {
            static const auto nt_handle = GetModuleHandle("ntdll.dll");
            return reinterpret_cast<T>(GetProcAddress(nt_handle, func));
        }
    }
    JOBJECT_FORCEINLINE const auto init()
    {
        /* set apis */
        const static auto NtCreateJobObject = detail::get<detail::typedefs::NtCreateJobObject_t>("NtCreateJobObject");
        const static auto NtAssignProcessToJobObject = detail::get<detail::typedefs::NtAssignProcessToJobObject_t>("NtAssignProcessToJobObject");
        const static auto NtSetInformationJobObject = detail::get<detail::typedefs::NtSetInformationJobObject_t>("NtSetInformationJobObject");

        /* execute */
        void* job = nullptr;
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
        limits.ProcessMemoryLimit = 0x1000;
        limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_MEMORY;

        NtCreateJobObject(&job, MAXIMUM_ALLOWED, nullptr);
        NtAssignProcessToJobObject(job, GetCurrentProcess());
        NtSetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits));
    }
}

#endif // include guard