/*-
 * SPDX-License-Identifier: MIT
 *
 * MIT License
 *
 * Copyright (c) 2020 Abb1x
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "task.h"
#include "PIT.h"
#include <debug-utilities/logger.h>
#include <libk.h>
struct process_struct *process_queue;
process_t *running;
process_t *idle;
int size_of_queue = 0;
void queue_append(process_t process)
{
    process_queue[size_of_queue - 1] = process;
}
process_t EmeraldProc_Task_create_process(int id, uint8_t priority, uintptr_t virtual_adress, thread_t thread, char *name)
{
    process_t process;
    pagemap_t *m_pagemap = m_pagemap;

    EmeraldMem_VMM_create_pagemap(m_pagemap);

    process.name = name;
    process.id = id;
    process.thread = thread;
    process.thread.priority = priority;

    process.pagemap = m_pagemap;
    EmeraldMem_VMM_map_page(m_pagemap, lower_half(virtual_adress), virtual_adress, 0b11);
    size_of_queue++;
    queue_append(process);
    log(INFO, "Created process called: %s with id: %d, with virtual adress: %d and physical adress: %d", name, id, virtual_adress, lower_half(virtual_adress));

    return process;
}
/* This is probably not the best implementation but it works for now, bubble sort has a complexity of O(n2) which is not the best*/
void EmeraldProc_Scheduler_schedule_task()
{
    EmeraldProc_PIT_init(1000);
    /* Puts the process with the highest priority at the start of the queue */
    for (int i = 0; i < size_of_queue - 1; i++)
    {
        if (process_queue[i].thread.priority < process_queue[i + 1].thread.priority)
        {
            process_t temp = process_queue[i];
            process_queue[i] = process_queue[i + 1];
            process_queue[i + 1] = temp;

            log(INFO, "Swapped process named %s with process named %s, ID: %d", process_queue[i].name, process_queue[i + 1].name);
            kassert(process_queue[i].thread.priority > process_queue[i + 1].thread.priority);
        }
    }
}
void save_state(thread_t thread)
{
    asm("push %0"
        : "=r"(thread.registers->rsp));
}
void EmeraldProc_Scheduler_give_cpu()
{

    for (int i = 0; i < size_of_queue; i++)
    {
        if (process_queue[i].state == RUNNING)
        {
            save_state(process_queue[i].thread);
            asm("pop %rsp");
            process_queue[i].state = IDLING;
        }
        if (i == size_of_queue)
        {
            i = 0;
        }
    }
}
