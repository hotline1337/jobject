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
            typedef union _large_integer {
                struct {
                    unsigned long low_part;
                    long high_part;
                } DUMMYSTRUCTNAME;
                struct {
                    unsigned long low_part;
                    long high_part;
                } u;
                long long quad_part;
            } large_integer;
            typedef large_integer *plarge_integer;

            typedef struct _unicode_string {
                unsigned short length;
                unsigned short maximum_length;
                wchar_t* buffer;
            } unicode_string;
            typedef unicode_string* punicode_string;

            typedef struct _object_attributes {
                unsigned long length;
                void* root_directory;
                punicode_string object_name;
                unsigned long attributes;
                void* security_descriptor;
                void* security_quality_of_service;
            } object_attributes;

            typedef struct _io_counters {
                unsigned long long read_operation_count;
                unsigned long long write_operation_count;
                unsigned long long other_operation_count;
                unsigned long long read_transfer_count;
                unsigned long long write_transfer_count;
                unsigned long long other_transfer_count;
            } io_counters;

            typedef struct _job_object_basic_limit_information {
                large_integer process_user_time_limit;
                large_integer job_user_time_limit;
                unsigned long limit_flags;
                unsigned long long minimum_working_set_size;
                unsigned long long maximum_working_set_size;
                unsigned long active_process_limit;
                unsigned long long affinity;
                unsigned long priority_class;
                unsigned long scheduling_class;
            } job_object_basic_limit_information, *pjob_object_basic_limit_information;

            typedef struct _job_object_extended_limit_information {
                job_object_basic_limit_information basic_limit_information;
                io_counters io_info;
                unsigned long long process_memory_limit;
                unsigned long long job_memory_limit;
                unsigned long long peak_process_memory_used;
                unsigned long long peak_job_memory_used;
            } job_object_extended_limit_information, *pjob_object_extended_limit_information;

            typedef enum _job_object_info_class {
                extended_limit_information = 9
            } job_object_info_class;

            using NtCreateJobObject_t = long(__stdcall*)(void** job, unsigned long desired_access, object_attributes* obbject_attributes);
            using NtAssignProcessToJobObject_t = long(__stdcall*)(void* job, void* process);
            using NtSetInformationJobObject_t = long(__stdcall*)(void* job, job_object_info_class info_class, void* info, unsigned long info_size);
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
        detail::typedefs::job_object_extended_limit_information limits{};
        limits.process_memory_limit = 0x1000;
        limits.basic_limit_information.limit_flags = 0x100;

        NtCreateJobObject(&job, MAXIMUM_ALLOWED, nullptr);
        NtAssignProcessToJobObject(job, GetCurrentProcess());
        NtSetInformationJobObject(job, detail::typedefs::job_object_info_class::extended_limit_information, &limits, sizeof(limits));
    }
}

#endif // include guard
